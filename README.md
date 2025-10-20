# AirMouse

**AirMouse** turns an ESP32 + MPU6050 into a motion-controlled mouse. It reads gyroscope data on the ESP32, streams simple `MOVE x y` messages over serial, and a Python client reads those messages to move the system cursor in real time.

> Quick pitch: move the ESP32 in your hand and the pointer follows. Clean, low-latency, and hackable.

---

## Features

* Real-time cursor control using an MPU6050 gyroscope.
* Simple serial protocol: `MOVE <x> <y>`
* Smooth movement using a moving-average filter on the ESP32.
* Python client uses `pyautogui` to translate serial messages to OS mouse movement.

---

## What you need (Hardware)

* ESP32 development board (any common board with USB/Serial)
* MPU6050 IMU module
* Jumper wires
* USB cable to connect ESP32 to your PC

---

## Wiring

Connect the MPU6050 to the ESP32 as follows (I2C):

* `MPU6050 VCC` -> `ESP32 3.3V` (or 5V if your module supports it; prefer 3.3V)
* `MPU6050 GND` -> `ESP32 GND`
* `MPU6050 SDA` -> `ESP32 GPIO21` (SDA)
* `MPU6050 SCL` -> `ESP32 GPIO22` (SCL)

> Note: the Arduino sketch uses `Wire.begin(21, 22);` so use those pins or change the sketch to match your wiring.

---

## How it works (high level)

1. **ESP32 (firmware)** reads the MPU6050 gyro values, applies a moving-average filter and deadzone, then prints `MOVE <x> <y>` lines to the serial port when movement is detected.
2. **PC (Python client)** listens to the serial port, parses `MOVE` lines, scales them by a sensitivity factor, and calls the OS mouse API via `pyautogui` to move the cursor relative to its current position.

This split keeps microcontroller code minimal and lets the PC handle platform-specific input control.

---

## Files in this repo

* `arduino/gyrodrive.ino` — ESP32 sketch (MPU6050 reader, prints `MOVE x y`).
* `python/mpu_mouse.py` — Python client that reads serial and moves the mouse.
* `README.md` — (this file)

---

## Arduino (ESP32) — Quick Install & Notes

1. Open `arduino/gyrodrive.ino` in Arduino IDE or PlatformIO.
2. Make sure you have the `MPU6050_light` library installed (or change to your preferred MPU6050 library).
3. Confirm `Wire.begin(21, 22);` if your SDA/SCL pins are `21/22`. Modify if using different pins.
4. Upload to your ESP32.

**Behavior:** on startup the sketch prints initialization messages, runs `mpu.calcOffsets()` for a quick calibration (keep the board still), and then starts streaming `MOVE x y` at ~50Hz when movement is present.

---

## Python client — Install & Usage

1. Create a Python 3 environment (recommended):

   ```bash
   python -m venv venv
   source venv/bin/activate   # on Windows: venv\Scripts\activate
   ```
2. Install dependencies:

   ```bash
   pip install pyserial pyautogui
   ```
3. Edit `python/mpu_mouse.py` and set the `PORT` variable to your ESP32 serial port (e.g. `COM3` on Windows or `/dev/ttyUSB0` on Linux).
4. Run the client:

   ```bash
   python python/mpu_mouse.py
   ```

**Settings inside the client:**

* `SPEED` - multiplier for sensitivity (smaller -> slower pointer)
* `pyautogui.FAILSAFE` - can be toggled if you need screen-corner fail-safe

---

## Tuning & Calibration

* **Calibration:** Keep the ESP32 still during the Arduino startup so `mpu.calcOffsets()` measures offsets correctly.
* **Deadzone:** The sketch uses a deadzone of `5`; increase if you see jitter when the board is stationary.
* **Smoothing:** The moving-average filter uses a 4-sample blend `(prev*3 + new)/4`. Tweak weights for smoother or more responsive motion.
* **Sensitivity:** Use the Python `SPEED` multiplier to scale the raw `MOVE` numbers to comfortable cursor speeds.

---

## Troubleshooting

* **Nothing moves:** Check serial port, verify Arduino prints `MOVE` lines (open serial monitor at 115200). Ensure Python `PORT` matches.
* **Cursor too jittery:** Increase deadzone in sketch or increase smoothing weights.
* **Cursor too slow/fast:** Adjust `SPEED` in Python client.
* **Permissions on Linux:** You may need to add your user to `dialout` or use `sudo` to access `/dev/ttyUSB0`.

---

## Safety & Notes

* This project directly moves your system cursor — take care when testing (don't set extremely high sensitivity).
* On macOS, `pyautogui` may require Accessibility permissions to control the mouse; grant them in System Preferences.

---

## Ideas & Next Steps

* Add buttons on the ESP32 to send `CLICK` commands that map to left/right clicks.
* Implement gesture detection (e.g., quick tilt = click, shake = middle click).
* Convert the Python client into a background service or platform-specific driver for lower latency.
* Add calibration UI to the Python client to adjust offsets and sensitivity interactively.

---

## License

MIT — feel free to hack, fork, and improve. Attribution appreciated.

---

## Contributing

Send PRs or open issues with improvements, bug reports, or feature requests.

---

*Made with a little bit of science and a lot of curiosity.*
