# Spacecraft Telemetry Fault Detection

## Overview

This project simulates spacecraft telemetry data pipelines and injects faultswhile implementing
fault detection and finite state machine mode management logic. The system is designed to
model a spacecraft detecting faults and transitioning to safe operating modes.

## Simulation

The simulator writes one row of data to a csv per second. Simulated telemetry includes the
following: bus voltage, battery percentage, temperature, and heartbeat. Noise is injected to simulate real hardware variation, and clamps are imposed to ensure realistic limits.

## Faults

The faults that are injected include the following: heartbeat failure, undervoltage, brownout, low/critical battery, and overheating.

Heartbeat Failure: Unresponsive and unreliable systems

Undervoltage: Low voltage loss due to gradual power degradation over time

Brownout: Voltage dropping below a critical value

Low/Critical Battery: Energy depletion, different thresholds for low and critical

Overheating: Temperature rising above a safe threshold

Faults may overlap, but the system prioritizes the most critical failure.

## Detection Logic

Fault detection is a separate process from telemetry simulation. Telemetry data is read from CSV and flags are produced based on the data from defined thresholds and persistence criteria. Flags are included for the aforementioned faults.

## State Machine

The Finite State Machine keeps track of safe spacecraft operating modes:

Nominal: All systems healthy

Degraded: Faults detected, non-critical, lowered activity

Safe Mode: Critical faults detected, heavily reduced activity

Safe Mode is not meant to be recovered from in this project. However, degraded mode can be recovered from when non-critical conditions clear, but this project does not include recovery in the telemetry simulation.

## Results

The system outputs three csv files:

telemetry_data.csv: Raw simulated telemetry data

telemetry_flags.csv: Detected faults and severity

state_log.csv: Mode transitions with reason

The three systems outline a clear separation of priorities: simulation, fault detection, and response with FSM.

## Limitations

The model does not include eclipses or orbital dynamics.

Thresholds and rates are not calibrated from real data.

No recovery actions are implemented based on state machine output.

These limitations are present to ensure the project focuses on systems behavior.

## Next Steps

Modeling recovery actions from the degraded and safe modes

Refining thresholds and persistence criteria

Visualizing state transitions for analysis with Python

Implementing hardware for hardware-in-the-loop testing

## How To Run

Run the commands in this order:
```
g++ sim/telemetry_sim.cpp -o sim/telemetry
./sim/telemetry
```
```
g++ detection/fault_detection.cpp -o detection/fault
./detection/fault
```
```
g++ state/state_machine.cpp -o state/fsm
./state/fsm
```