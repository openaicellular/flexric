package main

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	"fmt"
	"time"
	utils "build/examples/xApp/go/utils/slice_utils"
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
//  MAIN
// ------------------------------------------------------------------------ //
func main() {

	xapp.Init()

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
	s1_params_nvs := utils.SliceAlgoParams{PctRsvd: 0.5}
	s1_nvs := utils.Slice{
		Id:          0,
		Label:       "s1",
		UeSchedAlgo: "PF",
		Type:        "SLICE_SM_NVS_V0_CAPACITY",
		SliceAlgoParams: s1_params_nvs}

	// Idle slice
	s2_params_nvs := utils.SliceAlgoParams{PctRsvd: 0.05}
	s2_nvs := utils.Slice{
		Id:          2,
		Label:       "idle",
		UeSchedAlgo: "PF",
		Type:        "SLICE_SM_NVS_V0_CAPACITY",
		SliceAlgoParams: s2_params_nvs}

	// AddStaticSlices
	addNvsSlicesCap := utils.Request{
		NumSlices:      2,
		SliceSchedAlgo: "NVS",
		Slices:         []utils.Slice{s1_nvs, s2_nvs},
	}

	msg6 := utils.FillSliceCtrlMsg("ADDMOD", addNvsSlicesCap)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg6)
	time.Sleep(1 * time.Second)

	// ----------------------- Slice Control Add ----------------------- //
	s1_params := utils.SliceAlgoParams{PosLow: 0, PosHigh: 2}
	s1 := utils.Slice{
		Id:              0,
		Label:           "s1",
		UeSchedAlgo:     "PF",
		SliceAlgoParams: s1_params}

	s2_params := utils.SliceAlgoParams{PosLow: 3, PosHigh: 10}
	s2 := utils.Slice{
		Id:              2,
		Label:           "s2",
		UeSchedAlgo:     "PF",
		SliceAlgoParams: s2_params}

	s3_params := utils.SliceAlgoParams{PosLow: 11, PosHigh: 13}
	s3 := utils.Slice{
		Id:              5,
		Label:           "s3",
		UeSchedAlgo:     "PF",
		SliceAlgoParams: s3_params}

	// AddStaticSlices
	AddStaticSlices := utils.Request{
		NumSlices:      3,
		SliceSchedAlgo: "STATIC",
		Slices:         []utils.Slice{s1, s2, s3},
	}

	msg := utils.FillSliceCtrlMsg("ADDMOD", AddStaticSlices)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg)
	time.Sleep(5 * time.Second)

	// ----------------------- SLICE CTRL ASSOC ----------------------- //
	// AddStaticSlices
	ues := utils.Ue{
		Rnti:           0,
		AssocDlSliceId: 2,
	}
	assocUeSlice := utils.Request{
		NumUes: 1,
		Ues:    []utils.Ue{ues},
	}
	msg2 := utils.FillSliceCtrlMsg("ASSOC_UE_SLICE", assocUeSlice)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg2)
	time.Sleep(5 * time.Second)

	// ----------------------- SLICE CTRL DEL ----------------------- //
	del := utils.Request{
		NumDlSlices:     1,
		DeleteDlSliceId: []int{5},
	}
	msg3 := utils.FillSliceCtrlMsg("DEL", del)
	xapp.Control_slice_sm(conn.Get(nodeIdx).GetId(), msg3)
	time.Sleep(2 * time.Second)

	// ----------------------- SLICE CTRL RESET ----------------------- //
	reset := utils.Request{
		NumSlices: 0,
	}
	msg4 := utils.FillSliceCtrlMsg("ADDMOD", reset)
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
