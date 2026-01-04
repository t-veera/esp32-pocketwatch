P32 Pocketwatch Firmware

Custom smartwatch firmware for the Waveshare ESP32-S3-Touch-AMOLED-2.06 development board, featuring real-time clock functionality and a clean analog watchface.

## Project Goals

### Completed ✅
- [x] Real-time clock display with analog hands
- [x] RTC (PCF85063) integration for accurate timekeeping
- [x] Auto-updating clock (hands move every second)
- [x] Digital time display (HH:MM format)
- [x] Date display with day of week
- [x] Time persists through power cycles (RTC keeps running)
- [x] Based on esp-brookesia Phone system framework

### In Progress 🔄
- [ ] Button (GPIO0) press to sleep/wake display
- [ ] Auto-launch watchface on boot (skip app launcher)
- [ ] Fix system status bar time to read from RTC

### Planned 📋
- [ ] Bluetooth connectivity for phone notifications
- [ ] WiFi integration for NTP time synchronization
- [ ] Weather display
- [ ] Step counter using onboard IMU (QMI8658)
- [ ] Multiple watch apps:
  - [ ] Snapshot (CSV data logging)
  - [ ] Todo list with voice dictation
  - [ ] Location saving (GPS integration)
  - [ ] Torch control
  - [ ] Distance measurement
  - [ ] Stopwatch
  - [ ] Timer
  - [ ] Find my phone
  - [ ] Bluetooth device management
  - [ ] Alarm functionality

## Hardware

- **Board:** Waveshare ESP32-S3-Touch-AMOLED-2.06
- **Display:** 2.06" AMOLED touchscreen (416x466)
- **RTC:** PCF85063 (I2C)
- **IMU:** QMI8658 6-axis
- **Audio:** ES8311 codec with speaker
- **Storage:** MicroSD card slot
- **Button:** BOOT button (GPIO0)

## Quick Flash (No Build Required)

If you just want to flash the pre-built firmware without setting up the development environment:

### Requirements
- Python 3.6 or newer
- USB cable to connect the watch

### Steps

1. **Install esptool:**
   ```bash
   pip install esptool
   ```

2. **Download firmware:**
   ```bash
   git clone https://github.com/t-veera/esp32-pocketwatch.git
   cd esp32-pocketwatch/releases
   ```

3. **Connect your watch via USB** and find the port:
   - **Linux:** Usually `/dev/ttyACM0` or `/dev/ttyUSB0`
   - **macOS:** Usually `/dev/cu.usbserial-*`
   - **Windows:** Usually `COM3` or higher

4. **Flash the firmware:**
   ```bash
   # Linux/macOS
   esptool.py --chip esp32s3 -p /dev/ttyACM0 write_flash \
     0x0 bootloader.bin \
     0x8000 partition-table.bin \
     0x10000 pocketwatch-v0.1.bin

   # Windows
   esptool.py --chip esp32s3 -p COM3 write_flash \
     0x0 bootloader.bin \
     0x8000 partition-table.bin \
     0x10000 pocketwatch-v0.1.bin
   ```

5. **Done!** The watch will reboot and show the clock.

## Development Setup

### Requirements
- ESP-IDF v5.5.1 or newer
- Git
- Python 3.8+

### Clone and Build

1. **Install ESP-IDF:**
   Follow the official guide: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/

2. **Clone this repository:**
   ```bash
   git clone https://github.com/t-veera/esp32-pocketwatch.git
   cd esp32-pocketwatch
   ```

3. **Activate ESP-IDF environment:**
   ```bash
   # Linux/macOS (bash/zsh)
   source ~/esp/esp-idf/export.sh
   
   # Fish shell
   source ~/esp/esp-idf/export.fish
   
   # Windows
   %userprofile%\esp\esp-idf\export.bat
   ```

4. **Build the project:**
   ```bash
   idf.py build
   ```

5. **Flash to device:**
   ```bash
   idf.py -p /dev/ttyACM0 flash monitor
   ```

## Setting the Time

On first boot, the RTC time needs to be set. The firmware currently sets it to a default time on startup.

To set the current time:

1. Edit `components/brookesia_app_squareline_demo/esp_brookesia_app_squareline_demo.cpp`
2. Find the time-setting code (around line 59-67)
3. Update to your current time:
   ```cpp
   timeinfo.tm_year = 2025 - 1900;  // Year
   timeinfo.tm_mon = 12 - 1;        // Month (0-11)
   timeinfo.tm_mday = 12;           // Day
   timeinfo.tm_hour = 14;           // Hour (24h format)
   timeinfo.tm_min = 30;            // Minute
   timeinfo.tm_sec = 0;             // Second
   ```
4. Flash once to set the time
5. Comment out those lines (add `//` in front) and flash again
6. The RTC will now keep accurate time even when powered off

## Project Structure

```
esp32-pocketwatch/
├── components/
│   ├── brookesia_app_squareline_demo/  # Main clock app
│   │   ├── esp_brookesia_app_squareline_demo.cpp
│   │   ├── esp_brookesia_app_squareline_demo.hpp
│   │   └── ui/                          # SquareLine Studio UI files
│   ├── brookesia_core/                  # Phone system framework
│   └── sensorlib/                       # RTC and sensor drivers
├── main/
│   └── main.cpp                         # App entry point
├── releases/                            # Pre-built firmware binaries
├── CMakeLists.txt
├── sdkconfig.defaults
└── partitions.csv
```

## Key Features

- **Framework:** esp-brookesia Phone system for UI management
- **Graphics:** LVGL 9.2 for smooth UI rendering
- **Time Accuracy:** Hardware RTC keeps time even when powered off
- **Updates:** Clock updates every second from RTC
- **Display:** Analog clock hands + digital time + date

## Contributing

Feel free to open issues or submit pull requests for improvements!

## License

This project builds upon Waveshare's ESP32-S3-Touch-AMOLED examples and the esp-brookesia framework.

## Acknowledgments

- Waveshare for the hardware and BSP
- Espressif for ESP-IDF and esp-brookesia framework
- lewishe for the SensorLib library
