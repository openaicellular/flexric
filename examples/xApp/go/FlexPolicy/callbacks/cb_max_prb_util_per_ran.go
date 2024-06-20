package callbacks

import (
	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
	xapp "build/examples/xApp/go/xapp_sdk"
	"math"

	//api "build/examples/xApp/go/FlexPolicy/utils/api"
	mac "build/examples/xApp/go/FlexPolicy/utils/sm/mac"
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"

	"fmt"
	"time"
)

func init() {
	RegisterCallback("MAX_PRB_UTIL_PER_RAN", CallbackMaxPrbUtilPerRan)
}

// CallbackMaxPrbUtilPerRan is a policy for maintaining a maximum PRB utilisation
func CallbackMaxPrbUtilPerRan(PolicyConfiguration policy.Configuration) {

	// Iterate over all the RANs
	for i := 0; i <= int(sm.E2Nodes.Size()-1); i++ {

		// STEP 0: Scope on the desired RAN
		// - a) PLMN
		currMcc := sm.E2Nodes.Get(i).GetId().GetPlmn().GetMcc()
		currMnc := sm.E2Nodes.Get(i).GetId().GetPlmn().GetMnc()

		// - b) RanType
		currRanType := xapp.Get_e2ap_ngran_name(sm.E2Nodes.Get(i).GetId().GetXtype())

		// - c) NbId
		currNbId := uint32(sm.E2Nodes.Get(i).GetId().GetNb_id().GetNb_id())

		// - d) CuDuId    TODO: maybe swig needs to be extended to support this
		// currCuDuId := sm.E2Nodes.Get(i).GetId().GetCu_du_id().GetCu_du_id()

		// Skip CUs
		if currRanType == "ngran_eNB_CU" || currRanType == "ngran_ng_eNB_CU" || currRanType == "ngran_gNB_CU" {
			continue
		}

		// Compare with policy
		if currMcc != PolicyConfiguration.Scope.Mcc || currMnc != PolicyConfiguration.Scope.Mnc ||
			currRanType != PolicyConfiguration.Scope.RanType || currNbId != PolicyConfiguration.Scope.NbId {
			continue
		}

		// STEP 1: Check if there is an idle slice and if no create it
		e2nodeId := slice.E2NodeId{
			Mcc:     currMcc,
			Mnc:     currMnc,
			NbId:    currNbId,
			CuDuId:  0,
			RanType: currRanType,
		}
		idleSliceId := slice.FindIdleSlice(e2nodeId)

		if idleSliceId != -1 {
			fmt.Printf("Found slice with index %d and PctRsvd = 0.05\n", idleSliceId)
		} else {
			fmt.Println("No slice found with PctRsvd = 0.05")

			// Adjust the zero slice first
			s1_params_nvs := slice.SliceAlgoParams{PctRsvd: 0.95}
			s1_nvs := slice.Slice{
				Id:              0,
				Label:           "s1",
				UeSchedAlgo:     "PF",
				Type:            "SLICE_SM_NVS_V0_CAPACITY",
				SliceAlgoParams: s1_params_nvs}

			// Create Idle slice
			algoParams := slice.SliceAlgoParams{PctRsvd: 0.05}
			idleSlice := slice.Slice{
				Id:              1, // TODO: Do this dynamically to find a free id
				Label:           "idle",
				UeSchedAlgo:     "PF",
				Type:            "SLICE_SM_NVS_V0_CAPACITY",
				SliceAlgoParams: algoParams}

			// Request to add the slices
			idleNvsSlicesCap := slice.Request{
				NumSlices:      2,
				SliceSchedAlgo: "NVS",
				Slices:         []slice.Slice{s1_nvs, idleSlice},
			}

			// Send the ADDMOD control message to the RIC
			msg := slice.FillSliceCtrlMsg("ADDMOD", idleNvsSlicesCap)
			xapp.Control_slice_sm(sm.E2Nodes.Get(i).GetId(), msg)

			time.Sleep(1 * time.Second)

			// ensure the slice is created
			idleSliceId = slice.FindIdleSlice(e2nodeId)
			if idleSliceId != -1 {
				fmt.Printf("Found slice with index %d and PctRsvd = 0.05\n", idleSliceId)
			} else {
				fmt.Println("Error creating idle slice")
				return
			}
		}

		// STEP 2: Enforce the policy
		fmt.Println("\n[Policy]:------------------ Enforcement -------------------------")
		fmt.Println("[Policy]: RAN Type:", currRanType, ", MCC:", currMcc, ", MNC:", currMnc, ", NbId:", currNbId)

		// Get the maximum and current number of UEs allowed in the slice
		maxPrbUtilization := int(PolicyConfiguration.Statement.MacPrbUtilisation)

		// At this point and only, read the global structure
		// Consider reading all the desired values at once in a single iteration.
		// Otherwise, inconsistencies may occur if the global structure is updated between multiple readings

		e2nodeId = slice.E2NodeId{
			Mcc:     currMcc,
			Mnc:     currMnc,
			NbId:    currNbId,
			CuDuId:  0,
			RanType: currRanType,
		}
		CurrPrbUtilization := mac.TotalPrbUtilization(e2nodeId)

		reading := slice.ReadSliceStats("multiple_rntis_num_of_ues", idleSliceId, e2nodeId).(interface{})
		fmt.Println("[Policy]: Curr PRB util:", CurrPrbUtilization, " %, Max PRB util:", maxPrbUtilization, " %")

		fmt.Println("[Policy]: Reset slices to minimum capacities")
		// Reset slices first giving minimum capacities at both slices
		// This step ensures that the sum of the capacities is not more than 1
		s1_params_nvs := slice.SliceAlgoParams{PctRsvd: 0.05}
		s1_nvs := slice.Slice{
			Id:              0,
			Label:           "s1",
			UeSchedAlgo:     "PF",
			Type:            "SLICE_SM_NVS_V0_CAPACITY",
			SliceAlgoParams: s1_params_nvs}

		// Create Idle slice
		algoParams := slice.SliceAlgoParams{PctRsvd: 0.05}
		idleSlice := slice.Slice{
			Id:              1, // TODO: Do this dynamically to find a free id
			Label:           "idle",
			UeSchedAlgo:     "PF",
			Type:            "SLICE_SM_NVS_V0_CAPACITY",
			SliceAlgoParams: algoParams}

		// Request to add the slices
		idleNvsSlicesCap := slice.Request{
			NumSlices:      2,
			SliceSchedAlgo: "NVS",
			Slices:         []slice.Slice{s1_nvs, idleSlice},
		}

		// Send the ADDMOD control message to the RIC
		msg := slice.FillSliceCtrlMsg("ADDMOD", idleNvsSlicesCap)
		xapp.Control_slice_sm(sm.E2Nodes.Get(i).GetId(), msg)

		time.Sleep(100 * time.Millisecond)

		// Enforce the policy
		// Adjust Slices
		if maxPrbUtilization < 5 {
			maxPrbUtilization = 5
		} else if maxPrbUtilization > 95 {
			maxPrbUtilization = 95
		}

		normalSliceCapacity := float64(maxPrbUtilization) / 100.0
		normalSliceCapacity = math.Round(normalSliceCapacity*100) / 100
		s1_params_nvs = slice.SliceAlgoParams{PctRsvd: normalSliceCapacity}
		s1_nvs = slice.Slice{
			Id:              0,
			Label:           "s1",
			UeSchedAlgo:     "PF",
			Type:            "SLICE_SM_NVS_V0_CAPACITY",
			SliceAlgoParams: s1_params_nvs}

		// Create Idle slice
		IdleSliceCapacity := 1.0 - normalSliceCapacity
		IdleSliceCapacity = math.Round(IdleSliceCapacity*100) / 100
		algoParams = slice.SliceAlgoParams{PctRsvd: IdleSliceCapacity}
		idleSlice = slice.Slice{
			Id:              1, // TODO: Do this dynamically to find a free id
			Label:           "idle",
			UeSchedAlgo:     "PF",
			Type:            "SLICE_SM_NVS_V0_CAPACITY",
			SliceAlgoParams: algoParams}

		// Request to add the slices
		idleNvsSlicesCap = slice.Request{
			NumSlices:      2,
			SliceSchedAlgo: "NVS",
			Slices:         []slice.Slice{s1_nvs, idleSlice},
		}
		fmt.Println("[Policy]: Adjusting slices to:")
		fmt.Println("[Policy]: s1:", normalSliceCapacity, ", idle:", 0.05)

		// Send the ADDMOD control message to the RIC
		msg = slice.FillSliceCtrlMsg("ADDMOD", idleNvsSlicesCap)
		xapp.Control_slice_sm(sm.E2Nodes.Get(i).GetId(), msg)

		time.Sleep(100 * time.Millisecond)

		// Associate UEs to the normal slice
		allRntis := reading.(map[string]interface{})["all_rntis"].([]uint16)

		fmt.Println("[Policy]: RNTIs of the UEs that can be associated to the slice:", allRntis)

		// Associate the UEs to the slice
		// Create a structure with the UEs to be associated
		uesToBeAssoc := []slice.Ue{}
		for i := 0; i < len(allRntis); i++ {
			ue := slice.Ue{
				Rnti:           allRntis[i],
				AssocDlSliceId: 0, // TODO: fix hardcoded, maybe give a name constant to slices or per scope
			}

			uesToBeAssoc = append(uesToBeAssoc, ue)
		}

		assocUeSlice := slice.Request{
			NumUes: len(allRntis),
			Ues:    uesToBeAssoc,
		}

		msg = slice.FillSliceCtrlMsg("ASSOC_UE_SLICE", assocUeSlice)
		xapp.Control_slice_sm(sm.E2Nodes.Get(i).GetId(), msg)
		time.Sleep(100 * time.Millisecond)
	}
}
