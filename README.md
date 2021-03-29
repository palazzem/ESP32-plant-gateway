# ESP32 MiFlora Gateway

ESP32 BLE client for MiFlora compatible sensors. ESP32 metrics and MiFlora
sensor measurements are pushed through MQTT.

## Requirements

- A compatible ESP32 board
- A compatible MiFlora sensor
- MQTT server with authentication enabled. [Mosquitto container][1] is a good start
- (optional) A home automation software such as [Home Assistant][2]

[1]: https://hub.docker.com/_/eclipse-mosquitto
[2]: https://www.home-assistant.io/

## Compatible Boards

This sketch has been tested against the following boards:
- Adafruit HUZZAH32 – Feather Board
- ESP-WROOM-32 - Development Board

## Supported MiFlora Sensors

This sketch has been tested with the following MiFlora devices:
- VegTrug Flower Care Soil Tester
- WANFEI Flower Care Soil Tester

## Setup

1. Edit settings in config.h:
- FLORA_DEVICES - MAC address(es) of your MiFlora sensor(s)
- SLEEP_DURATION - sleep duration between sensor reads
- EMERGENCY_HIBERNATE - emergency hibernate countdown when something gets stuck
- BATTERY_INTERVAL - Mi Flora battery status read interval
- RETRY - device retry count when something fails
- WIFI_SSID - WLAN SSID
- WIFI_PASSWORD - WLAN password
- ADC_Pin - ESP32 ADC pin number where the Battery is connected to
- MQTT Server settings

2. Open sketch in Arduino, compile & upload.

## (optional) Home Assistant setup

TBD.

## Credits

- Original fork: https://github.com/e6on/ESP32_MiFlora_MQTT
