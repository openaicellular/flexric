package utils

import "C"
import (
	slice "build/examples/xApp/go/a1/utils/slice"

	"fmt"
	"log"
	"time"

	"context"
	"github.com/gin-gonic/gin"
	"net/http"
	"gopkg.in/ini.v1"
)


// ------------------------------------------------------------------------ //
//	Define Global Variables for the xApp API Server
//	The APIs are used to receive policies and sent the current state from/to the Non-RT RIC
// ------------------------------------------------------------------------ //
var Router *gin.Engine
var Srv *http.Server
var ServerFinished bool

// ------------------------------------------------------------------------ //
//	Global structures for Policy configuration
//	- PolicyConfiguration: the latest policy configuration
//	- PrevPolicyConfiguration: the previous policy configuration
// ------------------------------------------------------------------------ //
var PolicyConfiguration Configuration
var PrevPolicyConfiguration Configuration
var Initialized bool = false


// ------------------------------------------------------------------------ //
//	ParseXAppConfig function for parsing the xApp configuration
// ------------------------------------------------------------------------ //
func ParseXAppConfig(name string) (string, int) {
	cfg, err := ini.Load(name)
	if err != nil {
		log.Fatalf("Fail to read file: %v", err)
	}

	xappSection, err := cfg.GetSection("XAPP")
	if err != nil {
		log.Fatalf("Failed to get XAPP section: %v", err)
	}

	a1IP := xappSection.Key("A1_IP").String()
	a1Port := xappSection.Key("A1_PORT").MustInt()

	fmt.Printf("A1_IP: %s\n", a1IP)
	fmt.Printf("A1_PORT: %d\n", a1Port)

	return a1IP, a1Port
}


// Channel Variable for finishing the xApp
var FinishChannel chan bool

// xApp policy configuration API
// Configuration represents the JSON configuration received by the first server.

// ------------------------------------------------------------------------ //
//	Configuration struct for storing the xApp policy configuration
//	coming from the Non-RT RIC via the APIs
// ------------------------------------------------------------------------ //
type Configuration struct {
	PolicyID   int             `json:"PolicyId,omitempty"`
	PolicyType string          `json:"PolicyType,omitempty"`
	Scope      ScopeConfig     `json:"scope,omitempty"`
	Statement  StatementConfig `json:"statement,omitempty"`
}

type ScopeConfig struct {
	SliceID int `json:"sliceId,omitempty"`
	CellID  int `json:"cellId,omitempty"`
}

type StatementConfig struct {
	// -----  For ORAN xApps -----------------//
	MaxNumberOfUEs int `json:"maxNumberOfUes,omitempty"`

	// -----  For Br (Non-ORAN) xApps --------//
	// Type of the Control Request
	//     string: "ADDMOD", "DEL", "ASSOC_UE_SLICE"
	CtrlType string `json:"type,omitempty"`

	// Requested Structure
	CtrlRequest slice.Request `json:"request,omitempty"`
}

// ------------------------------------------------------------------------ //
//	PolicyEnforcementCallback function for enforcing the policy
//	The calback function is defined in the xApp
// ------------------------------------------------------------------------ //
type PolicyEnforcementCallback func(Configuration)

// ------------------------------------------------------------------------ //
//
//	OpenA1Apis function for starting the xApp server APIs
//
//	APIs:
//	- /api/policy: for receiving the policy configuration
//	- /api/finish: for receiving the finish command
//	- /api/slice/stats: for sending the current state to the Non-RT RIC
//
//	Input Variables:
//	- policyEnforceCallback: the callback function for enforcing the policy
//  - conf: name of configuration file
//
// ------------------------------------------------------------------------ //
func OpenA1Apis(policyEnforceCallback PolicyEnforcementCallback, conf string) {

	// Create the channel for receiving the finish command
	FinishChannel = make(chan bool)

	// Parse A1 parameters
	A1IP, A1Port := ParseXAppConfig(conf)

	gin.SetMode(gin.ReleaseMode)

	Router = gin.Default()

	// [API 1]: POST /api/policy
	// Receive the policy configuration from the Non-RT RIC
	Router.POST("/api/policy", func(c *gin.Context) {
		var config Configuration
		if err := c.ShouldBindJSON(&config); err != nil {
			c.JSON(http.StatusBadRequest, gin.H{
				"error": err.Error(),
			})
			return
		}

		if Initialized == true {
			// Update the previous policy configuration
			PrevPolicyConfiguration = PolicyConfiguration
		}

		// Store the received policy configuration
		PolicyConfiguration = config

		// Show that xApp is initialized
		if Initialized == false {
			Initialized = true
		}

		// Call the callback function for enforcing the policy
		go policyEnforceCallback(PolicyConfiguration)

		// Send the JSON response
		c.JSON(http.StatusOK, gin.H{
			"message": "Configuration received",
		})
	})

	// [API 2]: POST /api/finish
	// Receive the finish command from the Non-RT RIC
	Router.POST("api/finish", func(c *gin.Context) {

		fmt.Println("Received finish command in Client")

		// Send the JSON response
		response := map[string]interface{}{
			"message": "Finish command received",
		}
		c.JSON(http.StatusOK, response)

		// Send the finish command to the xApp
		FinishChannel <- true
	})

	// [API 3]: GET /api/slice/stats
	// Send the current state to the Non-RT RIC
	Router.GET("/api/slice/stats", func(c *gin.Context) {
		// lock
		slice.Mutex.Lock()
		c.JSON(http.StatusOK, slice.SliceStats)
		// unlock
		slice.Mutex.Unlock()
	})

	// ----------------------- Gin Server ----------------------- //
	Srv = &http.Server{
		Addr:    fmt.Sprintf("%s:%d", A1IP, A1Port),
		Handler: Router,
	}

	// ----------------------- Start Gin Server ----------------------- //
	go func() {
		// service connections
		if err := Srv.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			log.Fatalf("listen: %s\n", err)
		}
	}()

	// ----------------------- Wait for Finish Command ----------------------- //
	<-FinishChannel

	log.Println("Shutdown API server ...")

	ctx, cancel := context.WithTimeout(context.Background(), 3*time.Second)
	defer cancel()
	if err := Srv.Shutdown(ctx); err != nil {
		log.Fatal("Server Shutdown:", err)
	}
	select {
	case <-ctx.Done():
		log.Println("Timeout of 3 seconds.")
	}
	log.Println("Server exiting")
}