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

// need to remove this
func RanNametoInt(value string) int {
	switch value {
	case "ngran_eNB":
		return 0
	case "ngran_ng_eNB":
		return 1
	case "ngran_gNB":
		return 2
	case "ngran_eNB_CU":
		return 3
	case "ngran_ng_eNB_CU":
		return 4
	case "ngran_gNB_CU":
		return 5
	case "ngran_eNB_DU":
		return 6
	case "ngran_gNB_DU":
		return 7
	case "ngran_eNB_MBMS_STA":
		return 8
	}
	return -1
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
    n_handle := 0;
    for i := int64(0); i < nodes.Size(); i++ {
        e2Node := nodes.Get(int(i))
        var ranTypeName string = xapp.Get_e2ap_ngran_name(e2Node.GetId().GetXtype()) //this need to be fixed
        var ranTypeNameInt int = RanNametoInt(ranTypeName);
        var actionSlice []string
        inner := KPMCallback{}
        callback := xapp.NewDirectorKpm_cb(inner)
        if xapp.E2ap_ngran_gNB == ranTypeNameInt {
            actionSlice = []string{"DRB.PdcpSduVolumeDL", "DRB.PdcpSduVolumeUL", "DRB.RlcSduDelayDl", "DRB.UEThpDl", "DRB.UEThpUl", "RRU.PrbTotDl", "RRU.PrbTotUl"}
        } else if xapp.E2ap_ngran_gNB_CU == ranTypeNameInt {
            actionSlice = []string{"DRB.PdcpSduVolumeDL", "DRB.PdcpSduVolumeUL"}
        } else if xapp.E2ap_ngran_gNB_DU == ranTypeNameInt {
            actionSlice = []string{"DRB.RlcSduDelayDl", "DRB.UEThpDl", "DRB.UEThpUl", "RRU.PrbTotDl", "RRU.PrbTotUl"}
        } else if xapp.E2ap_ngran_eNB == ranTypeNameInt {
            fmt.Printf("not yet implemented eNB\n") //TODO
        } else {
    	    panic("NG-RAN Type not yet implemented\n")
        }

        //TODO
    	if xapp.E2ap_ngran_eNB != ranTypeNameInt {
            hndlr := xapp.Report_kpm_sm(e2Node.GetId(), xapp.Interval_ms_1000, xapp.SlToStrVec(actionSlice), callback)
            kpmHndlr = append(kpmHndlr, hndlr)
            n_handle += 1
	    }
	}
    time.Sleep(10 * time.Second)

// 	for _, hndlr := range kpmHndlr {
// 	    if val, ok := hndlr.(int); ok {
//             xapp.Rm_report_kpm_sm(val)
//         } else {
//             fmt.Println("Error: hndlr is not of type int")
//        }
//    }
    for i := 0; i < n_handle; i++ {
        if val, ok := kpmHndlr[i].(int); ok {
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
