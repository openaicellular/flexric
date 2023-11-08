package utils

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"

	"encoding/json"
	"fmt"
	"log"
	"os"
	"sync"
)

// SLICECallback SLICE INDICATION CALLBACK
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

// E2NodeSliceStatsMap is a map of E2Node ID to SliceStatsDict
type E2NodeSliceStatsMap struct {
	Mcc              uint16         `json:"mcc,omitempty"`
	Mnc              uint16         `json:"mnc,omitempty"`
	NbId             uint32         `json:"nb_id,omitempty"`
	CuDuId           uint32         `json:"cu_du_id,omitempty"`
	RanType          string         `json:"ran_type,omitempty"`
	E2NodeSliceStats SliceStatsDict `json:"slice_stats,omitempty"`
}

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

// SliceStats Definition of variable of the global structure for storing the latest indication message
var SliceStats []E2NodeSliceStatsMap

// Mutex for locking the global structure SliceStatsDict
var Mutex sync.Mutex

type E2NodeId struct {
	Mcc     uint16
	Mnc     uint16
	NbId    uint32
	CuDuId  uint32
	RanType string
}

// ------------------------------------------------------------------------ //
//
//	 ReadSliceStats function for reading desired stats items from the global structure
//
//	 Currently supported items:
//	 - [RAN]:                        return the RAN field
//	 - [UE]:                         return the UE field
//	 - [RAN_DL]:				        return the RAN DL field
//	 - [num_of_slices]:			    return the number of slices
//	 - [ues]:				        return the UEs field
//	 - [num_of_ues]:			        return the number of UEs
//	 - [rntis]:				        return the RNTIs field
//	 - [multiple_rntis_num_of_ues]:  return multiple output items:
//									- normal_rntis: all RNTIs associated with the normal slice
//									- num_of_normal_ues: the number of UEs associated with the normal slice
//									- idle_rntis: all RNTIs associated with the idle slice
//									- num_of_idle_ues: the number of UEs associated with the idle slice
//									- num_of_all_ues: the total number of UEs in the cell
//
// ------------------------------------------------------------------------ //
func ReadSliceStats(item string, sliceId int, e2nodeId E2NodeId) interface{} {

	// find entry index in the global structure SliceStats based on the E2Node ID
	entryFound := false
	entryIndex := -1

	for i, entry := range SliceStats {

		if entry.Mcc == e2nodeId.Mcc && entry.Mnc == e2nodeId.Mnc && entry.NbId == e2nodeId.NbId && entry.RanType == e2nodeId.RanType {
			entryFound = true
			entryIndex = i
			break
		}

	}
	if entryFound == false {
		fmt.Println("ERROR: Entry not found in ReadSliceStats()")
		return nil
	}

	switch item {
	case "RAN":
		// Read the RAN field from the global structure
		Mutex.Lock()
		value := SliceStats[entryIndex].E2NodeSliceStats.RAN
		Mutex.Unlock()
		return value
	case "UE":
		// Read the UE field from the global structure
		Mutex.Lock()
		value := SliceStats[entryIndex].E2NodeSliceStats.UE
		Mutex.Unlock()
		return value
	case "RAN_DL":
		// Read the RAN DL field from the global structure
		Mutex.Lock()
		value := SliceStats[entryIndex].E2NodeSliceStats.RAN.DL
		Mutex.Unlock()
		return value
	case "num_of_slices":
		// Read the number of slices from the global structure
		Mutex.Lock()
		value := SliceStats[entryIndex].E2NodeSliceStats.RAN.DL.NumOfSlices
		Mutex.Unlock()
		return value
	case "ues":
		// Read the UEs field from the global structure
		Mutex.Lock()
		value := SliceStats[entryIndex].E2NodeSliceStats.UE.Ues
		Mutex.Unlock()
		return value
	case "num_of_ues":
		// Read the number of UEs from the global structure
		// - If sliceId = -1, return the total number of UEs in the cell
		// - If sliceId >= 0, return the number of UEs associated with the sliceId

		if sliceId == -1 {
			// Read the total number of UEs from the global structure
			Mutex.Lock()
			value := SliceStats[entryIndex].E2NodeSliceStats.UE.NumOfUes
			Mutex.Unlock()
			return value
		} else if sliceId >= 0 {
			// Read the number of UEs associated with the sliceId from the global structure
			Mutex.Lock()
			num_of_ues_per_slice := 0

			numOfUes := SliceStats[entryIndex].E2NodeSliceStats.UE.NumOfUes

			// If there is no UE in the cell, return 0
			if numOfUes == 0 {
				Mutex.Unlock()
				return 0
			}

			// Iterate through all UEs in the cell
			for _, currUe := range SliceStats[entryIndex].E2NodeSliceStats.UE.Ues {
				// get the sliceId of the current UE
				currAssocDlSliceId := currUe.AssocDlSliceId

				// Convert the string to byte
				b := []byte(currAssocDlSliceId)[0]

				// Convert the byte to integer
				currAssocDlSliceIdInt := int(b)

				// If the current UE is associated with the sliceId, increment the number of UEs
				if currAssocDlSliceIdInt == sliceId {
					num_of_ues_per_slice++
				}
			}
			Mutex.Unlock()
			return num_of_ues_per_slice
		} else {
			fmt.Println("ERROR: Invalid sliceId")
			return nil
		}
	case "rntis":
		// Read the RNTIs field from the global structure
		// - If sliceId = -1, return all RNTIs in the cell
		// - If sliceId >= 0, return all RNTIs associated with the sliceId

		if sliceId == -1 {
			// Read all RNTIs from the global structure
			Mutex.Lock()
			rntis := []uint16{}
			for _, currUe := range SliceStats[entryIndex].E2NodeSliceStats.UE.Ues {
				rntis = append(rntis, currUe.Rnti)
			}
			Mutex.Unlock()
			return rntis
		} else if sliceId >= 0 {
			// Read the RNTIs associated with the sliceId from the global structure
			Mutex.Lock()
			rntis := []uint16{}
			for _, currUe := range SliceStats[entryIndex].E2NodeSliceStats.UE.Ues {
				// get the sliceId of the current UE
				currAssocDlSliceId := currUe.AssocDlSliceId

				// Convert the string to byte
				b := []byte(currAssocDlSliceId)[0]

				// Convert the byte to integer
				currAssocDlSliceIdInt := int(b)

				// If the current UE is associated with the sliceId, append the RNTI to the list
				if currAssocDlSliceIdInt == sliceId {
					rntis = append(rntis, currUe.Rnti)
				}
			}
			Mutex.Unlock()
			return rntis
		} else {
			fmt.Println("ERROR: Invalid sliceId")
			return nil
		}
	case "multiple_rntis_num_of_ues":
		// sliceId is the idle found before
		//
		// Take multiple output items from the global structure
		// - normal_rntis: return all RNTIs associated with the normal slice
		// - num_of_normal_ues: return the number of UEs associated with the normal slice
		// - idle_rntis: return all RNTIs associated with the idle slice
		// - num_of_idle_ues: return the number of UEs associated with the idle slice
		// - num_of_all_ues: return the total number of UEs in the cell
		Mutex.Lock()
		// [Output 0]: Read all RNTIs from the global structure
		allRntis := []uint16{}
		for _, currUe := range SliceStats[entryIndex].E2NodeSliceStats.UE.Ues {
			allRntis = append(allRntis, currUe.Rnti)
		}

		// [Output 1 & 2]: Read the RNTIs associated with the normal slice from the global structure
		rntis := []uint16{}
		for _, currUe := range SliceStats[entryIndex].E2NodeSliceStats.UE.Ues {
			// get the sliceId of the current UE
			currAssocDlSliceId := currUe.AssocDlSliceId

			// Convert the string to byte
			b := []byte(currAssocDlSliceId)[0]

			// Convert the byte to integer
			currAssocDlSliceIdInt := int(b)

			// If the current UE is associated with the normal slice, append the RNTI to the list
			if currAssocDlSliceIdInt != sliceId { // different than idle
				rntis = append(rntis, currUe.Rnti)
			}
		}
		// Store the RNTIs associated with the normal slice and calculate the number of UEs associated with the normal slice
		normalRntis := rntis
		numOfNormalUes := len(normalRntis)

		// [Output 3 & 4]: Read the RNTIs associated with the idle slice from the global structure
		rntis = []uint16{}
		for _, currUe := range SliceStats[entryIndex].E2NodeSliceStats.UE.Ues {
			// get the sliceId of the current UE
			currAssocDlSliceId := currUe.AssocDlSliceId

			// Convert the string to byte
			b := []byte(currAssocDlSliceId)[0]

			// Convert the byte to integer
			currAssocDlSliceIdInt := int(b)

			// If the current UE is associated with the idle slice, append the RNTI to the list
			if currAssocDlSliceIdInt == sliceId { // equal to idle
				rntis = append(rntis, currUe.Rnti)
			}
		}
		// Store the RNTIs associated with the idle slice and calculate the number of UEs associated with the idle slice
		idleRntis := rntis
		numOfIdleUes := len(idleRntis)

		// [Output 5]: Read the total number of UEs from the global structure
		numAllUes := SliceStats[entryIndex].E2NodeSliceStats.UE.NumOfUes

		Mutex.Unlock()

		return map[string]interface{}{
			"all_rntis":         allRntis,
			"normal_rntis":      normalRntis,
			"num_of_normal_ues": numOfNormalUes,
			"idle_rntis":        idleRntis,
			"num_of_idle_ues":   numOfIdleUes,
			"num_of_all_ues":    numAllUes,
		}
	default:
		fmt.Println("ERROR: Invalid item")
		return nil
	}
}

