package callbacks

import (
	api "build/examples/xApp/go/FlexPolicy/utils/api"
	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
	mac "build/examples/xApp/go/FlexPolicy/utils/sm/mac"
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
	xapp "build/examples/xApp/go/xapp_sdk"

	"fmt"
	"math/rand"
	"time"
)

func init() {
	RegisterCallback("MAX_PRB_UTIL", CallbackMaxPrbUtil)
}

// CallbackMaxPrbUtil is a policy for maintaining a maximum PRB utilisation
func CallbackMaxPrbUtil(PolicyConfiguration policy.Configuration) {

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
			s1_params_nvs := slice.SliceAlgoParams{PctRsvd: 0.25}
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
		cellId := int(PolicyConfiguration.Scope.CellID)
		sliceId := int(PolicyConfiguration.Scope.SliceID)
		fmt.Println("[Policy]: Cell Id:", cellId, ", Slice Id:", sliceId)

		// Get the maximum and current number of UEs allowed in the slice
		maxPrbUtilization := int(PolicyConfiguration.Statement.MacPrbUtilisation)

		// At this point and only, read the global structure
		// Consider reading all the desired values at once in a single iteration.
		// Otherwise, inconsistencies may occur if the global structure is updated between multiple readings

		CurrPrbUtilization := mac.TotalPrbUtilization(e2nodeId)

		reading := slice.ReadSliceStats("multiple_rntis_num_of_ues", idleSliceId, e2nodeId).(interface{})
		//fmt.Println("[Policy]: Curr PRB util:", CurrPrbUtilization, ", Max PRB util:", maxPrbUtilization)
		fmt.Println("[Policy]: PRB:", CurrPrbUtilization, " / ", maxPrbUtilization, " %")

		// Check if the number of UEs in the slice is greater than the maximum number of UEs allowed in the slice
		if CurrPrbUtilization > maxPrbUtilization {
			// TODO: do this with a method in apis only if it is changes its value not writing every time
			api.PolicyEnforced = false

			fmt.Println("[Policy]: Decision: DELETE 1 extra UE to reduce PRB utilisation")

			// Delete randomly one extra UE by associating it to the idle slice
			curNumOfUes := reading.(map[string]interface{})["num_of_normal_ues"].(int)

			var numOfExtraUes int
			if curNumOfUes > 0 {
				numOfExtraUes = 1
			} else {
				numOfExtraUes = 0
			}

			// Get the RNTIs of the UEs in the slice
			sliceRntis := reading.(map[string]interface{})["normal_rntis"].([]uint16)
			fmt.Println("[Policy]: RNTIs of the UEs in the slice:", sliceRntis)

			// Randomly select the RNTIs of the extra UEs
			extraUesRntis := []uint16{}
			for i := 0; i < numOfExtraUes; i++ {
				// Random Ue Idx
				randRntiIdx := rand.Intn(len(sliceRntis))

				// Append the RNTI to the list of the extra UEs
				extraUesRntis = append(extraUesRntis, sliceRntis[randRntiIdx])

				// Remove the RNTI from the list of the UEs in the slice
				sliceRntis = append(sliceRntis[:randRntiIdx], sliceRntis[randRntiIdx+1:]...)
			}
			fmt.Println("[Policy]: RNTI of the extra UE:", extraUesRntis)

			// Associate the extra UEs to the idle slice
			// Create a structure with the extra UEs
			uesToBeAssoc := []slice.Ue{}
			for i := 0; i < numOfExtraUes; i++ {
				ue := slice.Ue{
					Rnti:           extraUesRntis[i],
					AssocDlSliceId: idleSliceId,
				}

				uesToBeAssoc = append(uesToBeAssoc, ue)
			}

			// Associate the UEs to the slice
			assocUeSlice := slice.Request{
				NumUes: numOfExtraUes,
				Ues:    uesToBeAssoc,
			}
			msg := slice.FillSliceCtrlMsg("ASSOC_UE_SLICE", assocUeSlice)
			xapp.Control_slice_sm(sm.E2Nodes.Get(0).GetId(), msg)
			time.Sleep(1000 * time.Millisecond)

			// Policy is not enforced yet

		} else if CurrPrbUtilization < maxPrbUtilization {
			// Find if there are UEs in the idle slice that can be associated to the slice
			fmt.Println("[Policy]: Policy is enforced. Less UEs than max num allowed")

			// Policy is enforced
			api.PolicyEnforced = true

			// Get the RNTIs of the UEs in the idle slice
			idleSliceRntis := reading.(map[string]interface{})["idle_rntis"].([]uint16)
			fmt.Println("[Policy]: RNTIs of the UEs in the idle slice:", idleSliceRntis)

			// Get the number of UEs in the idle slice
			numOfIdleSliceUes := reading.(map[string]interface{})["num_of_idle_ues"].(int)
			fmt.Println("[Policy]: Number of UEs in the idle slice:", numOfIdleSliceUes)

			// Get the number of UEs that can be associated to the slice
			numOfUesToBeAssociated := int(float64(numOfIdleSliceUes))
			fmt.Println("[Policy]: Number of UEs that can be associated to the slice:", numOfUesToBeAssociated)

			// Check if there are UEs in the idle slice that can be associated to the slice
			if numOfUesToBeAssociated > 0 {
				fmt.Println("[Policy]: Decision: ADD one idle UE to the extra space")
				numOfUesToBeAssociated = 1

				// Randomly select the RNTIs of the UEs that can be associated to the slice
				uesToBeAssociatedRntis := []uint16{}

				for i := 0; i < numOfUesToBeAssociated; i++ {
					// Random Ue Idx
					randRntiIdx := rand.Intn(len(idleSliceRntis))

					// Append the RNTI to the list of the UEs that can be associated to the slice
					uesToBeAssociatedRntis = append(uesToBeAssociatedRntis, idleSliceRntis[randRntiIdx])

					// Remove the RNTI from the list of the UEs in the idle slice
					idleSliceRntis = append(idleSliceRntis[:randRntiIdx], idleSliceRntis[randRntiIdx+1:]...)
				}
				fmt.Println("[Policy]: RNTIs of the UEs that can be associated to the slice:", uesToBeAssociatedRntis)

				// Associate the UEs to the slice
				// Create a structure with the UEs to be associated
				uesToBeAssoc := []slice.Ue{}
				for i := 0; i < numOfUesToBeAssociated; i++ {
					ue := slice.Ue{
						Rnti:           uesToBeAssociatedRntis[i],
						AssocDlSliceId: sliceId,
					}

					uesToBeAssoc = append(uesToBeAssoc, ue)
				}

				assocUeSlice := slice.Request{
					NumUes: numOfUesToBeAssociated,
					Ues:    uesToBeAssoc,
				}
				msg := slice.FillSliceCtrlMsg("ASSOC_UE_SLICE", assocUeSlice)
				xapp.Control_slice_sm(sm.E2Nodes.Get(0).GetId(), msg)
				time.Sleep(1 * time.Second)

			} else {
				// There are no UEs in the idle slice that can be associated to the slice
				// Policy is enforced
				fmt.Println("[Policy]: No idle UEs to be associated to the slice")
			}
		} else {
			// CurrPrbUtilization == maxPrbUtilization
			// Policy is enforced
			fmt.Println("[Policy]: Policy is enforced. Maximum PRB util is reached")
			api.PolicyEnforced = true
		}
	}
}
