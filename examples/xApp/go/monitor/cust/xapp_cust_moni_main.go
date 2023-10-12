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

// ------------------------------------------------------------------------ //
//	MAIN
// ------------------------------------------------------------------------ //
func main() {
	// ----- STEP 1: Init ----- //
	// E42 Setup
	xapp.Init(xapp.SlToStrVec(os.Args))
	// Get custom conf section, ex: "xAPP_A1"
// 	a1 := xapp.Get_conf("xApp_A1")
// 	a1ip := a1.Get("ip")
// 	a1port := a1.Get("port")
// 	a1policy := a1.Get("policy")
//     fmt.Printf("A1: IP %s, PORT %s, POLICY %s\n", a1ip, a1port, a1policy)
    // Get cust sm
    cust_sm := xapp.Get_cust_sm_conf()

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

	
	for i := 0; i < int(nodes.Size()); i++ {

        for j := 0; j < int(cust_sm.Size()); j++ {
            smInfo := cust_sm.Get(j)
            smName := smInfo.GetName()
            smTime := smInfo.GetTime()
            tti := get_cust_tti(smTime)
            if smName == "MAC" {
                // ----------------------- MAC Indication ----------------------- //
                innerMac := MACCallback{}
                callbackMac := xapp.NewDirectorMac_cb(innerMac)
                HndlrMac := xapp.Report_mac_sm(nodes.Get(int(i)).GetId(), tti, callbackMac)
                handleSlice = append(handleSlice, HndlrMac)
            } else if smName == "RLC" {
                //------------------------ RLC Indication ----------------------- //
                innerRlc := RLCCallback{}
                callbackRlc := xapp.NewDirectorRlc_cb(innerRlc)
                HndlrRlc := xapp.Report_rlc_sm(nodes.Get(int(i)).GetId(), tti, callbackRlc)
                handleSlice1 = append(handleSlice1, HndlrRlc)
            } else if smName == "PDCP" {
                //------------------------ PDCP Indication ----------------------- //
                innerPdcp := PDCPCallback{}
                callbackPdcp := xapp.NewDirectorPdcp_cb(innerPdcp)
                HndlrPdcp := xapp.Report_pdcp_sm(nodes.Get(int(i)).GetId(), tti, callbackPdcp)
                handleSlice2 = append(handleSlice2, HndlrPdcp)
            } else if smName == "GTP" {
                // ----------------------- GTP Indication ----------------------- //
                innerGtp := GTPCallback{}
                callbackGtp := xapp.NewDirectorGtp_cb(innerGtp)
                HndlrGtp := xapp.Report_gtp_sm(nodes.Get(int(i)).GetId(), tti, callbackGtp)
                handleSlice3 = append(handleSlice3, HndlrGtp)
            } else {
                fmt.Printf("not yet implemented function to send subscription for %s\n", smName)
            }
        }
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