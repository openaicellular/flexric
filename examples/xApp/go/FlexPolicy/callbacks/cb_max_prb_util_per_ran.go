package callbacks

import (
	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
	xapp "build/examples/xApp/go/xapp_sdk"
	//api "build/examples/xApp/go/FlexPolicy/utils/api"
	mac "build/examples/xApp/go/FlexPolicy/utils/sm/mac"
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"

	"fmt"
	"time"
)

func init() {
	RegisterCallback("MAX_PRB_UTIL_PER_RAN", CallbackMaxPrbUtilPerRan)
}

// CallbackMaxPrbUtil is a policy for maintaining a maximum PRB utilisation
func CallbackMaxPrbUtilPerRan(PolicyConfiguration policy.Configuration) {

	// Iterate over all the RANs
	for i := 0; i <= int(sm.E2Nodes.Size()-1); i++ {

		// STEP 0: Scope on the desired RAN

		// - a) PLMN
		currMcc := sm.E2Nodes.Get(i).GetId().GetPlmn().GetMcc()
		currMnc := sm.E2Nodes.Get(i).GetId().GetPlmn().GetMnc()
		currPlmn := fmt.Sprintf("%d%d", currMcc, currMnc)

		// - b) RanType
		currRanType := sm.E2Nodes.Get(i).GetId().GetXtype()

		// - c) NbId
		currNbId := sm.E2Nodes.Get(i).GetId().GetNb_id().GetNb_id()

		// - d) CuDuId    TODO: maybe swig needs to be extended to support this
		// currCuDuId := sm.E2Nodes.Get(i).GetId().GetCu_du_id().GetCu_du_id()

		// Compare with policy
		if currPlmn != PolicyConfiguration.Scope.Plmn ||
			currRanType != PolicyConfiguration.Scope.RanType ||
			currNbId != PolicyConfiguration.Scope.NbId {

		}

		PolicyConfiguration.Scope.Plmn

		// STEP 1: Check if there is an idle slice and if no create it
		idleSliceId := slice.FindIdleSlice()

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
			// TODO: fix hard coded 0
			msg := slice.FillSliceCtrlMsg("ADDMOD", idleNvsSlicesCap)
			xapp.Control_slice_sm(sm.E2Nodes.Get(0).GetId(), msg)

			time.Sleep(1 * time.Second)

			// ensure the slice is created
			idleSliceId = slice.FindIdleSlice()
			if idleSliceId != -1 {
				fmt.Printf("Found slice with index %d and PctRsvd = 0.05\n", idleSliceId)
			} else {
				fmt.Println("Error creating idle slice")
				return
			}
		}

		// STEP 2: Enforce the policy
		fmt.Println("\n[Policy]:------------------ Enforcement -------------------------")
		cellId := int(PolicyConfiguration.Scope.CellID)
		sliceId := int(PolicyConfiguration.Scope.SliceID)
		fmt.Println("[Policy]: Cell Id:", cellId, ", Slice Id:", sliceId)

		// Get the maximum and current number of UEs allowed in the slice
		maxPrbUtilization := int(PolicyConfiguration.Statement.MacPrbUtilisation)

		// At this point and only, read the global structure
		// Consider reading all the desired values at once in a single iteration.
		// Otherwise, inconsistencies may occur if the global structure is updated between multiple readings

		CurrPrbUtilization := mac.TotalPrbUtilization()

		reading := slice.ReadSliceStats("multiple_rntis_num_of_ues", idleSliceId).(interface{})
		//fmt.Println("[Policy]: Curr PRB util:", CurrPrbUtilization, ", Max PRB util:", maxPrbUtilization)
		fmt.Println("[Policy]: PRB:", CurrPrbUtilization, " / ", maxPrbUtilization, " %")

		// Enforce the policy
		// Adjust Slices
		if maxPrbUtilization == 0 {
			maxPrbUtilization = 5
		} else if maxPrbUtilization == 100 {
			maxPrbUtilization = 95
		}

		normalSliceCapacity := float64(maxPrbUtilization / 100.0)
		s1_params_nvs := slice.SliceAlgoParams{PctRsvd: normalSliceCapacity}
		s1_nvs := slice.Slice{
			Id:              0,
			Label:           "s1",
			UeSchedAlgo:     "PF",
			Type:            "SLICE_SM_NVS_V0_CAPACITY",
			SliceAlgoParams: s1_params_nvs}

		// Create Idle slice
		IdleSliceCapacity := 1.0 - normalSliceCapacity
		algoParams := slice.SliceAlgoParams{PctRsvd: IdleSliceCapacity}
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
		// TODO: fix hard coded 0
		msg := slice.FillSliceCtrlMsg("ADDMOD", idleNvsSlicesCap)
		xapp.Control_slice_sm(sm.E2Nodes.Get(0).GetId(), msg)

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

		for i := 0; i <= int(sm.E2Nodes.Size()-1); i++ {
			// TODO: fix send only to DUs the type is du
			msg := slice.FillSliceCtrlMsg("ASSOC_UE_SLICE", assocUeSlice)
			xapp.Control_slice_sm(sm.E2Nodes.Get(i).GetId(), msg)
			time.Sleep(100 * time.Millisecond)
		}
	}

}
