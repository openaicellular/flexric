package parser

import (
	"gopkg.in/ini.v1"
	"log"
	"strings"
	"fmt"
)


//	ParseXAppConfig function for parsing the xApp A1 configuration
func ParseXAppConfig(name string) (string, int, []string, string) {
	cfg, err := ini.Load(name)
	if err != nil {
		log.Fatalf("Fail to read file: %v", err)
	}

	xappSection, err := cfg.GetSection("XAPP-A1")
	if err != nil {
		log.Fatalf("Failed to get XAPP section: %v", err)
	}

	a1IP := xappSection.Key("A1_IP").String()
	a1Port := xappSection.Key("A1_PORT").MustInt()
	sm := xappSection.Key("SM").String()
	policyID := xappSection.Key("POLICY").String()

	// Parsing SM as a list, even if it contains a single item
	smList := strings.Split(strings.TrimSpace(sm), ", ")

	fmt.Printf("A1_IP: %s\n", a1IP)
	fmt.Printf("A1_PORT: %d\n", a1Port)
	fmt.Printf("SM: %v\n", smList)
	fmt.Printf("POLICY: %s\n", policyID)

	return a1IP, a1Port, smList, policyID
}