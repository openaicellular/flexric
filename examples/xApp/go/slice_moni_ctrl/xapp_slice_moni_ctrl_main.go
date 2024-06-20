package main

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"sync"
	"time"
)

// Mutex for locking the global structure SliceStatsDict
var Mutex sync.Mutex

// ------------------------------------------------------------------------ //
//  SLICE INDICATION CALLBACK
// ------------------------------------------------------------------------ //

type SLICECallback struct {
}

func (c SLICECallback) Handle(ind xapp.Swig_slice_ind_msg_t) {
	SliceIndToDictJSON(ind)
}

// ------------------------------------------------------------------------ //
//
//	Definition of Global Structure for storing the latest indication message
//
//	- SliceStatsDict is a struct for storing the latest indication message
//	- RANStats is a struct for storing the RAN field
//	- DlStats is a struct for storing the RAN DL field
//	- UlStats is a struct for storing the RAN UL field
//	- SliceDictionary is a struct for storing the slice parameters
//	- SliceAlgorithmParams is a struct for storing the slice algorithm parameters
//	- UeStats is a struct for storing the UE field
//	- UeDictionary is a struct for storing the UE parameters
//
// ------------------------------------------------------------------------ //
type SliceStatsDict struct {
	RAN RANStats `json:"RAN,omitempty"`
	UE  UeStats  `json:"UE,omitempty"`
}

type RANStats struct {
	DL DlStats `json:"dl,omitempty"`
	UL UlStats `json:"ul,omitempty"`
}

type DlStats struct {
	NumOfSlices    int32             `json:"num_of_slices,omitempty"`
	SliceSchedAlgo string            `json:"slice_sched_algo,omitempty"`
	UeSchedAlgo    string            `json:"ue_sched_algo,omitempty"`
	Slices         []SliceDictionary `json:"slices,omitempty"`
}

type UlStats struct {
	// UL statistics fields
}

type SliceDictionary struct {
	Index           int32                `json:"index,omitempty"`
	Label           string               `json:"label,omitempty"`
	UeSchedAlgo     string               `json:"ue_sched_algo,omitempty"`
	SliceAlgoParams SliceAlgorithmParams `json:"slice_algo_params,omitempty"`
}

type SliceAlgorithmParams struct {
	Type           string  `json:"type,omitempty"`
	PosLow         int32   `json:"pos_low,omitempty"`
	PosHigh        int32   `json:"pos_high,omitempty"`
	MbpsRsvd       float32 `json:"mbps_rsvd,omitempty"`
	MbpsRef        float32 `json:"mbps_ref,omitempty"`
	PctRsvd        float32 `json:"pct_rsvd,omitempty"`
	Deadline       int32   `json:"deadline,omitempty"`
	GuaranteedPrbs int32   `json:"guaranteed_prbs,omitempty"`
	MaxReplenish   int32   `json:"max_replenish,omitempty"`
}

type UeStats struct {
	NumOfUes int32          `json:"num_of_ues,omitempty"`
	Ues      []UeDictionary `json:"ues,omitempty"`
}

type UeDictionary struct {
	Rnti           uint16 `json:"rnti,omitempty"`
	AssocDlSliceId string `json:"assoc_dl_slice_id,omitempty"`
}

// Definition of variable of the global structure for storing the latest indication message
var SliceStats SliceStatsDict

