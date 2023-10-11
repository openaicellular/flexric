package sm

import (
	xapp "build/examples/xApp/go/xapp_sdk"
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
	mac "build/examples/xApp/go/FlexPolicy/utils/sm/mac"
	"fmt"
)

// Service Model subscription
var E2Nodes xapp.E2NodeVector

var SliceSmHandlers []int
var MacSmHandlers []int

var globalCustSm xapp.SwigSubCustSmVector
var globalOranSm xapp.SwigSubOranSmVector

func SmSubscription(custSm xapp.SwigSubCustSmVector, oranSm xapp.SwigSubOranSmVector) {

	// store sms
	globalCustSm = custSm
    globalOranSm = oranSm

	
	for i:=0 ; i <= int(E2Nodes.Size()-1); i++ {

		// Custom SMs
        for j := 0; j < int(custSm.Size()); j++ {
            smInfo := custSm.Get(j)
            smName := smInfo.GetName()
            smTime := smInfo.GetTime()
            tti := get_cust_tti(smTime)
            if smName == "MAC" {
                // ----------------------- MAC Indication ----------------------- //
                innerMac := mac.MACCallback{}
                callbackMac := xapp.NewDirectorMac_cb(innerMac)
                HndlrMac := xapp.Report_mac_sm(E2Nodes.Get(int(i)).GetId(), tti, callbackMac)
                // Append values to the slice
				MacSmHandlers = append(MacSmHandlers, HndlrMac)
            } else if smName == "RLC" {
                // TODO: //------------------------ RLC Indication ----------------------- //
                // innerRlc := RLCCallback{}
                // callbackRlc := xapp.NewDirectorRlc_cb(innerRlc)
                // HndlrRlc := xapp.Report_rlc_sm(E2Nodes.Get(int(i)).GetId(), tti, callbackRlc)
                // handleSlice1 = append(handleSlice1, HndlrRlc)
            } else if smName == "PDCP" {
                // TODO: //------------------------ PDCP Indication ----------------------- //
                // innerPdcp := PDCPCallback{}
                // callbackPdcp := xapp.NewDirectorPdcp_cb(innerPdcp)
                // HndlrPdcp := xapp.Report_pdcp_sm(E2Nodes.Get(int(i)).GetId(), tti, callbackPdcp)
                // handleSlice2 = append(handleSlice2, HndlrPdcp)
            } else if smName == "GTP" {
                // TODO: // ----------------------- GTP Indication ----------------------- //
                // innerGtp := GTPCallback{}
                // callbackGtp := xapp.NewDirectorGtp_cb(innerGtp)
                // HndlrGtp := xapp.Report_gtp_sm(E2Nodes.Get(int(i)).GetId(), tti, callbackGtp)
                // handleSlice3 = append(handleSlice3, HndlrGtp)
            } else if smName == "SLICE" {
				// ----------------------- Slice Indication ----------------------- //
				innerSlice := slice.SLICECallback{}
				callbackSlice := xapp.NewDirectorSlice_cb(innerSlice)
				HndlrSlice := xapp.Report_slice_sm(E2Nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackSlice)

				// Append values to the slice
				SliceSmHandlers = append(SliceSmHandlers, HndlrSlice)
			} else {
                fmt.Printf("not yet implemented function to send subscription for %s\n", smName)
            }
        }

		// TODO: ORAN SMs
		// e2Node := E2Nodes.Get(int(i))
        // for j := 0; j < int(oranSm.Size()); j++ {
        //     smInfo := oranSm.Get(j)
        //     smName := smInfo.GetName()
        //     if smName != "KPM" {
        //         continue
        //     }
        //     smTime := smInfo.GetTime()
        //     tti := get_oran_tti(smTime)
        //     // format := smInfo.GetFormat()
        //     ranType := smInfo.GetRan_type()
        //     // actLen := smInfo.GetAct_len()
        //     actions := smInfo.GetActions()
        //     var actionSlice []string
        //     for a := 0; a < int(actions.Size()); a++ {
        //         actName := actions.Get(a)
        //         actionSlice = append(actionSlice, actName)
        //     }
        //     // fmt.Println("Actions:", actionSlice)
        //     var ranTypeName string = xapp.Get_e2ap_ngran_name(e2Node.GetId().GetXtype())
        //     if ranTypeName == "ngran_eNB" {
        //         continue
        //     }
        //     if ranTypeName != ranType {
        //         continue
        //     }
        //     inner := KPMCallback{}
        //     callback := xapp.NewDirectorKpm_cb(inner)
        //     hndlr := xapp.Report_kpm_sm(e2Node.GetId(), tti, xapp.SlToStrVec(actionSlice), callback)
        //     kpmHndlr = append(kpmHndlr, hndlr)
        //     n_handle += 1
        // }

	}
}


