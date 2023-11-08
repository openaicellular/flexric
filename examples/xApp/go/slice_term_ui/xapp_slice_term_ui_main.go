package main

import "C"
import (
	mac "build/examples/xApp/go/FlexPolicy/utils/sm/mac"
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
	xapp "build/examples/xApp/go/xapp_sdk"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"time"

	"github.com/gin-gonic/gin"
	"net/http"

	"github.com/rthornton128/goncurses"
	"sync"
	//"strings"
)

var router *gin.Engine
var srv *http.Server
var client *http.Client
var serverFinished bool

// Mutex for locking the global structure
var mutex sync.Mutex

// ------------------------------------------------------------------------ //
//
//	Global Structure for storing the latest indication message
//
// ------------------------------------------------------------------------ //
var sliceStats = map[string]interface{}{
	"RAN": map[string]interface{}{
		"dl": map[string]interface{}{},
	},
	"UE": map[string]interface{}{},
}

// ------------------------------------------------------------------------ //
//
//	Function for reading desired stats items from the global structure
//
// ------------------------------------------------------------------------ //
func readSliceStats(item string, sliceId int) interface{} {

	switch item {
	case "RAN":
		mutex.Lock()
		value := sliceStats["RAN"]
		mutex.Unlock()
		return value
	case "UE":
		mutex.Lock()
		value := sliceStats["UE"]
		mutex.Unlock()
		return value
	case "RAN_DL":
		mutex.Lock()
		value := sliceStats["RAN"].(map[string]interface{})["dl"]
		mutex.Unlock()
		return value
	case "num_of_slices":
		mutex.Lock()
		value := sliceStats["RAN"].(map[string]interface{})["dl"].(map[string]interface{})["num_of_slices"].(uint)
		mutex.Unlock()
		return value
	case "ues":
		mutex.Lock()
		value := sliceStats["UE"].(map[string]interface{})["ues"]
		mutex.Unlock()
		return value
	case "num_of_ues":
		// Check whether we need to get the number of UEs of all the slices or only the number of UEs of a specific slice
		if sliceId == -1 {
			// Get the number of UEs of all the slices
			mutex.Lock()
			value := sliceStats["UE"].(map[string]interface{})["num_of_ues"].(uint)
			mutex.Unlock()
			return value
		} else if sliceId >= 0 {
			mutex.Lock()
			// Get the number of UEs of a specific slice
			num_of_ues_per_slice := 0

			numOfUes := sliceStats["UE"].(map[string]interface{})["num_of_ues"].(uint)

			if numOfUes == 0 {
				mutex.Unlock()
				return 0
			}

			var i uint
			for i = 0; i < numOfUes; i++ {
				currUe := sliceStats["UE"].(map[string]interface{})["ues"].([]map[string]interface{})[i]
				currAssocDlSliceId := currUe["assoc_dl_slice_id"].(string)

				// Convert the string to byte
				b := []byte(currAssocDlSliceId)[0]

				// Convert the byte to integer
				currAssocDlSliceIdInt := int(b)

				if int(currAssocDlSliceIdInt) == sliceId {
					num_of_ues_per_slice++
				}
			}
			mutex.Unlock()
			return num_of_ues_per_slice
		} else {
			fmt.Println("ERROR: Invalid sliceId")
			return nil
		}
	case "rntis":
		// Check whether we need to get all the RNTIs or only the RNTIs of a specific slice
		if sliceId == -1 {
			mutex.Lock()
			// Get all the RNTIs
			rntis := []uint16{}
			numOfUes := sliceStats["UE"].(map[string]interface{})["num_of_ues"].(uint)
			var i uint
			for i = 0; i < numOfUes; i++ {
				currUe := sliceStats["UE"].(map[string]interface{})["ues"].([]map[string]interface{})[i]
				rnti := currUe["rnti"].(uint16)

				// rntiInt, err := strconv.ParseInt(rnti, 16, 64)
				// if err != nil {
				// 	fmt.Println("Error:", err)
				// 	mutex.Unlock()
				// 	return nil
				// }
				rntis = append(rntis, rnti)
			}
			mutex.Unlock()
			return rntis
		} else if sliceId >= 0 {
			mutex.Lock()
			// Get the RNTIs of a specific slice
			rntis := []uint16{}
			numOfUes := sliceStats["UE"].(map[string]interface{})["num_of_ues"].(uint)
			var i uint
			for i = 0; i < numOfUes; i++ {
				currUe := sliceStats["UE"].(map[string]interface{})["ues"].([]map[string]interface{})[i]
				currAssocDlSliceId := currUe["assoc_dl_slice_id"].(string)

				// Convert the string to byte
				b := []byte(currAssocDlSliceId)[0]

				// Convert the byte to integer
				currAssocDlSliceIdInt := int(b)

				if int(currAssocDlSliceIdInt) == sliceId {
					rnti := currUe["rnti"].(uint16)

					// rntiInt, err := strconv.ParseInt(rnti, 16, 64)
					// if err != nil {
					// 	fmt.Println("Error:", err)
					// 	mutex.Unlock()
					// 	return nil
					// }
					rntis = append(rntis, rnti)
				}
			}
			mutex.Unlock()
			return rntis
		} else {
			fmt.Println("ERROR: Invalid sliceId")
			return nil
		}
		return nil

	default:
		fmt.Println("ERROR: Invalid item")
		return nil
	}
}

