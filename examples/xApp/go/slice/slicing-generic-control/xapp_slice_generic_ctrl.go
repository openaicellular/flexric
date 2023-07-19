package main

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	utils "build/examples/xApp/go/utils/slice_utils"


	"fmt"
	"time"
)

// ------------------------------------------------------------------------ //
//  SLICE INDICATION CALLBACK
// ------------------------------------------------------------------------ //
type SLICECallback struct {
}

func (c SLICECallback) Handle(ind xapp.Swig_slice_ind_msg_t) {
	utils.SliceIndToDictJSON(ind)
}


// ------------------------------------------------------------------------ //
//  Policy Enforcement Callback
// ------------------------------------------------------------------------ //
func PolicyEnforcement(PolicyConfiguration utils.Configuration){
	fmt.Println("\n[Policy]:------------------ Enforcement -------------------------")
	cellId := utils.PolicyConfiguration.Scope.CellID
	sliceId := utils.PolicyConfiguration.Scope.SliceID
	fmt.Println("[Policy]: Cell Id:", cellId, ", Slice Id:", sliceId)
	fmt.Println("[Policy]: Control Type:", utils.PolicyConfiguration.Statement.CtrlType)


	// Get the control type and request
	controlType := utils.PolicyConfiguration.Statement.CtrlType
	controlRequest := utils.PolicyConfiguration.Statement.CtrlRequest

	// Send the control request
	msg := utils.FillSliceCtrlMsg(controlType, controlRequest)
	xapp.Control_slice_sm(utils.Conn.Get(utils.NodeIdx).GetId(), msg)
	time.Sleep(1 * time.Second)
}


// ------------------------------------------------------------------------ //
//  MAIN
// ------------------------------------------------------------------------ //
func main() {
	// ----------------------- Initialization ----------------------- //
	utils.Init()

	// Connect
	utils.Conn = xapp.Conn_e2_nodes()

	var nodes_len int64 = utils.Conn.Size()

	if nodes_len <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	utils.NodeIdx = 0

	// ----------------------- SLICE Indication ----------------------- //
	inner := SLICECallback{}
	callback := xapp.NewDirectorSlice_cb(inner)
	utils.Hndlr = xapp.Report_slice_sm(utils.Conn.Get(utils.NodeIdx).GetId(), xapp.Interval_ms_5, callback)
	time.Sleep(1 * time.Second)

	// ----------------------- Gin Client ----------------------- //
	utils.StartXAppServer(PolicyEnforcement)
	
	// ----------------------- END ----------------------- //
	xapp.Rm_report_slice_sm(utils.Hndlr)
	// --------------------------------------------------------- //

	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}