// ------------------------------------------------------------------------ //
//
//	SliceIndToDictJSON function for storing the latest indication message to the global structure
//	This function is called by the callback function in order to store the latest indication message
//
// ------------------------------------------------------------------------ //
func SliceIndToDictJSON(ind xapp.Swig_slice_ind_msg_t) {
	Mutex.Lock()

	// Access the RAN DL field
	dlDict := &SliceStats.RAN.DL

	if ind.GetSlice_stats().GetDl().GetLen_slices() <= 0 {
		// Set the NumOfSlices, SliceSchedAlgo, and Slices fields
		dlDict.NumOfSlices = int32(ind.GetSlice_stats().GetDl().GetLen_slices())
		dlDict.SliceSchedAlgo = "null"
		dlDict.UeSchedAlgo = ind.GetSlice_stats().GetDl().GetSched_name().Get(0)
	} else {
		// Set the NumOfSlices, SliceSchedAlgo, and Slices fields
		dlDict.NumOfSlices = int32(ind.GetSlice_stats().GetDl().GetLen_slices())
		dlDict.SliceSchedAlgo = "null"
		dlDict.Slices = []SliceDictionary{}

		var sliceAlgo string = ""

		// Iterate through all slices
		for i := 0; i < int(ind.GetSlice_stats().GetDl().GetSlices().Size()); i++ {
			// obtain current slice
			currSlice := ind.GetSlice_stats().GetDl().GetSlices().Get(i)
			// obtain current slice's parameters type
			sliceType := currSlice.GetParams().GetXtype()

			// obtain current slice's scheduling algorithm
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
			dlDict.SliceSchedAlgo = sliceAlgo

			// obtain current slice's parameters
			sliceDict := SliceDictionary{
				Index:       int32(currSlice.GetId()),
				Label:       currSlice.GetLabel().Get(0),
				UeSchedAlgo: currSlice.GetSched().Get(0),
			}

			if dlDict.SliceSchedAlgo == "STATIC" {
				// if the slice scheduling algorithm is STATIC, use PosLow and PosHigh
				sliceDict.SliceAlgoParams = SliceAlgorithmParams{
					PosLow:  int32(currSlice.GetParams().GetU().GetSta().GetPos_low()),
					PosHigh: int32(currSlice.GetParams().GetU().GetSta().GetPos_high()),
				}
			} else if dlDict.SliceSchedAlgo == "NVS" {
				if currSlice.GetParams().GetU().GetNvs().GetConf() == 0 {
					// if the slice scheduling algorithm is NVS and type RATE, use MbpsRsvd and MbpsRef
					sliceDict.SliceAlgoParams = SliceAlgorithmParams{
						Type:     "RATE",
						MbpsRsvd: currSlice.GetParams().GetU().GetNvs().GetU().GetRate().GetU1().GetMbps_required(),
						MbpsRef:  currSlice.GetParams().GetU().GetNvs().GetU().GetRate().GetU2().GetMbps_reference(),
					}
				} else if currSlice.GetParams().GetU().GetNvs().GetConf() == 1 {
					// if the slice scheduling algorithm is NVS and type CAPACITY, use PctRsvd
					sliceDict.SliceAlgoParams = SliceAlgorithmParams{
						Type:    "CAPACITY",
						PctRsvd: currSlice.GetParams().GetU().GetNvs().GetU().GetCapacity().GetU().GetPct_reserved(),
					}
				} else {
					sliceDict.SliceAlgoParams = SliceAlgorithmParams{
						Type: "unknown",
					}
				}
			} else if dlDict.SliceSchedAlgo == "EDF" {
				// if the slice scheduling algorithm is EDF, use Deadline, GuaranteedPrbs, and MaxReplenish
				sliceDict.SliceAlgoParams = SliceAlgorithmParams{
					Deadline:       int32(currSlice.GetParams().GetU().GetEdf().GetDeadline()),
					GuaranteedPrbs: int32(currSlice.GetParams().GetU().GetEdf().GetGuaranteed_prbs()),
					MaxReplenish:   int32(currSlice.GetParams().GetU().GetEdf().GetMax_replenish()),
				}
			} else {
				fmt.Println("unknown slice algorithm, cannot handle params")
			}
			dlDict.Slices = append(dlDict.Slices, sliceDict)
		}

	}

	// Access the UE field
	ueDict := &SliceStats.UE

	if ind.GetUe_slice_stats().GetLen_ue_slice() <= 0 {
		// if there is no UE in the cell, set the NumOfUes fields
		ueDict.NumOfUes = int32(ind.GetUe_slice_stats().GetLen_ue_slice())
	} else {
		// Set the NumOfUes and Ues fields
		ueDict.NumOfUes = int32(ind.GetUe_slice_stats().GetLen_ue_slice())
		ueDict.Ues = []UeDictionary{}

		// Iterate through all UEs
		for i := 0; i < int(ind.GetUe_slice_stats().GetUes().Size()); i++ {
			// obtain current ue
			currUe := ind.GetUe_slice_stats().GetUes().Get(i)

			var dlId string

			if currUe.GetDl_id() >= 0 && int(dlDict.NumOfSlices) > 0 {
				// if the UE is associated with a slice, obtain the sliceId
				dlId = string(currUe.GetDl_id())
			} else {
				// if the UE is not associated with a slice, set the sliceId to "Null"
				dlId = "Null"
			}

			// Set the RNTI and AssocDlSliceId fields
			uesDict := UeDictionary{
				Rnti:           currUe.GetRnti(),
				AssocDlSliceId: dlId,
			}
			ueDict.Ues = append(ueDict.Ues, uesDict)
		}
	}

	// Write the global structure to a JSON file
	indDict := map[string]interface{}{
		"RAN": map[string]interface{}{
			"dl": dlDict,
		},
		"UE": ueDict,
	}

	file, _ := os.Create("rt_slice_stats.json")
	defer file.Close()

	indJson, _ := json.Marshal(indDict)
	file.Write(indJson)
	Mutex.Unlock()
}