func SmUnsubscription(custSm xapp.SwigSubCustSmVector, oranSm xapp.SwigSubOranSmVector) {

	// Custom SMs
	for j := 0; j < int(custSm.Size()); j++ {
		smInfo := custSm.Get(j)
		smName := smInfo.GetName()
		if smName == "MAC" {
			for _, value := range MacSmHandlers {
				xapp.Rm_report_mac_sm(value)
			}
		} else if smName == "RLC" {
			// TODO: RLC 
		} else if smName == "PDCP" {
			// TODO: PDCP
		} else if smName == "GTP" {
			// TODO: GTP
		} else if smName == "SLICE" {
			for _, value := range SliceSmHandlers {
				xapp.Rm_report_slice_sm(value)
			}
		} else {
			fmt.Printf("not yet implemented function to send unsubscription for %s\n", smName)
		}
	}

	// TODO: ORAN SMs
	// for i := 0; i < n_handle; i++ {
    //     if val, ok := kpmHndlr[i].(int); ok {
    //             xapp.Rm_report_kpm_sm(val)
    //         } else {
    //             fmt.Println("Error: hndlr is not of type int")
    //         }
    // }

}

func get_cust_tti(smTime string) xapp.Interval {
	if smTime == "1_ms" {
	    return xapp.Interval_ms_1
    } else if smTime == "2_ms" {
        return xapp.Interval_ms_2
    } else if smTime == "5_ms" {
        return xapp.Interval_ms_5
    } else if smTime == "10_ms" {
        return xapp.Interval_ms_10
    } else if smTime == "100_ms" {
        return xapp.Interval_ms_100
    } else if smTime == "1000_ms" {
        return xapp.Interval_ms_1000
    } else {
        panic("Unknown sm time\n")
    }
}

var Feedback FeedbackType

// globad structure for feedback api
type FeedbackType struct {
	MacExists bool
	MacFeedback mac.MacStorage
	SliceExists bool
	SliceFeedback slice.SliceStatsDict
	PolicyEnforced bool
}

func FillFeedback(PolicyEnforced bool) FeedbackType{
	
	MacBool := false
	SliceBool := false 

	for j := 0; j < int(globalCustSm.Size()); j++ {
		smInfo := globalCustSm.Get(j)
		smName := smInfo.GetName()
		if smName == "MAC" {
			MacBool = true
		} else if smName == "RLC" {
			// TODO: RLC 
		} else if smName == "PDCP" {
			// TODO: PDCP Indication
		} else if smName == "GTP" {
			// TODO: GTP Indication
		} else if smName == "SLICE" {
			SliceBool = true
		} else {
			fmt.Printf("not yet implemented function to send feedback for %s\n", smName)
		}
	}

	// TODO: ORAN SMs

	// Fill the feedback structure
	FeedBack := FeedbackType{
		MacExists: MacBool,
		MacFeedback: mac.DeepCopyMacStorage(mac.MacStats),
		SliceExists: SliceBool,
		SliceFeedback: slice.DeepCopySliceStatsDict(slice.SliceStats),
		PolicyEnforced: PolicyEnforced,
	}

	return FeedBack
}