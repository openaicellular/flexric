package callbacks

import (
	slice "build/examples/xApp/go/FlexPolicy/utils/slice"
	xapp "build/examples/xApp/go/xapp_sdk"
	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
	
	"fmt"
	"time"
)

func init() {
	RegisterCallback("SLICE_ENFORCE", CallbackSliceEnforce)
}

// CallbackSliceEnforce is an example callback function.
func CallbackSliceEnforce(PolicyConfiguration policy.Configuration) {

	// STEP 1: Scope
	fmt.Println("\n[Policy]:------------------ Enforcement -------------------------")
	cellId := PolicyConfiguration.Scope.CellID
	sliceId := PolicyConfiguration.Scope.SliceID
	fmt.Println("[Policy]: Cell Id:", cellId, ", Slice Id:", sliceId)
	fmt.Println("[Policy]: Control Type:", PolicyConfiguration.Statement.CtrlType)

	// STEP 2: Enforce
	for i:=0 ; i <= int(sm.E2Nodes.Size()-1); i++ {
		
		// Get the control type and request
		controlType := PolicyConfiguration.Statement.CtrlType
		controlRequest := PolicyConfiguration.Statement.CtrlRequest
		
		// Send the control request
		msg := slice.FillSliceCtrlMsg(controlType, controlRequest)
		xapp.Control_slice_sm(sm.E2Nodes.Get(i).GetId(), msg)

		time.Sleep(1 * time.Second)
		
	}

}