// ------------------------------------------------------------------------ //
//
//	SLICE INDICATION MSG TO JSON
//
// ------------------------------------------------------------------------ //
func sliceIndToDictJSON(ind xapp.Swig_slice_ind_msg_t) {
	mutex.Lock()
	dlDict := sliceStats["RAN"].(map[string]interface{})["dl"].(map[string]interface{})

	if ind.GetSlice_stats().GetDl().GetLen_slices() <= 0 {
		dlDict["num_of_slices"] = ind.GetSlice_stats().GetDl().GetLen_slices()
		dlDict["slice_sched_algo"] = "null"
		dlDict["ue_sched_algo"] = ind.GetSlice_stats().GetDl().GetSched_name().Get(0)
	} else {
		dlDict["num_of_slices"] = ind.GetSlice_stats().GetDl().GetLen_slices()
		dlDict["slice_sched_algo"] = "null"
		dlDict["slices"] = []map[string]interface{}{}

		var sliceAlgo string = ""

		for i := 0; i < int(ind.GetSlice_stats().GetDl().GetSlices().Size()); i++ {
			// obtain current slice
			var currSlice = ind.GetSlice_stats().GetDl().GetSlices().Get(i)
			// obtain current slice's parameters type
			var sliceType = currSlice.GetParams().GetXtype()

			switch sliceType {
			case 1:
				sliceAlgo = "STATIC"
			case 2:
				sliceAlgo = "NVS"
			case 4:
				sliceAlgo = "EDF"
			default:
				sliceAlgo = "unknown"
			}
			dlDict["slice_sched_algo"] = sliceAlgo

			sliceDict := map[string]interface{}{
				"index":         int(currSlice.GetId()),
				"label":         currSlice.GetLabel().Get(0),
				"ue_sched_algo": currSlice.GetSched().Get(0),
			}

			if dlDict["slice_sched_algo"] == "STATIC" {
				sliceDict["slice_algo_params"] = map[string]interface{}{
					"pos_low":  currSlice.GetParams().GetU().GetSta().GetPos_low(),
					"pos_high": currSlice.GetParams().GetU().GetSta().GetPos_high(),
				}
			} else if dlDict["slice_sched_algo"] == "NVS" {
				if currSlice.GetParams().GetU().GetNvs().GetConf() == 0 {
					sliceDict["slice_algo_params"] = map[string]interface{}{
						"type":      "RATE",
						"mbps_rsvd": currSlice.GetParams().GetU().GetNvs().GetU().GetRate().GetU1().GetMbps_required(),
						"mbps_ref":  currSlice.GetParams().GetU().GetNvs().GetU().GetRate().GetU2().GetMbps_reference(),
					}
				} else if currSlice.GetParams().GetU().GetNvs().GetConf() == 1 {
					sliceDict["slice_algo_params"] = map[string]interface{}{
						"type":     "CAPACITY",
						"pct_rsvd": currSlice.GetParams().GetU().GetNvs().GetU().GetCapacity().GetU().GetPct_reserved(),
					}
				} else {
					sliceDict["slice_algo_params"] = map[string]interface{}{
						"type": "unknown",
					}
				}
			} else if dlDict["slice_sched_algo"] == "EDF" {
				sliceDict["slice_algo_params"] = map[string]interface{}{
					"deadline":        currSlice.GetParams().GetU().GetEdf().GetDeadline(),
					"guaranteed_prbs": currSlice.GetParams().GetU().GetEdf().GetGuaranteed_prbs(),
					"max_replenish":   currSlice.GetParams().GetU().GetEdf().GetMax_replenish(),
				}
			} else {
				fmt.Println("unknown slice algorithm, cannot handle params")
			}
			dlDict["slices"] = append(dlDict["slices"].([]map[string]interface{}), sliceDict)
		}

	}
	ueDict := sliceStats["UE"].(map[string]interface{})
	if ind.GetUe_slice_stats().GetLen_ue_slice() <= 0 {
		ueDict["num_of_ues"] = ind.GetUe_slice_stats().GetLen_ue_slice()
	} else {
		ueDict["num_of_ues"] = ind.GetUe_slice_stats().GetLen_ue_slice()
		ueDict["ues"] = []map[string]interface{}{}

		for i := 0; i < int(ind.GetUe_slice_stats().GetUes().Size()); i++ {
			// obtain current ue
			var currUe = ind.GetUe_slice_stats().GetUes().Get(i)

			var dlId string
			if currUe.GetDl_id() >= 0 && dlDict["num_of_slices"].(uint) > 0 {
				dlId = string(currUe.GetDl_id())
			} else {
				dlId = "Null"
			}

			uesDict := map[string]interface{}{
				"rnti":              currUe.GetRnti(),
				"assoc_dl_slice_id": dlId,
			}
			ueDict["ues"] = append(ueDict["ues"].([]map[string]interface{}), uesDict)
		}
	}

	indDict := sliceStats

	file, _ := os.Create("rt_slice_stats.json")
	defer file.Close()

	indJson, _ := json.Marshal(indDict)
	file.Write(indJson)
	mutex.Unlock()
}

