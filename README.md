# TT Motor Turntable (ESP32 + 3D Printed)

<p align="center">
  <img src="doc/images/thumbnail.GIF" alt="TT Motor Turntable thumbnail" width="520">
</p>


A compact DIY turntable powered by a TT geared motor, a TB6612FNG motor driver and an ESP32 board, with 3D-printable parts and a simple web dashboard for control.

## What’s in this repo
- **Firmware (ESP-IDF):** `firmware/`
- **3D models / CAD:** `hardware/`
- **Docs & guides:** `doc/`

## Features
- WiFi control (web dashboard)
- Speed + direction control
- Configurable pins via ESP-IDF menuconfig

## Quick start
➡️ **Start here:** [Getting Started Guide](doc/getting_started.md)

That guide covers:
- Wiring / pin mapping
- WiFi + pin setup in `idf.py menuconfig`
- Build, flash, and monitor
- Finding the web dashboard address

## Photos / Demos
Control the turntable via the WebDashboard!
<p align="center">
  <img src="./images/web_dashboard.jpg" alt="WIFI and Driver Pins" width="400">
</p>

Displaying a mouse!
<p align="center">
  <img src="doc/images/demo.GIF" alt="Demo" width="520">
</p>


## Hardware
- STL files: `hardware`
- CAD files: `hardware/cad`

<p align="center">
  <img src="doc/images/drawing.png" alt="Drawing" width="520">
</p>



## Documentation
- Getting started: [doc/getting_started.md](doc/getting_started.md)

## License
See [LICENSE](LICENSE).