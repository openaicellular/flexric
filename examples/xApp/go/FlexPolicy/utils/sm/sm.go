package sm

import (
	mac "build/examples/xApp/go/FlexPolicy/utils/sm/mac"
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
	xapp "build/examples/xApp/go/xapp_sdk"
	"fmt"
	"time"
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

	for i := 0; i < int(E2Nodes.Size()); i++ {

		// Custom SMs
		for j := 0; j < int(custSm.Size()); j++ {
			smInfo := custSm.Get(j)
			smName := smInfo.GetName()
			smTime := smInfo.GetTime()
			tti := get_cust_tti(smTime)
			if smName == "MAC" {
				// ----------------------- MAC Indication ----------------------- //
				// "ngran_eNB_CU" "ngran_ng_eNB_CU" "ngran_gNB_CU";
				// Skip CUs
				currRanType := xapp.Get_e2ap_ngran_name(E2Nodes.Get(i).GetId().GetXtype())
				if currRanType == "ngran_eNB_CU" || currRanType == "ngran_ng_eNB_CU" || currRanType == "ngran_gNB_CU" {
					continue
				}

				innerMac := mac.MACCallback{}
				callbackMac := xapp.NewDirectorMac_cb(innerMac)
				HndlrMac := xapp.Report_mac_sm(E2Nodes.Get(i).GetId(), tti, callbackMac)
				// Append values to the slice
				MacSmHandlers = append(MacSmHandlers, HndlrMac)

				time.Sleep(1 * time.Second)
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

				// Skip CUs
				currRanType := xapp.Get_e2ap_ngran_name(E2Nodes.Get(i).GetId().GetXtype())
				if currRanType == "ngran_eNB_CU" || currRanType == "ngran_ng_eNB_CU" || currRanType == "ngran_gNB_CU" {
					continue
				}

				innerSlice := slice.SLICECallback{}
				callbackSlice := xapp.NewDirectorSlice_cb(innerSlice)
				HndlrSlice := xapp.Report_slice_sm(E2Nodes.Get(i).GetId(), tti, callbackSlice)

				// Append values to the slice
				SliceSmHandlers = append(SliceSmHandlers, HndlrSlice)

				time.Sleep(1 * time.Second)
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
	MacExists      bool
	MacFeedback    []mac.E2NodeMacStorageMap
	SliceExists    bool
	SliceFeedback  []slice.E2NodeSliceStatsMap
	PolicyEnforced bool
}

func FillFeedback(PolicyEnforced bool) FeedbackType {

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
		MacExists:      MacBool,
		MacFeedback:    mac.DeepCopyMacStats(mac.MacStats),
		SliceExists:    SliceBool,
		SliceFeedback:  slice.DeepCopySliceStats(slice.SliceStats),
		PolicyEnforced: PolicyEnforced,
	}

	return FeedBack
}
