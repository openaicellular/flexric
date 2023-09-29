// Package callbacks holds all the callback functions and their registration mechanism.
package callbacks

import (
	policy "build/examples/xApp/go/FlexPolicy/utils/policy"
)

// Registry for storing the callback functions against their IDs.
var Registry = make(map[string]func(policy.Configuration))

// RegisterCallback is used to register a callback function against an ID.
func RegisterCallback(id string, callback func(policy.Configuration)) {
	Registry[id] = callback
}

func FindCallback(xappID string) func(policy.Configuration){
	// find the xApp Callback ID
	if callback, found := Registry[xappID]; found {
		return callback
	} else {
		return nil
	}
}