package utils

import "C"
import (
	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"

	"fmt"
	"log"
	"time"

	"context"
	"github.com/gin-gonic/gin"
	"net/http"
)

// ------------------------------------------------------------------------ //
//
//	Define Global Variables for the xApp API Server
//	The APIs are used to receive policies and sent the current state from/to the Non-RT RIC
//
// ------------------------------------------------------------------------ //
var Router *gin.Engine
var Srv *http.Server
var PolicyEnforced = false
var FinishChannel chan bool

// ------------------------------------------------------------------------ //
//
//		OpenA1Apis function for starting the xApp server APIs
//
//		APIs:
//		- /api/policy:   for receiving the policy configuration
//		- /api/finish:   for receiving the finish command to terminate
//		- /api/feedback: for sending the current state to the Non-RT RIC
//
//		Input Variables:
//		- policyEnforceCallback: the callback function for enforcing the policy
//	 - a1IP, a1Port: IP and port for the xApp API server
//
// ------------------------------------------------------------------------ //
// TODO: add a parameter for evaluating that the received policy is the desired based on the xApp logic
func OpenA1Apis(policyEnforceCallback policy.PolicyEnforcementCallback, a1IP string, a1Port string) {

	// Create the channel for receiving the finish command
	FinishChannel = make(chan bool)

	// Gin server
	gin.SetMode(gin.ReleaseMode)
	Router = gin.Default()

	// [API 1]: POST /api/policy
	// Receive the policy configuration from the Non-RT RIC
	Router.POST("/api/policy", func(c *gin.Context) {
		var config policy.Configuration
		if err := c.ShouldBindJSON(&config); err != nil {
			c.JSON(http.StatusBadRequest, gin.H{
				"error": err.Error(),
			})
			return
		}

		// TODO: Evaluate that the received policy is the desired based on the xApp logic
		// or support multiple policies

		// Call the callback function for enforcing the policy
		// TODO: Do it in a function that will call the callback function on an interval e.g. every 1 sec until it is enforced
		go policyEnforceCallback(config)

		// Send the JSON response
		c.JSON(http.StatusOK, gin.H{
			"message": "Configuration received",
		})
	})

	// [API 2]: POST /api/finish
	// Receive the finish command from the Non-RT RIC
	Router.POST("api/finish", func(c *gin.Context) {

		fmt.Println("Received finish command")

		// Send the JSON response
		response := map[string]interface{}{
			"message": "Finish command received",
		}
		c.JSON(http.StatusOK, response)

		// Send the finish command to the xApp
		FinishChannel <- true
	})

	// [API 3]: GET /api/feedback
	// Send the current state to the Non-RT RIC
	Router.GET("/api/feedback", func(c *gin.Context) {
		c.JSON(http.StatusOK, sm.FillFeedback(PolicyEnforced))
	})

	// ----------------------- Gin Server ----------------------- //
	Srv = &http.Server{
		Addr:    fmt.Sprintf("%s:%s", a1IP, a1Port),
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
