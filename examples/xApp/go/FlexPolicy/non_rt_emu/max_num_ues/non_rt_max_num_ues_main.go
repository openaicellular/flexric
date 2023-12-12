package main

import "C"
import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"math/rand"
	"net/http"
	"time"
	//"os"
	//"os/signal"
	//"syscall"
	//"context"
	"log"

	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
)

var server1URL = "http://127.0.0.10:7000/api/policy"

func main() {
	// ----------------- GET Feedback ----------------- //
	resp, err := http.Get("http://127.0.0.10:7000/api/feedback")
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

	// Convert the byte slice to a string and print it
	fmt.Println(string(prettyJSON))

	rand.Seed(time.Now().UnixNano())

	mcc := sm.Feedback.SliceFeedback[0].Mcc
	mnc := sm.Feedback.SliceFeedback[0].Mnc
	nbId := sm.Feedback.SliceFeedback[0].NbId
	ranType := sm.Feedback.SliceFeedback[0].RanType

	for i := 0; i < 10; i++ {
		maxNumOfUes := int16(rand.Intn(2))

		config := policy.Configuration{
			PolicyID:   2,
			PolicyType: "maxNumberOfUes",
			Scope: policy.ScopeConfig{
				Mcc:     mcc,
				Mnc:     mnc,
				NbId:    nbId,
				RanType: ranType,
			},
			Statement: policy.StatementConfig{
				MaxNumberOfUEs: maxNumOfUes,
			},
		}

		err := sendConfiguration(config)
		if err != nil {
			log.Printf("Failed to send configuration: %s", err)
		} else {
			log.Println("Configuration sent successfully, [maxNumberOfUes]: ", maxNumOfUes)
		}

		time.Sleep(10 * time.Second)
	}

	// ----------------- finish ----------------- //
	url := "http://127.0.0.10:7000/api/finish"

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
