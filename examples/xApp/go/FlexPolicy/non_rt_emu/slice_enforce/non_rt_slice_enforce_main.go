package main

import "C"
import (
	"time"
	"net/http"
	//"os"
	//"os/signal"
	//"syscall"
	//"context"
	"log"
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"

	slice "build/examples/xApp/go/FlexPolicy/utils/slice"
	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
)

var server1URL = "http://127.0.0.1:7000/api/policy"

func main() {
	time.Sleep(3 * time.Second)

	// ----------------- GET Feedback ----------------- //
	resp, err := http.Get("http://127.0.0.1:7000/api/feedback")
	if err != nil {
		fmt.Println("Error:", err)
		return
	} 
		
	fmt.Println("GET Feedback")
	
	defer resp.Body.Close()

	
	err = json.NewDecoder(resp.Body).Decode(&sm.Feedback)
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	// Marshal the interface back to JSON with indentation
	prettyJSON, err := json.MarshalIndent(sm.Feedback, "", "    ")
	if err != nil {
		fmt.Println("Error:", err)
		return
	}


	slice.SliceStats = sm.Feedback.SliceFeedback

	// Convert the byte slice to a string and print it
	fmt.Println(string(prettyJSON))

	// Use the sliceStats variable as needed
	//fmt.Println(utils.SliceStats)
	rntis := slice.ReadSliceStats("rntis", -1)
	fmt.Println("RNTIS: ",rntis)

	rntiSlice, ok := rntis.([]uint16)
	if !ok {
		fmt.Println("The interface{} does not contain a slice of int.")
		return
	}

	time.Sleep(5 * time.Second)




	// ----------------- First Policy ----------------- //
	s1_params_nvs := slice.SliceAlgoParams{PctRsvd: 0.25}
	s1_nvs := slice.Slice{
		Id:          0,
		Label:       "s1",
		UeSchedAlgo: "PF",
		Type:        "SLICE_SM_NVS_V0_CAPACITY",
		SliceAlgoParams: s1_params_nvs}

	// Idle slice
	s2_params_nvs := slice.SliceAlgoParams{PctRsvd: 0.05}
	s2_nvs := slice.Slice{
		Id:          1,
		Label:       "idle",
		UeSchedAlgo: "PF",
		Type:        "SLICE_SM_NVS_V0_CAPACITY",
		SliceAlgoParams: s2_params_nvs}

	// AddStaticSlices
	addNvsSlicesCap := slice.Request{
		NumSlices:      2,
		SliceSchedAlgo: "NVS",
		Slices:         []slice.Slice{s1_nvs, s2_nvs},
	}

	controlType := "ADDMOD"
	config := policy.Configuration{
		PolicyID:   2,
		PolicyType: "sliceGenericControl",
		Scope: policy.ScopeConfig{
			SliceID: 0,
			CellID:  3584,
		},
		Statement: policy.StatementConfig{
			CtrlType:       controlType,
			CtrlRequest:	addNvsSlicesCap,
		},
	}

	err = sendConfiguration(config)
	if err != nil {
		log.Printf("Failed to send configuration: %s", err)
	} else {
		log.Println("Configuration sent successfully, [controlType]: ", controlType)
	}

	time.Sleep(5 * time.Second)





	// ----------------- Second Policy ----------------- //
	ue := slice.Ue{
		Rnti:           rntiSlice[0],
		AssocDlSliceId: 1,
	}
	assocUeSlice := slice.Request{
		NumUes: 1,
		Ues:    []slice.Ue{ue},
	}

	controlType = "ASSOC_UE_SLICE"
	config = policy.Configuration{
		PolicyID:   2,
		PolicyType: "sliceGenericControl",
		Scope: policy.ScopeConfig{
			SliceID: 0,
			CellID:  3584,
		},
		Statement: policy.StatementConfig{
			CtrlType:       controlType,
			CtrlRequest:	assocUeSlice,
		},
	}

	err = sendConfiguration(config)
	if err != nil {
		log.Printf("Failed to send configuration: %s", err)
	} else {
		log.Println("Configuration sent successfully, [controlType]: ", controlType)
	}

	time.Sleep(5 * time.Second)




	// ----------------- Third Policy ----------------- //
	ue = slice.Ue{
		Rnti:           rntiSlice[0],
		AssocDlSliceId: 0,
	}
	assocUeSlice = slice.Request{
		NumUes: 1,
		Ues:    []slice.Ue{ue},
	}

	controlType = "ASSOC_UE_SLICE"
	config = policy.Configuration{
		PolicyID:   2,
		PolicyType: "sliceGenericControl",
		Scope: policy.ScopeConfig{
			SliceID: 0,
			CellID:  3584,
		},
		Statement: policy.StatementConfig{
			CtrlType:       controlType,
			CtrlRequest:	assocUeSlice,
		},
	}

	err = sendConfiguration(config)
	if err != nil {
		log.Printf("Failed to send configuration: %s", err)
	} else {
		log.Println("Configuration sent successfully, [controlType]: ", controlType)
	}


	time.Sleep(5 * time.Second)




	// ----------------- Finish ----------------- //
	url := "http://127.0.0.1:7000/api/finish"

	// Create the request body (if needed)
	// requestBody := []byte(`{"key": "value"}`)

	// Send the POST request
	resp, err = http.Post(url, "application/json", bytes.NewBuffer(nil)) // Replace nil with requestBody if you have a request body
	if err != nil {
		fmt.Println("Error sending request:", err)
		return
	}
	defer resp.Body.Close()

	// Check the response status code
	if resp.StatusCode != http.StatusOK {
		fmt.Println("Unexpected response status code:", resp.StatusCode)
		return
	}

	fmt.Println("POST request sent successfully")
}

func sendConfiguration(config policy.Configuration) error {
	payload, err := json.Marshal(config)
	if err != nil {
		return err
	}

	client := &http.Client{Timeout: 30 * time.Second}
	resp, err := client.Post(server1URL, "application/json", bytes.NewBuffer(payload))
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		body, _ := ioutil.ReadAll(resp.Body)
		return fmt.Errorf("failed to send configuration: %s", string(body))
	}

	return nil
}
