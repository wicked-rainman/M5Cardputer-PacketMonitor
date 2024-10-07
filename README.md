# M5Cardputer-PacketMonitor
Summary: M5Cardputer (ESP32S3) scans for WiFi networks and associated management beacons from connected devices
This code has been written to run on an M5Cardputer, but should port to any ESP32s3 board that has an SD card interface (and suitable screen if required).
## Method

On startup, two Mutexs are created. One for shared memory and one to organise screen writes. A small message queue is created to hold device MAC addresses (sender and receiver). The WiFi card is then placed in promiscuous mode.

- task 1 is created to display and update the current battery status.
- task 2 performs a scan looking for WiFi networks and stores Mac/SSID values.
- task 3 looks for rolling mac addresses and pairs them with any other fixed mac addresses found.
- task 4 rotates the WiFi card through different channel sequences, pausing on each channel.
- task 5 is sent new management packets from the packet handler and matches them against SSIDs (If found).
- task 6 looks for a keyboard press (key D) and dumps the logs to Serial.
- A WiFi packet handler dedupes mac addresses and places them on the packet queue.
