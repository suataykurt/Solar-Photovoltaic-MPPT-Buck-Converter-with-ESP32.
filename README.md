# Solar Photovoltaic MPPT Buck Converter with ESP32

An end-to-end power electronics engineering project featuring a custom PCB, embedded firmware, and Simulink verification. This system extracts the maximum available power from a solar photovoltaic (PV) panel using a Buck (step-down) converter architecture driven by the Perturb and Observe (P&O) algorithm.

## 📋 Project Overview
Solar panels have non-linear voltage-current (V-I) characteristics heavily dependent on irradiance and temperature. Directly connecting a load to a PV panel results in severe power mismatch. This project solves that inefficiency by implementing an active DC-DC Buck converter. Controlled by an ESP32-WROOM-32 module, the system constantly monitors panel voltage and current, dynamically adjusting the high-side MOSFET's PWM duty cycle to lock the panel at its Maximum Power Point (MPP).

## 🛠 Technical Architecture
The project is built across three core engineering disciplines:

### 1. Mathematical Simulation (MATLAB/Simulink)
- Modeled a 213W PV array connected to an ideal Buck converter topology.
- Implemented and verified the P&O algorithm to ensure stable duty cycle convergence.
- Achieved ~96.5% tracking efficiency, extracting a stable 205W at the 29V MPP.

### 2. Embedded Firmware (ESP32 C/C++)
- **Sensors:** Reads PV voltage via a resistor divider and current via an ACS712 Hall-effect sensor module, utilizing a 16-sample moving average software filter.
- **Hardware PWM:** Uses the ESP32 LEDC peripheral to generate a clean, non-blocking 20 kHz, 10-bit PWM signal.
- **MPPT Loop:** Executes the P&O algorithm every 5 milliseconds to continuously optimize the duty cycle.
- **Telemetry:** Streams live $V_{pv}$, $I_{pv}$, $P_{pv}$, and Duty Cycle data to the Serial Plotter every 50ms, acting as a virtual oscilloscope.

### 3. Hardware & PCB Design (KiCad 7)
- **Power Stage:** IRFZ44N N-Channel Power MOSFET, MBR20100CT Schottky freewheeling diode, and a 1mH Toroidal Inductor.
- **Gate Drive:** IR2104 High-Side MOSFET driver with a bootstrap circuit (1N4148 and 1uF capacitor).
- **PCB Layout:** A 2-layer board layout optimized for power electronics, featuring widened high-current traces, proper component keep-out zones for the ESP32 antenna, and a continuous bottom copper ground plane (B.Cu) for noise reduction and EMI mitigation.

## 🚀 Key Features
- **Perturb and Observe (P&O):** A robust algorithm that continually seeks the peak of the PV power curve.
- **High-Frequency Switching:** Operates at 20 kHz to minimize inductor size and output voltage ripple.
- **Open-Circuit Protection:** Automatically cuts PWM output if panel voltage drops below 12V.
- **Production Ready:** Includes fully DRC-cleared Gerber files and a Bill of Materials (BOM) ready for fabrication (JLCPCB/PCBWay).

## 📂 Project Structure
To maintain an organized repository, the files have been structured as follows:

- `Simulink/`
  - `mppt_buck_model.slx`: The main Simulink model file.
  - `matlab_code.m`: MATLAB initialization script for workspace variables.
  - `matlab_sim.jpg`: Screenshot of the simulation results.
- `Firmware/`
  - `esp_code.ino`: The main embedded C++ source code for the ESP32.
- `Hardware/`
  - `mppt_buck_model.kicad_sch`: KiCad schematic file.
  - `mppt_buck_model.kicad_pcb`: KiCad PCB layout file.
  - `mppt_buck_model.kicad_pro`: KiCad project file.
  - `mppt_buck_model.csv`: Bill of Materials (BOM) for component sourcing.
  - `Images/`: Contains 3D renders of the PCB (`mppt_buck_model.png`, `mppt_buck_model_backside.png`).
- `Hardware/Gerber_Outputs/`
  - Directory containing all `.gbr` (Copper, Mask, Paste, Silkscreen, Edge Cuts) and `.drl` files required for PCB manufacturing.

## 💡 How to Use
1. **Simulation:** Open `matlab_code.m` in MATLAB to load workspace parameters, then run `mppt_buck_model.slx` to observe the theoretical MPPT tracking.
2. **Firmware:** Open `esp_code.ino` using the Arduino IDE (with ESP32 board manager installed) or PlatformIO. Compile and flash it to your ESP32 module.
3. **Hardware Fabrication:** Zip the contents of the `Hardware/Gerber_Outputs/` directory and upload it to a PCB manufacturer. Use the `mppt_buck_model.csv` BOM file to order components.

---
*Developed by Suat Aykurt*