func FindIdleSlice(e2nodeId E2NodeId) int {
	Mutex.Lock()

	// find entry index in the global structure SliceStats based on the E2Node ID
	entryFound := false
	entryIndex := -1
	for i, entry := range SliceStats {
		if entry.Mcc == e2nodeId.Mcc && entry.Mnc == e2nodeId.Mnc && entry.NbId == e2nodeId.NbId && entry.RanType == e2nodeId.RanType {
			entryFound = true
			entryIndex = i
			break
		}

	}
	if entryFound == false {
		fmt.Println("ERROR: Entry not found in ReadSliceStats()")
		return -1
	}

	defer Mutex.Unlock()

	if len(SliceStats[entryIndex].E2NodeSliceStats.RAN.DL.Slices) == 0 {
		return -1
	}

	for _, sl := range SliceStats[entryIndex].E2NodeSliceStats.RAN.DL.Slices {
		if sl.Label == "idle" {
			return int(sl.Index)
		}
	}

	return -1
}

// ------------------------------------------------------------------------ //
//
//	SliceIndToDictJSON function for storing the latest indication message to the global structure
//	This function is called by the callback function in order to store the latest indication message
//
// ------------------------------------------------------------------------ //
func SliceIndToDictJSON(ind xapp.Swig_slice_ind_msg_t) {
	Mutex.Lock()

	// get E2node details
	NbId := uint32(ind.GetId().GetNb_id().GetNb_id())
	// CuDuId := ind.GetId().GetCu_du_id() // TODO: not yet supported, maybe needs swig extension
	RanType := xapp.Get_e2ap_ngran_name(ind.GetId().GetXtype())
	Mcc := ind.GetId().GetPlmn().GetMcc()
	Mnc := ind.GetId().GetPlmn().GetMnc()

	// Find the entry in the global structure SliceStats if it exists or create a new entry
	entryFound := false
	entryIndex := -1
	for i, entry := range SliceStats {
		if entry.Mcc == Mcc && entry.Mnc == Mnc && entry.NbId == NbId && entry.RanType == RanType {
			entryFound = true
			entryIndex = i
			break
		}

	}
	if entryFound == false {
		// Create a new entry in the global structure SliceStats
		newEntry := E2NodeSliceStatsMap{
			Mcc:  Mcc,
			Mnc:  Mnc,
			NbId: NbId,
			//CuDuId:           CuDuId,
			RanType:          RanType,
			E2NodeSliceStats: SliceStatsDict{},
		}
		SliceStats = append(SliceStats, newEntry)
		entryIndex = len(SliceStats) - 1

	}

	// Access the RAN DL field
	dlDict := &SliceStats[entryIndex].E2NodeSliceStats.RAN.DL

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
	ueDict := &SliceStats[entryIndex].E2NodeSliceStats.UE

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

// CreateSlice function for creating a slice
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

// Function for returning the appropriate Int for the RAN Name
func RanNametoInt(value string) int {
	switch value {
	case "ngran_eNB":
		return 0
	case "ngran_ng_eNB":
		return 1
	case "ngran_gNB":
		return 2
	case "ngran_eNB_CU":
		return 3
	case "ngran_ng_eNB_CU":
		return 4
	case "ngran_gNB_CU":
		return 5
	case "ngran_eNB_DU":
		return 6
	case "ngran_gNB_DU":
		return 7
	case "ngran_eNB_MBMS_STA":
		return 8
	}
	return -1
}

// Definition of the Slicing Request
// - SliceAlgoParams is a struct for storing the slice algorithm parameters
// - Slice is a struct for storing the slice parameters
// - Ue is a struct for storing the UE parameters
// - Request is a struct for storing the request
// The Requests are used to request a control message to the RIC
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

// FillSliceCtrlMsg function for filling the control message
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

func DeepCopySliceStats(original []E2NodeSliceStatsMap) []E2NodeSliceStatsMap {
	Mutex.Lock()
	defer Mutex.Unlock()

	copied := make([]E2NodeSliceStatsMap, len(original))
	for i, sliceStatsMap := range original {
		copied[i] = E2NodeSliceStatsMap{
			Mcc:              sliceStatsMap.Mcc,
			Mnc:              sliceStatsMap.Mnc,
			NbId:             sliceStatsMap.NbId,
			CuDuId:           sliceStatsMap.CuDuId,
			RanType:          sliceStatsMap.RanType,
			E2NodeSliceStats: DeepCopySliceStatsDict(sliceStatsMap.E2NodeSliceStats),
		}
	}
	return copied
}

// DeepCopySliceStatsDict Deep copy functions for each nested struct
func DeepCopySliceStatsDict(original SliceStatsDict) SliceStatsDict {

	CopiedSliceStats := SliceStatsDict{
		RAN: deepCopyRANStats(original.RAN),
		UE:  deepCopyUeStats(original.UE),
	}

	return CopiedSliceStats
}

func deepCopyRANStats(original RANStats) RANStats {
	return RANStats{
		DL: deepCopyDlStats(original.DL),
		UL: original.UL, // Deep copy UL if necessary
	}
}

func deepCopyDlStats(original DlStats) DlStats {
	copiedSlices := make([]SliceDictionary, len(original.Slices))
	for i, slice := range original.Slices {
		copiedSlices[i] = deepCopySliceDictionary(slice)
	}
	return DlStats{
		NumOfSlices:    original.NumOfSlices,
		SliceSchedAlgo: original.SliceSchedAlgo,
		UeSchedAlgo:    original.UeSchedAlgo,
		Slices:         copiedSlices,
	}
}

func deepCopySliceDictionary(original SliceDictionary) SliceDictionary {
	return SliceDictionary{
		Index:           original.Index,
		Label:           original.Label,
		UeSchedAlgo:     original.UeSchedAlgo,
		SliceAlgoParams: original.SliceAlgoParams, // Assuming SliceAlgorithmParams has no pointers or slices
	}
}

func deepCopyUeStats(original UeStats) UeStats {
	copiedUes := make([]UeDictionary, len(original.Ues))
	copy(copiedUes, original.Ues)
	return UeStats{
		NumOfUes: original.NumOfUes,
		Ues:      copiedUes,
	}
}
