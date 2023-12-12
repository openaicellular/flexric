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

func get_oran_tti(smTime int) xapp.Interval {
	if smTime == 1 {
	    return xapp.Interval_ms_1
    } else if smTime == 2 {
        return xapp.Interval_ms_2
    } else if smTime == 5 {
        return xapp.Interval_ms_5
    } else if smTime == 10 {
        return xapp.Interval_ms_10
    } else if smTime == 100 {
        return xapp.Interval_ms_100
    } else if smTime == 1000 {
        return xapp.Interval_ms_1000
    } else {
        panic("Unknown sm time\n")
    }
}

// ------------------------------------------------------------------------ //
//
//	MAIN
//
// ------------------------------------------------------------------------ //
func main() {
	xapp.Init(xapp.SlToStrVec(os.Args))
	// Get custom conf section, ex: "xAPP_A1"
// 	a1 := xapp.Get_conf("xApp_A1")
//     a1ip := a1.Get("ip")
//     a1port := a1.Get("port")
//     a1policy := a1.Get("policy")
//     fmt.Printf("A1: IP %s, PORT %s, POLICY %s\n", a1ip, a1port, a1policy)
    // Get oran sm
    oran_sm := xapp.Get_oran_sm_conf()

	var nodes xapp.E2NodeVector = xapp.Conn_e2_nodes()
	if nodes.Size() <= 0 {
		panic(fmt.Sprintf("panic"))
	}
	fmt.Printf("Connected E2 nodes = %d\n", nodes.Size())

    var kpmHndlr []interface{}
    n_handle := 0;
    for i := int64(0); i < nodes.Size(); i++ {
        e2Node := nodes.Get(int(i))
        for j := 0; j < int(oran_sm.Size()); j++ {
            smInfo := oran_sm.Get(j)
            smName := smInfo.GetName()
            if smName != "KPM" {
                continue
            }
            smTime := smInfo.GetTime()
            tti := get_oran_tti(smTime)
            // format := smInfo.GetFormat()
            ranType := smInfo.GetRan_type()
            // actLen := smInfo.GetAct_len()
            actions := smInfo.GetActions()
            var actionSlice []string
            for a := 0; a < int(actions.Size()); a++ {
                actName := actions.Get(a)
                actionSlice = append(actionSlice, actName)
            }
            // fmt.Println("Actions:", actionSlice)
            var ranTypeName string = xapp.Get_e2ap_ngran_name(e2Node.GetId().GetXtype())
            if ranTypeName == "ngran_eNB" {
                continue
            }
            if ranTypeName != ranType {
                continue
            }
            inner := KPMCallback{}
            callback := xapp.NewDirectorKpm_cb(inner)
            hndlr := xapp.Report_kpm_sm(e2Node.GetId(), tti, xapp.SlToStrVec(actionSlice), callback)
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
