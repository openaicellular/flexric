package sm

import (
	xapp "build/examples/xApp/go/xapp_sdk"
	slice "build/examples/xApp/go/FlexPolicy/utils/slice"
	mac "build/examples/xApp/go/FlexPolicy/utils/mac"
	"strings"
	"time"
)

// Service Model subscription
var E2Nodes xapp.E2NodeVector

var SliceSmHandlers []int
var MacSmHandlers []int

var SmList []string

func SmSubscription(sms []string) {

	SmList = sms

	// Iterating over smList
	for _, item := range sms {
		switch strings.TrimSpace(item) {
		case "MAC":

			// Do something when item is SLICE
			for i:=0 ; i <= int(E2Nodes.Size()-1); i++ {

				// ----------------------- MAC Indication ----------------------- //
				innerMac := mac.MACCallback{}
				callbackMac := xapp.NewDirectorMac_cb(innerMac)
				HndlrMac := xapp.Report_mac_sm(E2Nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackMac)

				// Append values to the slice
				MacSmHandlers = append(MacSmHandlers, HndlrMac)

				time.Sleep(1 * time.Second)
				
			}

		case "SLICE":

			// Do something when item is SLICE
			for i:=0 ; i <= int(E2Nodes.Size()-1); i++ {

				// ----------------------- Slice Indication ----------------------- //
				innerSlice := slice.SLICECallback{}
				callbackSlice := xapp.NewDirectorSlice_cb(innerSlice)
				HndlrSlice := xapp.Report_slice_sm(E2Nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackSlice)

				// Append values to the slice
				SliceSmHandlers = append(SliceSmHandlers, HndlrSlice)

				time.Sleep(1 * time.Second)
				
			}

		default:
			// Do something
		}
	}
}


func SmUnsubscription(sms []string) {
	
	// Iterating over smList
	for _, item := range sms {
		switch strings.TrimSpace(item) {
		case "MAC":

			for _, value := range MacSmHandlers {
				xapp.Rm_report_mac_sm(value)
			}

		case "SLICE":

			for _, value := range SliceSmHandlers {
				xapp.Rm_report_slice_sm(value)
			}

		default:
			// Do something
		}
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
	for _, val := range SmList {
		switch strings.TrimSpace(val) {
		case "MAC":
			MacBool = true

		case "SLICE":
			SliceBool = true

		default:
			// Do something
		}
	}


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