// ------------------------------------------------------------------------ //
//
//	SLICE INDICATION CALLBACK
//
// ------------------------------------------------------------------------ //
type SLICECallback struct {
}

func (c SLICECallback) Handle(ind xapp.Swig_slice_ind_msg_t) {
	slice.SliceIndToDictJSON(ind)
}

// ------------------------------------------------------------------------ //
//
//	SLICE CONTROL FUNCS
//
// ------------------------------------------------------------------------ //
func CreateSlice(sliceParams Slice, sliceSchedAlgo string) xapp.Fr_slice_t {
	newSlice := xapp.NewFr_slice_t()

	newSlice.SetId(uint(sliceParams.Id))
	newSlice.SetLabel(sliceParams.Label)
	newSlice.SetLen_label(uint(len(sliceParams.Label)))
	newSlice.SetSched(sliceParams.UeSchedAlgo)
	newSlice.SetLen_sched(uint(len(sliceParams.UeSchedAlgo)))

	if sliceSchedAlgo == "STATIC" {
		params := xapp.NewSlice_params_t()
		sliceU := xapp.NewSlice_params_u()
		sta := xapp.NewStatic_slice_t()

		sta.SetPos_low(uint(sliceParams.SliceAlgoParams.PosLow))
		sta.SetPos_high(uint(sliceParams.SliceAlgoParams.PosHigh))

		sliceU.SetSta(sta)
		params.SetXtype(xapp.Slice_algorithm_e(xapp.SLICE_ALG_SM_V0_STATIC))
		params.SetU(sliceU)
		newSlice.SetParams(params)
	} else if sliceSchedAlgo == "NVS" {
		params := xapp.NewSlice_params_t()

		params.SetXtype(xapp.Slice_algorithm_e(xapp.SLICE_ALG_SM_V0_NVS))

		if sliceParams.Type == "SLICE_SM_NVS_V0_RATE" {
			sliceU := xapp.NewSlice_params_u()
			nvs := xapp.NewNvs_slice_t()
			nvsSliceU := xapp.NewNvs_slice_u()
			rate := xapp.NewNvs_rate_t()
			u1 := xapp.NewNvs_rate_req_u()
			u2 := xapp.NewNvs_rate_ref_u()

			u1.SetMbps_required(float32(sliceParams.SliceAlgoParams.MbpsRsvd))
			u2.SetMbps_reference(float32(sliceParams.SliceAlgoParams.MbpsRef))

			rate.SetU1(u1)
			rate.SetU2(u2)

			nvsSliceU.SetRate(rate)

			nvs.SetConf(xapp.Nvs_slice_conf_e(xapp.SLICE_SM_NVS_V0_RATE))
			nvs.SetU(nvsSliceU)

			sliceU.SetNvs(nvs)

			params.SetU(sliceU)

			newSlice.SetParams(params)
		} else if sliceParams.Type == "SLICE_SM_NVS_V0_CAPACITY" {
			sliceU := xapp.NewSlice_params_u()
			nvs := xapp.NewNvs_slice_t()
			nvsSliceU := xapp.NewNvs_slice_u()
			capacity := xapp.NewNvs_capacity_t()
			capacityU := xapp.NewNvs_capacity_u()

			capacityU.SetPct_reserved(float32(sliceParams.SliceAlgoParams.PctRsvd))

			capacity.SetU(capacityU)
			nvsSliceU.SetCapacity(capacity)
			nvs.SetConf(xapp.Nvs_slice_conf_e(xapp.SLICE_SM_NVS_V0_CAPACITY))
			nvs.SetU(nvsSliceU)
			sliceU.SetNvs(nvs)
			params.SetU(sliceU)
			newSlice.SetParams(params)
		} else {
			log.Println("ERROR: Unknown NVS slice type")
		}
	} else if sliceSchedAlgo == "EDF" {
		params := xapp.NewSlice_params_t()
		sliceU := xapp.NewSlice_params_u()
		edf := xapp.NewEdf_slice_t()

		edf.SetDeadline(uint(sliceParams.SliceAlgoParams.Deadline))
		edf.SetGuaranteed_prbs(uint(sliceParams.SliceAlgoParams.GuaranteedPrbs))
		edf.SetMax_replenish(uint(sliceParams.SliceAlgoParams.MaxReplenish))

		sliceU.SetEdf(edf)

		params.SetXtype(xapp.Slice_algorithm_e(xapp.SLICE_ALG_SM_V0_EDF))
		params.SetU(sliceU)
	} else {
		log.Println("ERROR: Unknown slice scheduling algorithm")
	}

	return newSlice
}

