 # SkyRadar
SkyRadar is a compact USB-C powered desktop flight tracker built with an ESP32-C3 SuperMini and a 1.28" GC9AO1 round LCD. It pulls live ADS-B aviation data over wifi,calculates real-time polar coordinates relative to your desk , and renders a sweeping radar scope showing nearby aircraft , callsign , and plane model.

# Why I Built this
 wanted a minimal , standalone desk display that could track avtive flights over my house in real time without needing a full computer monitor or phone app running. Drawing inspiration from classic retro radar scope made of phosphor this combines only 3 electronic component hardware with live open-source flight telemetry into a single desktop piece.

# CAD design
3D enclosure model made with custom two piece enclosure featuring a 45 degree slanted viewing angle with press fit usb acess and near cap for push button.

<img width="494" height="653" alt="Screenshot 2026-10-10 at 11 09 54 PM" src="https://github.com/user-attachments/assets/817748e7-e591-4411-8686-51777c8ac17f" />

type-c extrude at back of rectangle and push button extrude at the rectangle base cover 

<img width="625" height="625" alt="Screenshot 2026-10-10 at 11 10 57 PM" src="https://github.com/user-attachments/assets/2a6158df-2186-48f9-a22c-6ef1306f0072" />

cover at the back for product 

<img width="530" height="499" alt="Screenshot 2026-10-10 at 11 11 45 PM" src="https://github.com/user-attachments/assets/b792a34c-022e-40b2-929a-79987239f8fd" />

# Circuit

<img width="545" height="644" alt="Screenshot 2026-10-10 at 11 14 27 PM" src="https://github.com/user-attachments/assets/49a6be98-7902-460a-a4fb-7c8b4d232c2b" />

full pinout and wiring created in cirkitdesigner
https://app.cirkitdesigner.com/
this is where you can find ESP's

### Reference Pin Map
This project follows a point to point wiring foloow this connection to connect the GC9AO1 LCD , tactical button to the ESP32-C3 supermini.

| Component Pin | ESP32-C3 SuperMini Pin | Function |
| :--- | :--- | :--- |
| GC9AO1 VCC | 5V | Display power supply |
| GC9AO1 GND | GND | system common ground |
| GC9AO1 SCL | GPIO04 | SPI clock signal |
| GC9AO1 SDA | GPIO06 | SPI MOSI data signal |
| GC9AO1 DC | GPIO02 | Data/command selection |
| GC9AO1 CS | GPIO07 | SPI chip selection |
| GC9AO1 RST | GPIO03 | range toggle switch signal |
| Button T1 | GPIO09 | range toggle signal |
| Button T2 | GND | Button ground | 

# Assembly
print the SkyRadar_Bezel file and SkyRadar_cover file using PLA or PETG at a 0.2mm layer height with 20% infill

Press fit the screen into the front ring slanted bezel route the wires and solder screen , button and esp through the internal neck which connects bezel and base box press fit 6X6 button on the top of rectangle cover and make sure the type-c is exposed through back port wall of rectangle 

# Flash
ope arduino IDE 2.0 and add the ESP32 board manager package 
[https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json](https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json)

Install libraries through tools -> manage libraries ad install 
TFT_eSPI and ArduinoJson open and configure driver 

# WIFI credetials updation
update your local WIFI SSID , password and your desk exact location lon,lat coordinates

# Bill of material(BOM)

| Item | Quantity | Aprox. cost | Link/Source |
| :--- | :--- | :--- | :--- |
| ESP32-C3 SuperMini Development Board | 1 | $4 | https://robu.in/product/esp32-c3-development-board-esp32-supermini/ |
| 1.28" GC9A01 Round LCD Display Module (240x240 SPI) | 1 | $5 | https://robu.in/product/goldenmorning-gc9a01-7p-240x240-spi-1-28-inch-round-display-module/ |
| 6x6mm Tactile Push Button | 1 | $0.031 | https://robu.in/product/6x6x5mm-tactile-push-button-switch-pack-of-20/ |
| PLA Filament ~40g | 1 | $1 | Self printed |
| 28AWG Silicone Stranded Wire Set | 1 | $5 | https://robu.in/product/pro-range-28awg-silicone-wire-kit-5colors-x-5m25m/ |
| USB-C Cable (Power Data) | 1 | $3 | https://www.amazon.in/boAt-Charging-Transfer-Tangle-Free-Premium/dp/B0FMRLJ9LF/ref=sr_1_3?dib=eyJ2IjoiMSJ9.t6lfHNEBa_Tei_6mBPivsbFkSU8TuxUSBKN7-zmbzoq_MiK3C2RbvzpuyYVL9KhJsy1gOAe6-52xIVkY7LHv-acYnfWDWBDblETAHYT7UCDB94DpCOgq4BzADc5Yy0Ivp697fZfdMe1XDbRH5sTuITvjHDuse-QmK-c1-J7_ZqcvSaI7tN6HxZTrToR09xfgjvJHFVGv37ZODbV7-MF4gYbVVlQB_Chc1_QxDNbw8Bs.A64j5Q6nt-mD7K39a95e216q-y0OOMjUOUVO1vFQ2mQ&dib_tag=se&keywords=usb%2Bc%2Bcharging%2Bcable&qid=1791645581&sr=8-3&th=1 |
| Total cost | Aprox. ~$18.031 | | |

# Known Issues 
Open sky network rate limit: unregistered API request can occassionally return HTTP code 429 during peak hours.
Solution: create free account to opensky-network.org and pass authentication header inside fetchLiveFlightData(): 

Display backlight bleed in dark rooms: the screen remains continuously illuminated even when no aircraft are in range
Solution: add timer in loop() that dims the screen backlight pin after 5 minutes(desired time) of inactive waking it up by switch press. 

# Credits
* **[OpenSky Network](https://opensky-network.org/)** - For providing free live ADS-B flight data via REST API
* **[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)** - Display driver library by Bodmer for driving the GC9A01 screen over SPI
* **[Hack Club Forge](https://forge.hackclub.com/)** - For supporting projects
