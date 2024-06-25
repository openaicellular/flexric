FlexRIC supports O-RAN RC SM v1.03.

At the moment, OAI RAN supports:
* `xapp_kpm_rc` - RAN control function "QoS flow mapping configuration", i.e. creating a new DRB;
* `xapp_rc_moni` - aperiodic subscription for "UE RRC State Change".

We have showcased this feature at O-RAN meetings:
1. OAI RAN + FlexRIC demo - Osaka, June 2023: 
   - 55th minute: https://zoom.us/rec/play/1JdMqJPufSsZna09aLgauwmEo-CgKYFyxKPoEIxweJ4FCDvsTjmeUFzjfSTZJn8w9-MxTG1Bb0gG3GN_.orz4ka7mAZV4o6J8?canPlayFromShare=true&from=share_recording_detail&continueMode=true&componentName=rec-play&originRequestUrl=https%3A%2F%2Fzoom.us%2Frec%2Fshare%2F-Q87TBaTxMPII0SOBA44scg7NexCM1XwhY1X2iGIeGbDHc0kIA-_n1X-I3kJDMc.4fBtTHqeXJUHMHnG&autoplay=true&startTime=1687252602000

2. OAI RAN + OSC nearRT-RIC demo - Phoenix, October 2023:
   - 4th minute: https://zoom.us/rec/play/N5mnAQUcEVRf8HN6qLYa4k7kjNq3bK4hQiYqHGv9KUoLfcR6GHiE-GvnmAudT6xccmZSbkxxYHRwTaxk.Zi7d8Sl1kQ6Sk1SH?canPlayFromShare=true&from=share_recording_detail&continueMode=true&componentName=rec-play&originRequestUrl=https%3A%2F%2Fzoom.us%2Frec%2Fshare%2FwiYXulPlAqIIDY_vLPQSGqYIj-e5Ef_UCxveMjrDNGgXLLvEcDF4v1cmVBe8imb4.WPi-DA_dfPDBQ0FH

In addition, RC SM has been further tested against Keysight RAN simulator at O-RAN PlugFest Fall 2023:
   - RAN report function "Call Process Outcome"
   - RAN control function "DRB QoS Configuration"
For more information, please refer to the corresponding xApp: `flexric/examples/xApp/c/keysight/xapp_keysight_kpm_rc.c`
