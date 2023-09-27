package main

import "C"
import (
	api "build/examples/xApp/go/a1/utils/api"
	slice "build/examples/xApp/go/a1/utils/slice"
	xapp "build/examples/xApp/go/xapp_sdk"

	"fmt"
	"time"

	"math"
	"math/rand"
)

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
//	POLICY ENFORCEMENT CALLBACK
//
//	The function is called internally from the APIs when a new policy
//	is received in order to enforce the new policy.
//
//	Define the main logic of how the policy should be enforced inside this function
//	The new Policy is coming in its input variable `PolicyConfiguration`.
//	Parse it and write the code to enforce the policy using the Slice Utils functionalities
//	to read the current state of the RAN (indication messages) and apply control acions.
//
// ------------------------------------------------------------------------ //
func PolicyEnforcementCallback(PolicyConfiguration api.Configuration) {
	// STEP 1: Check if there is an idle slice and if no create it
	idleSliceId := slice.FindIdleSlice()

	if idleSliceId != -1 {
		fmt.Printf("Found slice with index %d and PctRsvd = 0.05\n", idleSliceId)
	} else {
		fmt.Println("No slice found with PctRsvd = 0.05")

		// Create Idle slice
		algoParams := slice.SliceAlgoParams{PctRsvd: 0.05}
		idleSlice := slice.Slice{
			Id:              2, // TODO: Do this dynamically to find a free id
			Label:           "idle",
			UeSchedAlgo:     "PF",
			Type:            "SLICE_SM_NVS_V0_CAPACITY",
			SliceAlgoParams: algoParams}

		// Request to add the slices
		idleNvsSlicesCap := slice.Request{
			NumSlices:      1,
			SliceSchedAlgo: "NVS",
			Slices:         []slice.Slice{idleSlice},
		}

		// Send the ADDMOD control message to the RIC
		msg := slice.FillSliceCtrlMsg("ADDMOD", idleNvsSlicesCap)
		xapp.Control_slice_sm(Conn.Get(NodeIdx).GetId(), msg)
		
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
	cellId := PolicyConfiguration.Scope.CellID
	sliceId := PolicyConfiguration.Scope.SliceID
	fmt.Println("[Policy]: Cell Id:", cellId, ", Slice Id:", sliceId)

	// Get the maximum and current number of UEs allowed in the slice
	maxNumOfUes := PolicyConfiguration.Statement.MaxNumberOfUEs

	// At this point and only, read the global structure
	// Consider reading all the desired values at once in a single iteration.
	// Otherwise, inconsistencies may occur if the global structure is updated between multiple readings

	reading := slice.ReadSliceStats("multiple_rntis_num_of_ues", -1).(interface{})

	curNumOfUes := reading.(map[string]interface{})["num_of_normal_ues"].(int)
	fmt.Println("[Policy]: Curr Num of UEs:", curNumOfUes, ", Max Num of UEs:", maxNumOfUes)

	// Check if the number of UEs in the slice is greater than the maximum number of UEs allowed in the slice
	if curNumOfUes > maxNumOfUes {
		// TODO: do this with a method in apis only if it is changes its value not writing every time
		api.PolicyEnforced = false

		fmt.Println("[Policy]: Decision: DELETE extra UEs")
		// Delete randomly the extra UEs by associating them to the idle slice
		numOfExtraUes := curNumOfUes - maxNumOfUes
		fmt.Println("[Policy]: Num of extra UEs:", numOfExtraUes)

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
		fmt.Println("[Policy]: RNTIs of the extra UEs:", extraUesRntis)

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
		xapp.Control_slice_sm(Conn.Get(NodeIdx).GetId(), msg)
		time.Sleep(1000 * time.Millisecond)

		// Policy is not enforced yet

	} else if curNumOfUes < maxNumOfUes {
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
		numOfUesToBeAssociated := int(math.Min(float64(maxNumOfUes-curNumOfUes), float64(numOfIdleSliceUes)))
		fmt.Println("[Policy]: Number of UEs that can be associated to the slice:", numOfUesToBeAssociated)

		// Check if there are UEs in the idle slice that can be associated to the slice
		if numOfUesToBeAssociated > 0 {
			fmt.Println("[Policy]: Decision: ADD idle UEs to the extra space")

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
			xapp.Control_slice_sm(Conn.Get(NodeIdx).GetId(), msg)
			time.Sleep(1 * time.Second)

		} else {
			// There are no UEs in the idle slice that can be associated to the slice
			// Policy is enforced
			fmt.Println("[Policy]: No idle UEs to be associated to the slice")
		}
	} else {
		// The number of UEs in the network is equal to the maximum number of UEs allowed in the slice
		// Policy is enforced
		fmt.Println("[Policy]: Policy is enforced. Maximum number of UEs in the slice is reached")
		api.PolicyEnforced = true
	}
}

// Global Variables
var Conn xapp.E2NodeVector
var NodeIdx int
var Hndlr int

// ------------------------------------------------------------------------ //
//  MAIN
// ------------------------------------------------------------------------ //
func main() {
	// ----------------------- Initialization ----------------------- //
	args := []string{"", "-c", "maxnues.conf"}
	xapp.Init(xapp.SlToStrVec(args))

	// Connect
	Conn = xapp.Conn_e2_nodes()

	var nodes_len int64 = Conn.Size()

	if nodes_len <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	NodeIdx = 0

	// ----------------------- SLICE Indication ----------------------- //
	inner := SLICECallback{}
	callback := xapp.NewDirectorSlice_cb(inner)
	Hndlr = xapp.Report_slice_sm(Conn.Get(NodeIdx).GetId(), xapp.Interval_ms_5, callback)
	
	time.Sleep(1 * time.Second)

	// ----------------------- Gin Client ----------------------- //
	api.OpenA1Apis(PolicyEnforcementCallback, "maxnues.conf")

	// ----------------------- END ----------------------- //
	xapp.Rm_report_slice_sm(Hndlr)
	// --------------------------------------------------------- //

	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}