// ------------------------------------------------------------------------ //
//
//	SLICE CONTROL PARAMETER STRUCTURES
//
// ------------------------------------------------------------------------ //
type SliceAlgoParams struct {
	PosLow  int `json:"pos_low,omitempty"`
	PosHigh int `json:"pos_high,omitempty"`

	MbpsRsvd int `json:"mbps_rsvd,omitempty"`
	MbpsRef  int `json:"mbps_ref,omitempty"`

	PctRsvd float64 `json:"pct_rsvd,omitempty"`

	Deadline       int `json:"deadline"`
	GuaranteedPrbs int `json:"guaranteed_prbs"`
	MaxReplenish   int `json:"max_replenish"`
}

type Slice struct {
	Id              int             `json:"id"`
	Label           string          `json:"label"`
	UeSchedAlgo     string          `json:"ue_sched_algo"`
	Type            string          `json:"type,omitempty"`
	SliceAlgoParams SliceAlgoParams `json:"slice_algo_params,omitempty"`
}

type Ue struct {
	Rnti           uint16 `json:"rnti"` // TODO: get rnti from slice_ind_to_dict_json()
	AssocDlSliceId int    `json:"assoc_dl_slice_id"`
}

type Request struct {
	NumSlices       int     `json:"num_slices,omitempty"`
	SliceSchedAlgo  string  `json:"slice_sched_algo,omitempty"`
	Slices          []Slice `json:"slices,omitempty"`
	NumDlSlices     int     `json:"num_dl_slices,omitempty"`
	DeleteDlSliceId []int   `json:"delete_dl_slice_id"`

	NumUes int  `json:"num_ues,omitempty"`
	Ues    []Ue `json:"ues,omitempty"`
}

