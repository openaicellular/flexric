package lifycycle

import (
	xapp "build/examples/xApp/go/xapp_sdk"
	slice "build/examples/xApp/go/a1/utils/slice"
	"strings"
	"time"
)

// Service Model subscription
var E2Nodes xapp.E2NodeVector
var SliceSmHandlers []int


func SmSubscription(sms []string) {
	// Iterating over smList
	for _, item := range sms {
		switch strings.TrimSpace(item) {
		case "MAC":

			// Do something when item is MAC

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

			// Do something when item is MAC

		case "SLICE":

			// unsubscribe from sms
			for _, value := range SliceSmHandlers {
				xapp.Rm_report_slice_sm(value)
			}

		default:
			// Do something
		}
	}
}
