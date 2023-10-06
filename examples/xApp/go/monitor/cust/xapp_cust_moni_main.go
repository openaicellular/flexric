package main

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	"fmt"
	"time"
	"os"
)

// Indication Callback MAC Function
type MACCallback struct {

}

func (mac_cb MACCallback) Handle(ind xapp.Swig_mac_ind_msg_t) {
	if ind.GetUe_stats().Size() > 0 {
		nowTime := time.Now().UnixNano()/1000   //in microseconds
		indTime := ind.GetTstamp() / 1.0
		diff := nowTime - indTime
		fmt.Printf("MAC Indication latency %f microsecs\n", float64(diff))
	}	
}

// Indication Callback RLC Function
type RLCCallback struct {

}

func (rlc_cb RLCCallback) Handle(ind xapp.Swig_rlc_ind_msg_t) {
	if ind.GetRb_stats().Size() > 0 {
		nowTime := time.Now().UnixNano()/1000   //in microseconds
		indTime := ind.GetTstamp() / 1.0
		diff := nowTime - indTime
		fmt.Printf("RLC Indication latency %f microsecs\n", float64(diff))
	}	
}

// Indication Callback PDCP Function
type PDCPCallback struct {

}

func (pdcp_cb PDCPCallback) Handle(ind xapp.Swig_pdcp_ind_msg_t) {
	if ind.GetRb_stats().Size() > 0 {
		nowTime := time.Now().UnixNano()/1000   //in microseconds
		indTime := ind.GetTstamp() / 1.0
		diff := nowTime - indTime
		fmt.Printf("PDCP Indication latency %f microsecs\n", float64(diff))
	}	
}

// Indication Callback GTP Function
type GTPCallback struct {

}

func (gtp_cb GTPCallback) Handle(ind xapp.Swig_gtp_ind_msg_t) {
	if ind.GetGtp_stats().Size() > 0 {
		nowTime := time.Now().UnixNano()/1000   //in microseconds
		indTime := ind.GetTstamp() / 1.0
		diff := nowTime - indTime
		fmt.Printf("GTP Indication latency %f microsecs\n", float64(diff))
	}
}



// ------------------------------------------------------------------------ //
//	MAIN
// ------------------------------------------------------------------------ //
func main() {
	// ----- STEP 1: Init ----- //
	// E42 Setup
	xapp.Init(xapp.SlToStrVec(os.Args))

	// E2 State
	var nodes xapp.E2NodeVector = xapp.Conn_e2_nodes()

	if nodes.Size() <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes.Size())

	// ----- STEP 2: Indication ----- //
	// SM Subscription
	var handleSlice []int
	var handleSlice1[]int
	var handleSlice2[]int
	var handleSlice3[]int

	
	for i := 0; i <= int(nodes.Size()-1); i++ {
		// ----------------------- MAC Indication ----------------------- //
		innerMac := MACCallback{}
		callbackMac := xapp.NewDirectorMac_cb(innerMac)
		HndlrMac := xapp.Report_mac_sm(nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackMac)

		//------------------------ RLC Indication ----------------------- //
        
		innerRlc := RLCCallback{}
		callbackRlc := xapp.NewDirectorRlc_cb(innerRlc)
		HndlrRlc := xapp.Report_rlc_sm(nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackRlc)
        
		//------------------------ PDCP Indication ----------------------- //

		innerPdcp := PDCPCallback{}
		callbackPdcp := xapp.NewDirectorPdcp_cb(innerPdcp)
		HndlrPdcp := xapp.Report_pdcp_sm(nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackPdcp)

		// ----------------------- GTP Indication ----------------------- //
        innerGtp := GTPCallback{}
        callbackGtp := xapp.NewDirectorGtp_cb(innerGtp)
        HndlrGtp := xapp.Report_gtp_sm(nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackGtp)

		// Append values to the slice
		handleSlice = append(handleSlice, HndlrMac)
		handleSlice1 = append(handleSlice1, HndlrRlc)
		handleSlice2 = append(handleSlice2, HndlrPdcp)
		handleSlice3 = append(handleSlice3, HndlrGtp)
	}

	time.Sleep(10 * time.Second)

	// ----- STEP 5: Exit ----- //
	// Mac SM Unsubscribe
	for _, value := range handleSlice {
		xapp.Rm_report_mac_sm(value)
	}
	for _, value := range handleSlice1 {
		xapp.Rm_report_rlc_sm(value)
	}
	for _, value := range handleSlice2 {
		xapp.Rm_report_pdcp_sm(value)
	}
	for _, value := range handleSlice3 {
        xapp.Rm_report_gtp_sm(value)
    }

	// E42 Teardown
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}