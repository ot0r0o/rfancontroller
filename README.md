# RFanController

RFanController is an **ESP32/Arduino-compatible** project for controlling **433.92 MHz RF ceiling fans** by transmitting pre-captured raw RF signals. Unlike RCSwitch, it **mimics the original signal** for full compatibility. It supports **WiFi & MQTT**, and can be adapted for other RF devices. 

## Features
- **433.92 MHz RF transmission** using **ASK** raw signal replay  
- **Compatible with ESP32 & Arduino**  
- **WiFi & MQTT support** to integrate it with Home Assistant (credentials stored in `secrets.h`)  
- **Custom TX library** for accurate signal transmission
- **Easily adaptable for other RF devices**  

## Hardware Requirements
- **ESP32/Arduino-compatible board**
- **433.92 MHz RF Transmitter** ([WL102-341](https://es.aliexpress.com/item/1005003436580019.html))
- **433.92 MHz RF Receiver** (for initial signal capture - in the same Aliexpress pack)

## Capturing RF Signals
To capture the signals of the remote of my ceilling fan I used the [Controlling my ceiling fan from Home Assistant with 433Mhz RF Modules](https://blog.yonatan.dev/controlling-ceiling-fan-home-assistant-433mhz-rf/) blog post. Kudos to [codechimp](https://github.com/cowchimp).
This project **does not include signal capture tools**, but you can use [RCSwitch's ReceiveDemo_Advanced](https://github.com/sui77/rc-switch/blob/master/examples/ReceiveDemo_Advanced/ReceiveDemo_Advanced.ino) to record raw RF data.

## Why I use Raw RF?
Standard RF libraries like RCSwitch were **not fully compatible** with my ceiling fan. Instead, the remote signals were recorded to extract the **raw signal timing** and **replay it precisely**, ensuring proper functionality.

## How It Works
1. **Capture the raw RF signal** using the RCSwitch tool. 
2. **Store the raw pulse sequence** in the `RFanController.h` file.  
3. **Transmit the signal** using the custom TX library.  

## Raw signals
The captured raw signals aren't uploaded to the repository for security reassons. You just need to capture each remote command and modify the `RFanController.h` to add your commands.
