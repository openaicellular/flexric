package main

// import "C"
// import (
// 	api "build/examples/xApp/go/a1/utils/api"
// 	slice "build/examples/xApp/go/a1/utils/slice"
// 	xapp "build/examples/xApp/go/xapp_sdk"

// 	"fmt"
// 	"time"
// )

// // ------------------------------------------------------------------------ //
// //
// //	SLICE INDICATION CALLBACK
// //
// // ------------------------------------------------------------------------ //
// type SLICECallback struct {
// }

// func (c SLICECallback) Handle(ind xapp.Swig_slice_ind_msg_t) {
// 	slice.SliceIndToDictJSON(ind)
// }

// // ------------------------------------------------------------------------ //
// //	POLICY ENFORCEMENT CALLBACK
// //
// //	The function is called internally from the APIs when a new policy
// //	is received in order to enforce the new policy.
// // ------------------------------------------------------------------------ //
// func PolicyEnforcementCallback(PolicyConfiguration api.Configuration) {
	
// 	// STEP 1: Scope
// 	fmt.Println("\n[Policy]:------------------ Enforcement -------------------------")
// 	cellId := PolicyConfiguration.Scope.CellID
// 	sliceId := PolicyConfiguration.Scope.SliceID
// 	fmt.Println("[Policy]: Cell Id:", cellId, ", Slice Id:", sliceId)
// 	fmt.Println("[Policy]: Control Type:", PolicyConfiguration.Statement.CtrlType)

// 	for i:=0 ; i <= int(E2Nodes.Size()-1); i++ {
		
// 		// Get the control type and request
// 		controlType := PolicyConfiguration.Statement.CtrlType
// 		controlRequest := PolicyConfiguration.Statement.CtrlRequest
		
// 		// Send the control request
// 		msg := slice.FillSliceCtrlMsg(controlType, controlRequest)
// 		xapp.Control_slice_sm(E2Nodes.Get(i).GetId(), msg)

// 		time.Sleep(1 * time.Second)
		
// 	}
// }

// // Global Variables
// var E2Nodes xapp.E2NodeVector
// var SliceSmHandlers []int

// // ------------------------------------------------------------------------ //
// //  MAIN
// // ------------------------------------------------------------------------ //
func main() {
// 	// ----------------------- Initialization ----------------------- //
// 	args := []string{"", "-c", "slgeneric.conf"}
// 	xapp.Init(xapp.SlToStrVec(args))

// 	// Connect
// 	E2Nodes = xapp.Conn_e2_nodes()

// 	var nodes_len int64 = E2Nodes.Size()

// 	if nodes_len <= 0 {
// 		panic(fmt.Sprintf("panic"))
// 	}

// 	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	
// 	for i:=0 ; i <= int(E2Nodes.Size()-1); i++ {

// 		// ----------------------- Slice Indication ----------------------- //
// 		innerSlice := SLICECallback{}
// 		callbackSlice := xapp.NewDirectorSlice_cb(innerSlice)
// 		HndlrSlice := xapp.Report_slice_sm(E2Nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackSlice)

// 		// Append values to the slice
// 		SliceSmHandlers = append(SliceSmHandlers, HndlrSlice)

// 		time.Sleep(1 * time.Second)
		
// 	}

// 	// ----------------------- Gin Client ----------------------- //
// 	api.OpenA1Apis(PolicyEnforcementCallback, "slgeneric.conf")

// 	// ----------------------- END ----------------------- //
// 	// unsubscribe from sms
// 	for _, value := range SliceSmHandlers {
// 		xapp.Rm_report_slice_sm(value)
// 	}

// 	// Stop the xApp. Avoid deadlock.
// 	for xapp.Try_stop() == false {
// 		time.Sleep(1 * time.Second)
// 	}

// 	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}
