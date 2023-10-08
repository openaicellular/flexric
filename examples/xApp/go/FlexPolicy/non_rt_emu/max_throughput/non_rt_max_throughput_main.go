package main

import "C"
import (
	"time"
	"net/http"
	"math/rand"
	//"os"
	//"os/signal"
	//"syscall"
	//"context"
	"log"
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"

	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
)


var server1URL = "http://127.0.0.1:7000/api/policy"

func main() {
	rand.Seed(time.Now().UnixNano())

	thps := []int16{100, 80, 30, 90, 30, 90, 30, 90, 30, 90}

	for _, maxThroughput := range thps { 

		config := policy.Configuration{
			PolicyID:   2,
			PolicyType: "maxThroughput",
			Scope: policy.ScopeConfig{
				SliceID: 0,
				CellID:  3584,
			},
			Statement: policy.StatementConfig{
				MaxThroughput: maxThroughput,
			},
		}

		err := sendConfiguration(config)
		if err != nil {
			log.Printf("Failed to send configuration: %s", err)
		} else {
			log.Println("Configuration sent successfully, [maxThroughput]: ", maxThroughput)
		}

		time.Sleep(10 * time.Second)
	}


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

	// Convert the byte slice to a string and print it
	fmt.Println(string(prettyJSON))


	// ----------------- finish ----------------- //
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