// ------------------------------------------------------------------------ //
//
//	Function to fill the slice control message
//
// ------------------------------------------------------------------------ //
func FillSliceCtrlMsg(ctrlType string, ctrlMsg Request) xapp.Slice_ctrl_msg_t {
	msg := xapp.NewSlice_ctrl_msg_t()

	if ctrlType == "ADDMOD" {
		msg.SetXtype(xapp.Slice_ctrl_msg_e(xapp.SLICE_CTRL_SM_V0_ADD))
		dl := xapp.NewUl_dl_slice_conf_t()
		//TODO: UL SLICE CTRL ADD
		//ul = ric.ul_dl_slice_conf_t()
		//ue_sched_algo can be "RR"(round-robin), "PF"(proportional fair) or "MT"(maximum throughput) and it has to be set in any len_slices

		var ueSchedAlgo string = "PF"
		dl.SetSched_name(ueSchedAlgo)
		dl.SetLen_sched_name(uint(len(ueSchedAlgo)))

		dl.SetLen_slices(uint(ctrlMsg.NumSlices))

		slices := xapp.NewSlice_array(ctrlMsg.NumSlices)
		//slices = xapp.Slice_array(ctrlMsg.(map[string]interface{})["slices"].(xapp.Slice_array))

		for i := 0; i < ctrlMsg.NumSlices; i++ {
			slices.Setitem(i, CreateSlice(ctrlMsg.Slices[i], ctrlMsg.SliceSchedAlgo))
		}

		dl.SetSlices(slices.Cast())

		u := xapp.NewSlice_ctrl_msg_u()
		addModSlice := xapp.NewSlice_conf_t()

		addModSlice.SetDl(dl)
		u.SetAdd_mod_slice(addModSlice)
		msg.SetU(u)

	} else if ctrlType == "DEL" {
		msg.SetXtype(xapp.Slice_ctrl_msg_e(xapp.SLICE_CTRL_SM_V0_DEL))
		u := xapp.NewSlice_ctrl_msg_u()
		delSlice := xapp.NewDel_slice_conf_t()

		delSlice.SetLen_dl(uint(ctrlMsg.NumDlSlices))

		del_dl_id := xapp.NewDel_dl_array(ctrlMsg.NumDlSlices)

		for i := 0; i < ctrlMsg.NumDlSlices; i++ {
			del_dl_id.Setitem(i, uint(ctrlMsg.DeleteDlSliceId[i]))
		}

		delSlice.SetDl(del_dl_id.Cast())
		u.SetDel_slice(delSlice)
		msg.SetU(u)

	} else if ctrlType == "ASSOC_UE_SLICE" {
		msg.SetXtype(xapp.Slice_ctrl_msg_e(xapp.SLICE_CTRL_SM_V0_UE_SLICE_ASSOC))

		u := xapp.NewSlice_ctrl_msg_u()
		ueSlice := xapp.NewUe_slice_conf_t()

		ueSlice.SetLen_ue_slice(uint(ctrlMsg.NumUes))

		assoc := xapp.NewUe_slice_assoc_array(ctrlMsg.NumUes)
		for i := 0; i < ctrlMsg.NumUes; i++ {
			a := xapp.NewUe_slice_assoc_t()
			a.SetRnti(uint16(ctrlMsg.Ues[i].Rnti))
			a.SetDl_id(uint(ctrlMsg.Ues[i].AssocDlSliceId))
			// TODO: UL SLICE CTRL ASSOC
			// a.ul_id = 0
			assoc.Setitem(i, a)
		}

		ueSlice.SetUes(assoc.Cast())
		u.SetUe_slice(ueSlice)
		msg.SetU(u)
	}

	return msg
}

// Indication Callback MAC Function
type MACCallback struct {
}

func (mac_cb MACCallback) Handle(ind xapp.Swig_mac_ind_msg_t) {
	mac.Calculate_UE_PRB_utilisation(ind)
	mac.CalculateUeThroughput(ind)
	mac.FillMacStorage(ind)
}

var end bool = false

// Global or main-scope variables to hold past metrics.
var prbData []int
var thptData []int

const maxDataPoints = 10 // Number of past data points to display.

// ------------------------------------------------------------------------ //
//
//	MAIN
//
// ------------------------------------------------------------------------ //
var conn xapp.E2NodeVector
var nodeIdx int