// ------------------------------------------------------------------------ //
//
//	Definition of the Slicing Request
//
//	- SliceAlgoParams is a struct for storing the slice algorithm parameters
//	- Slice is a struct for storing the slice parameters
//	- Ue is a struct for storing the UE parameters
//	- Request is a struct for storing the request
//
//	The Requests are used to request a control message to the RIC
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
//	FillSliceCtrlMsg function for filling the control message
//
// ------------------------------------------------------------------------ //
func FillSliceCtrlMsg(ctrlType string, ctrlMsg Request) xapp.Slice_ctrl_msg_t {
	msg := xapp.NewSlice_ctrl_msg_t()

	if ctrlType == "ADDMOD" {
		// If the control type is ADDMOD (Add or Modify), set the control type to ADD

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
		// If the control type is DEL (Delete), set the control type to DEL
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
		// If the control type is ASSOC_UE_SLICE (Associate UE with a slice), set the control type to ASSOC_UE_SLICE
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

// ------------------------------------------------------------------------ //
//
//	CreateSlice function for creating a slice
//
// ------------------------------------------------------------------------ //
func CreateSlice(sliceParams Slice, sliceSchedAlgo string) xapp.Fr_slice_t {
	newSlice := xapp.NewFr_slice_t()

	// Set the slice parameters: Id, Label, Len_label, Sched, and Len_sched
	newSlice.SetId(uint(sliceParams.Id))
	newSlice.SetLabel(sliceParams.Label)
	newSlice.SetLen_label(uint(len(sliceParams.Label)))
	newSlice.SetSched(sliceParams.UeSchedAlgo)
	newSlice.SetLen_sched(uint(len(sliceParams.UeSchedAlgo)))

	if sliceSchedAlgo == "STATIC" {
		// If the slice scheduling algorithm is STATIC, set the slice parameters: PosLow, PosHigh
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
			// If the slice scheduling algorithm is NVS and type RATE, set the slice parameters: MbpsRsvd, MbpsRef
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
			// If the slice scheduling algorithm is NVS and type CAPACITY, set the slice parameters: PctRsvd
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
		// If the slice scheduling algorithm is EDF, set the slice parameters: Deadline, GuaranteedPrbs, MaxReplenish
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
//	MAIN
//
// ------------------------------------------------------------------------ //
func main() {

	xapp.Init(xapp.SlToStrVec(os.Args))

	// Connect
	var conn xapp.E2NodeVector = xapp.Conn_e2_nodes()

	var nodes_len int64 = conn.Size()

	if nodes_len <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	nodeIdx := 0

	// ----------------------- SLICE INDICATION----------------------- //
	inner := SLICECallback{}
	callback := xapp.NewDirectorSlice_cb(inner)
	hndlr := xapp.Report_slice_sm(conn.Get(nodeIdx).GetId(), xapp.Interval_ms_5, callback)
	time.Sleep(5 * time.Second)

	// ----------------------- Slice Control Add ----------------------- //
	s1_params_nvs := SliceAlgoParams{PctRsvd: 0.5}
	s1_nvs := Slice{
		Id:              0,
		Label:           "s1",
		UeSchedAlgo:     "PF",
		Type:            "SLICE_SM_NVS_V0_CAPACITY",
		SliceAlgoParams: s1_params_nvs}

	// Idle slice
	s2_params_nvs := SliceAlgoParams{PctRsvd: 0.05}
	s2_nvs := Slice{
		Id:              2,
		Label:           "idle",
		UeSchedAlgo:     "PF",
		Type:            "SLICE_SM_NVS_V0_CAPACITY",
		SliceAlgoParams: s2_params_nvs}

	// AddStaticSlices
	addNvsSlicesCap := Request{
		NumSlices:      2,
		SliceSchedAlgo: "NVS",
		Slices:         []Slice{s1_nvs, s2_nvs},
	}

	msg6 := FillSliceCtrlMsg("ADDMOD", addNvsSlicesCap)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg6)
	time.Sleep(1 * time.Second)

	// ----------------------- Slice Control Add ----------------------- //
	s1_params := SliceAlgoParams{PosLow: 0, PosHigh: 2}
	s1 := Slice{
		Id:              0,
		Label:           "s1",
		UeSchedAlgo:     "PF",
		SliceAlgoParams: s1_params}

	s2_params := SliceAlgoParams{PosLow: 3, PosHigh: 10}
	s2 := Slice{
		Id:              2,
		Label:           "s2",
		UeSchedAlgo:     "PF",
		SliceAlgoParams: s2_params}

	s3_params := SliceAlgoParams{PosLow: 11, PosHigh: 13}
	s3 := Slice{
		Id:              5,
		Label:           "s3",
		UeSchedAlgo:     "PF",
		SliceAlgoParams: s3_params}

	// AddStaticSlices
	AddStaticSlices := Request{
		NumSlices:      3,
		SliceSchedAlgo: "STATIC",
		Slices:         []Slice{s1, s2, s3},
	}

	msg := FillSliceCtrlMsg("ADDMOD", AddStaticSlices)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg)
	time.Sleep(5 * time.Second)

	// ----------------------- SLICE CTRL ASSOC ----------------------- //
	// AddStaticSlices
	ues := Ue{
		Rnti:           0,
		AssocDlSliceId: 2,
	}
	assocUeSlice := Request{
		NumUes: 1,
		Ues:    []Ue{ues},
	}
	msg2 := FillSliceCtrlMsg("ASSOC_UE_SLICE", assocUeSlice)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg2)
	time.Sleep(5 * time.Second)

	// ----------------------- SLICE CTRL DEL ----------------------- //
	del := Request{
		NumDlSlices:     1,
		DeleteDlSliceId: []int{5},
	}
	msg3 := FillSliceCtrlMsg("DEL", del)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg3)
	time.Sleep(2 * time.Second)

	// ----------------------- SLICE CTRL RESET ----------------------- //
	reset := Request{
		NumSlices: 0,
	}
	msg4 := FillSliceCtrlMsg("ADDMOD", reset)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg4)
	time.Sleep(2 * time.Second)

	// ----------------------- END ----------------------- //
	xapp.Rm_report_slice_sm(hndlr)
	// --------------------------------------------------------- //
	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")

}
