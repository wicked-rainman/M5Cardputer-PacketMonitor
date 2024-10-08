# M5Cardputer-PacketMonitor
Summary: M5Cardputer (ESP32S3) scans for WiFi networks and associated management beacons from connected devices
This code has been written to run on an M5Cardputer, but should port to any ESP32s3 board that has an SD card interface (and suitable screen if required). A Serial GPS is attached and a GPS lock is expected. Code is easy to modify if a GPS is not available.
## Method
On startup, two Mutexs are created. One for shared memory and one to organise screen writes. A small message queue is created to hold device MAC addresses (sender and receiver). The WiFi card is then placed in promiscuous mode.

- **Task 1** GpsReader. Searches for valid GNRMC messages, stores the LAT/LON and TIME values, then sets the RTC (The RTC value is only referenced if a GPS lock is lost). After a lock has been established, spawning of tasks continues. 
- **Task 2** BatteryStatus. Checks the battery state and updates a battery state indicator on screen.
- **Task 3** NetworkScan. Periodically scans for new WiFi networks and stores SSIDs/Macs Etc. A fixed sized table holds these values. If it becomes full, the table is written to SD and treated as round-robin.
- **Task 4** RollToFix. Trys to associate rolling mac addresses with fixed WiFi station mac addresses.(This associates rolling mac devices to WiFi stations, when present)
- **Task 5** ChannelHop. Cycles the WiFi card through available WiFi channels
- **Task 6** DeviceAdd. Stores any new Mac addresses found to be broadcasting Management packets. Again, a fixed size table holds these values. When it becomes full, the table is appended to a file on the SD card.
- **Task 7** KeyboardBranch. Enables user interaction with keyboard.

To spread the load, GpsReader needs the CPU cycles, so it runs on Core1 together with BatteryStatus,RollToFix and KeyboardBranch, which all spend a large amount of time idle. The other tasks time share on Core0.

## Keyboard interaction.
Keyboard interaction can only take place after all tasks have initialised.

- **'d'** Kills all current tasks. Appends current cache contents to the network and device files, then outputs the files content to the serial port. Files are not removed. Process then loops forever awaiting a reset.
- **'w'** Kills all current tasks. Wipes the network and devices files, then waits forever, awaiting a reset.
- **'L'** As 'D', but trys to resolve each fixed source mac address to an OUI. This is a slow process, better suited to a PC, but the facility is there if needed. Records are output on serial. 

## To do

1.  The OUI lookup process needs to be optimised.