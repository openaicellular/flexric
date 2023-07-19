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
)

// Configuration represents the JSON configuration received by the first server.
type Configuration struct {
	PolicyID   int                    `json:"PolicyId"`
	PolicyType string                 `json:"PolicyType"`
	Scope      ScopeConfig            `json:"scope"`
	Statement  StatementConfig        `json:"statement"`
}

type ScopeConfig struct {
	SliceID int `json:"sliceId"`
	CellID  int `json:"cellId"`
}

type StatementConfig struct {
	MaxNumberOfUEs int `json:"maxNumberOfUes"`
}

var server1URL = "http://localhost:9000/api/policy"

func main() {
	rand.Seed(time.Now().UnixNano())

	for i := 0; i < 2; i++ {
		maxNumOfUes := rand.Intn(2)

		config := Configuration{
			PolicyID:   2,
			PolicyType: "maxNumberOfUes",
			Scope: ScopeConfig{
				SliceID: 0,
				CellID:  3584,
			},
			Statement: StatementConfig{
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


	// finish
	url := "http://localhost:9000/api/finish"

	// Create the request body (if needed)
	// requestBody := []byte(`{"key": "value"}`)

	// Send the POST request
	resp, err := http.Post(url, "application/json", bytes.NewBuffer(nil)) // Replace nil with requestBody if you have a request body
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

func sendConfiguration(config Configuration) error {
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
