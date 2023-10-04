package main

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	"fmt"
	"time"
	"os"
)

type KPMCallback struct {
}

func (c KPMCallback) Handle(ind xapp.Swig_kpm_ind_data_t) {
	if ind.GetHdr() != nil {
		tNow := time.Now().UnixNano() / 1000
		tKpm := int64(ind.GetHdr().GetKpm_ric_ind_hdr_format_1().GetCollectStartTime()) / 1
		tDiff := float64(tNow-tKpm)

		fmt.Printf("KPM Indication tstamp %d diff %.3f E2-node type %d nb_id %d\n",
			tNow, tDiff, ind.GetId().GetXtype(), ind.GetId().GetNb_id().GetNb_id())
	}
}

// ------------------------------------------------------------------------ //
//
//	MAIN
//
// ------------------------------------------------------------------------ //
func main() {
	xapp.Init(xapp.SlToStrVec(os.Args))

	var nodes xapp.E2NodeVector = xapp.Conn_e2_nodes()
	if nodes.Size() <= 0 {
		panic(fmt.Sprintf("panic"))
	}
	fmt.Printf("Connected E2 nodes = %d\n", nodes.Size())

    var kpmHndlr []interface{}
    for i := int64(0); i < nodes.Size(); i++ {
        e2Node := nodes.Get(int(i))
    	inner := KPMCallback{}
    	callback := xapp.NewDirectorKpm_cb(inner)
    	actionSlice := []string{"DRB.PdcpSduVolumeDL", "DRB.PdcpSduVolumeUL", "DRB.RlcSduDelayDl", "DRB.UEThpDl", "DRB.UEThpUl", "RRU.PrbTotDl", "RRU.PrbTotUl"}
    	hndlr := xapp.Report_kpm_sm(e2Node.GetId(), xapp.Interval_ms_1000, xapp.SlToStrVec(actionSlice), callback)
	    kpmHndlr = append(kpmHndlr, hndlr)
	}
    time.Sleep(10 * time.Second)

	for _, hndlr := range kpmHndlr {
	    if val, ok := hndlr.(int); ok {
            xapp.Rm_report_kpm_sm(val)
        } else {
            fmt.Println("Error: hndlr is not of type int")
       }
   }

	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}
