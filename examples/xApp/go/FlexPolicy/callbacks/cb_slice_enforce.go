package callbacks

import (
	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
	xapp "build/examples/xApp/go/xapp_sdk"

	"time"
)

func init() {
	RegisterCallback("SLICE_ENFORCE", CallbackSliceEnforce)
}

// CallbackSliceEnforce is an example callback function.
func CallbackSliceEnforce(PolicyConfiguration policy.Configuration) {

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

		// STEP 2: Enforce

		// Get the control type and request
		controlType := PolicyConfiguration.Statement.CtrlType
		controlRequest := PolicyConfiguration.Statement.CtrlRequest

		// Send the control request
		msg := slice.FillSliceCtrlMsg(controlType, controlRequest)
		xapp.Control_slice_sm(sm.E2Nodes.Get(i).GetId(), msg)

		time.Sleep(1 * time.Second)

	}
}