func main() {

	//sliceId := 0
	idleSliceId := 1

	xapp.Init(xapp.SlToStrVec(os.Args))

	// Connect
	conn := xapp.Conn_e2_nodes()

	var nodes_len int64 = conn.Size()

	if nodes_len <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	nodeIdx = 0

	// ----------------------- SLICE INDICATION----------------------- //
	inner := SLICECallback{}
	callback := xapp.NewDirectorSlice_cb(inner)
	hndlr := xapp.Report_slice_sm(conn.Get(nodeIdx).GetId(), xapp.Interval_ms_10, callback)
	time.Sleep(1 * time.Second)

	// ----------------------- MAC Indication ----------------------- //
	innerMac := MACCallback{}
	callbackMac := xapp.NewDirectorMac_cb(innerMac)
	HndlrMac := xapp.Report_mac_sm(conn.Get(nodeIdx).GetId(), xapp.Interval_ms_10, callbackMac)

	// ---------------- Print Slice - UE Association ---------------- //
	time.Sleep(1 * time.Second)
	// Initialize ncurses
	stdscr, _ := goncurses.Init()
	defer goncurses.End()

	// Enable keyboard input
	stdscr.Keypad(true)

	// Hide cursor
	goncurses.Cursor(0)

	// Set up colors
	if goncurses.HasColors() {
		goncurses.StartColor()
		goncurses.InitPair(1, goncurses.C_RED, goncurses.C_BLACK)
	}

	// Clear the screen
	stdscr.Clear()

	go func() {
		for end == false {
			// Check if the user entered "quit" or "q"
			input := stdscr.GetChar()
			if input == 'q' || input == 'Q' {
				// Send a quit signal through the channel
				goncurses.End()
				end = true
				break
			}
		}
	}()

	for end == false {
		fmt.Println("e")
		// Create a wrapper screen with dimensions based on the terminal size
		wrapper := stdscr.Sub(0, 0, 0, 0)

		// Clear the wrapper screen
		wrapper.Clear()

		// Get the dimensions of the wrapper screen
		height, width := wrapper.MaxYX()

		// Calculate the vertical and horizontal center positions
		centerY := height / 2 // Adjusted to 1/3 to better position plots
		centerX := width / 2  // No change

		// Universal statistics
		e2nodeId := slice.E2NodeId{
			Mcc:     conn.Get(nodeIdx).GetId().GetPlmn().GetMcc(),
			Mnc:     conn.Get(nodeIdx).GetId().GetPlmn().GetMnc(),
			NbId:    uint32(conn.Get(nodeIdx).GetId().GetNb_id().GetNb_id()),
			RanType: xapp.Get_e2ap_ngran_name(conn.Get(nodeIdx).GetId().GetXtype()),
		}

		// Update the slices

		reading := slice.ReadSliceStats("multiple_rntis_num_of_ues", idleSliceId, e2nodeId)

		normalSliceNumUes := reading.(map[string]interface{})["num_of_normal_ues"].(int)
		normalSliceRntis := reading.(map[string]interface{})["normal_rntis"].([]uint16)

		idleSliceNumUes := reading.(map[string]interface{})["num_of_idle_ues"].(int)
		idleSliceRntis := reading.(map[string]interface{})["idle_rntis"].([]uint16)
		//normalSliceNumUes := slice.ReadSliceStats("num_of_ues", sliceId, e2nodeId).(int)
		//normalSliceRntis := slice.ReadSliceStats("rntis", sliceId, e2nodeId)
		//
		//idleSliceNumUes := slice.ReadSliceStats("num_of_ues", 1, e2nodeId).(int)
		//idleSliceRntis := slice.ReadSliceStats("rntis", 1, e2nodeId)

		CurrDlThpt, _ := mac.TotalThroughput(e2nodeId)
		CurrPrbUtilization := mac.TotalPrbUtilization(e2nodeId)

		// Update the historical data
		updateHistoricalData(&prbData, CurrPrbUtilization, maxDataPoints)
		updateHistoricalData(&thptData, CurrDlThpt, maxDataPoints)

		printTimeSeriesPlot(wrapper, int(float64(width)*0.05), int(float64(height)*0.1), "DL Throughput (Mbps):", thptData, 200) // max throughput
		printTimeSeriesPlot(wrapper, int(float64(width)*0.05), int(float64(height)*0.6), "PRB Utilization (%):", prbData, 100)   // max prb

		// Adjust the vertical position to prevent overlap
		printSliceInfo(wrapper, centerX+int(float64(width)*0.1), centerY-int(float64(height)*0.2), "[Slice 0]:", normalSliceRntis, normalSliceNumUes) // Slice A
		printSliceInfo(wrapper, centerX+int(float64(width)*0.1), centerY+int(float64(height)*0.2), "[Slice Idle]:", idleSliceRntis, idleSliceNumUes)  // Slice B

		// Refresh the screen to display changes
		stdscr.Refresh()

		// Refresh the wrapper screen to display changes
		wrapper.Refresh()

		// Wait for a short duration
		time.Sleep(1000 * time.Millisecond)
	}

	// ----------------------- END ----------------------- //
	xapp.Rm_report_slice_sm(hndlr)
	xapp.Rm_report_mac_sm(HndlrMac)

	// --------------------------------------------------------- //
	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}

