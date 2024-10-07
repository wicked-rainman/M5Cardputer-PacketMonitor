# M5Cardputer-PacketMonitor
Summary: M5Cardputer (ESP32S3) scans for WiFi networks and associated management beacons from connected devices
This code has been written to run on an M5Cardputer, but should port to any ESP32s3 board that has an SD card interface (and suitable screen if required).
## Method

On startup, two Mutexs are created. One for shared memory and one to organise screen writes. A small message queue is created to hold device MAC addresses (sender and receiver). The WiFi card is then placed in promiscuous mode.

- task 1 GpsReader searches for valid GNRMC messages, stores the LAT/LON and TIME values, then sets the RTC (The RTC value is only referenced if a GPS lock is lost). After a lock has been established, spawning of tasks continues. 
- task 2 Checks the battery state and updates a battery state indicator on screen.
- task 3 Periodically scans for new WiFi networks and stores SSIDs/Macs Etc. A fixed sized table holds these values. If it becomes full, the table is written to SD and treated as round-robin.
- task 4 Trys to associate rolling mac addresses with fixed WiFi station mac addresses.(This associates rolling mac devices to WiFi stations, when present)
- task 5 Cycles the WiFi card through available WiFi channels
- task 6 Stores any new Mac addresses found to be broadcasting Management packets. Again, a fixed size table holds these values. When it becomes full, the table is appended to a file on the SD card.
- task 7 Enables user interaction with keyboard.