// printSliceInfo prints the number of UEs and their RNTIs for a given slice
func printSliceInfo(stdscr *goncurses.Window, x int, y int, title string, rntis []uint16, numOfUes int) {
	stdscr.MovePrint(y, x-len(title)/2, title)

	stdscr.MovePrint(y+1, x-(len("Number of UEs:")-len("UE RNTIs:")), fmt.Sprintf("Number of UEs: %d", numOfUes))

	rntis_string := "UE RNTIs: "
	for _, ue := range rntis {
		rntis_string += fmt.Sprintf("[%d]  ", ue)
	}
	stdscr.MovePrint(y+2, x, rntis_string)

}

// Update the array holding historical data.
func updateHistoricalData(data *[]int, newValue int, maxLength int) {
	*data = append(*data, newValue)
	if len(*data) > maxLength {
		*data = (*data)[1:]
	}
}

// Print the time-series data.
func printTimeSeries(stdscr *goncurses.Window, x int, y int, title string, data []int) {
	dataStr := "  "
	for _, value := range data {
		dataStr += fmt.Sprintf("%d, ", value)
	}
	stdscr.MovePrint(y, x, title+dataStr)
}

func printTimeSeriesPlot(stdscr *goncurses.Window, startX int, startY int, title string, data []int, maxVal int) {
	horizontalBorder := goncurses.Char('-')
	verticalBorder := goncurses.Char('|')
	corner := goncurses.Char('+')

	// Define padding space
	horizontalPadding := 2
	verticalPadding := 1

	// Adjust the startX and startY to include the padding
	paddedStartX := startX + horizontalPadding
	paddedStartY := startY + verticalPadding

	stdscr.MovePrint(startY-1, startX-1, title)

	maxBarHeight := 7
	maxBarValue := maxVal
	stepSize := 3

	// Draw border
	stdscr.MovePrint(startY, startX-1, string(corner))
	stdscr.HLine(startY, startX, horizontalBorder, len(data)*stepSize+horizontalPadding)
	stdscr.MovePrint(startY, startX+len(data)*stepSize+horizontalPadding, string(corner))
	stdscr.VLine(startY+1, startX-1, verticalBorder, maxBarHeight)
	stdscr.VLine(startY+1, startX+len(data)*stepSize+horizontalPadding, verticalBorder, maxBarHeight)

	// Loop through data and print bars vertically
	for x, value := range data {
		scaledValue := (value * maxBarHeight) / maxBarValue
		for y := 0; y < scaledValue; y++ {
			stdscr.MovePrint(paddedStartY+maxBarHeight-y-1, paddedStartX+x*stepSize, "|")
		}
		if scaledValue > 0 {
			stdscr.MovePrint(paddedStartY+maxBarHeight-scaledValue-1, paddedStartX+x*stepSize, fmt.Sprintf("%d", value))
		}
	}

	// Close bottom border
	stdscr.MovePrint(startY+maxBarHeight+1, startX-1, string(corner))
	stdscr.HLine(startY+maxBarHeight+1, startX, horizontalBorder, len(data)*stepSize+horizontalPadding)
	stdscr.MovePrint(startY+maxBarHeight+1, startX+len(data)*stepSize+horizontalPadding, string(corner))
}
