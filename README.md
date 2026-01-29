\# Embedded System Final Project – Joystick-Controlled Car (HCS12)

\## 📌 Project Overview

This project is a small robotic car controlled using a joystick.

The joystick is connected to an \*\*Arduino Uno\*\*, which sends control data to an \*\*HCS12 microcontroller\*\* using \*\*asynchronous UART serial communication\*\*.

The \*\*HCS12\*\* is responsible for controlling two DC motors through an \*\*L298N H-bridge\*\*, managing both \*\*direction\*\* and \*\*speed\*\* using \*\*GPIO\*\* and \*\*PWM\*\*.

---

\## 👥 Team Members

\- \*\*Dana Al-Mahrouk\*\* – HCS12 Firmware (Embedded C)

\- \*\*Rama Almanasreh\*\* – Hardware Connections \& Assembly

\- \*\*Dana Twal\*\* – MATLAB/Simulink + Joystick Interface

---

\## 🚗 Mechanical Design

The car uses a \*\*3-wheel structure\*\*:

\- \*\*Right wheel\*\* → Motor 1

\- \*\*Left wheel\*\* → Motor 2

\- \*\*Front wheel\*\* → Free wheel (support only)

The car moves using \*\*differential drive\*\*:

\- Both motors forward → move forward

\- Both motors backward → move backward

\- Different speeds → turn left or right

📷 \*\*Car Hardware Setup\*\*

!\[Car Setup](images/car_setup.jpeg)

---

\## 🔌 Electrical Design

\### Main Components

\- Arduino Uno

\- HCS12 microcontroller

\- L298N H-Bridge motor driver

\- 2 DC motors

\- Joystick module (Funduino)

\- Battery pack (motor power)

\- External battery for HCS12 power

📷 \*\*L298N Motor Driver\*\*

!\[L298N Pinout](images/l298n_pinout.jpg)

📷 \*\*Joystick Module\*\*

!\[Joystick Module](images/joystick_module.png)

---

\## 📡 Communication Protocol (Arduino → HCS12)

The Arduino transmits joystick commands \*\*byte-by-byte\*\* to the HCS12 using UART.

\### Byte 1: Forward / Backward Control

\- \*\*MSB = 0\*\*

\- Remaining 7 bits: speed and direction

| Range | Meaning |

|------|---------|

| 0–60 | Backward |

| 61–67 | Stop (dead zone) |

| 68–127 | Forward |

---

\### Byte 2: Left / Right Steering Control

\- \*\*MSB = 1\*\*

\- Remaining 7 bits: steering intensity

| Range | Meaning |

|------|---------|

| 0–60 | Turn Right |

| 61–67 | Straight |

| 68–127 | Turn Left |

---

\## ⚙️ HCS12 Motor Control

\### PWM (Speed Control)

\- \*\*PP0 → Motor 1 (Right Motor Enable)\*\*

\- \*\*PP1 → Motor 2 (Left Motor Enable)\*\*

\### GPIO (Direction Control) – Port B

\- \*\*Motor 1 direction:\*\* PB0, PB1

\- \*\*Motor 2 direction:\*\* PB2, PB3

The HCS12 firmware sets direction pins and updates PWM duty cycles to achieve movement.

---

\## 💻 Software Design

\### Arduino Side

\- Reads joystick analog values (10-bit ADC)

\- Scales values to 8-bit format

\- Sends two bytes continuously via UART

\### HCS12 Side

\- Receives bytes through SCI1 UART

\- Detects byte type using MSB

\- Maps joystick readings to:

&nbsp; - Motor direction (GPIO)

&nbsp; - Motor speed (PWM)

\- Implements differential drive turning logic

---

\## 🧪 MATLAB/Simulink (Joystick Scaling)

MATLAB/Simulink was used to monitor and scale joystick values before transmission.

📷 \*\*Simulink Model\*\*

!\[Simulink Model](images/simulink_model.png)

---

\## ⚠️ Problems \& Recommendations

\### Issues

\- UART data was sent \*\*byte-by-byte\*\*, and sometimes the Arduino did not send the correct bytes or the correct order.

\- This caused incorrect steering or speed values at the HCS12.

\### Recommendations

\- Send data in a structured frame:

&nbsp; - Start byte + Byte1 + Byte2

\- Add error checking:

&nbsp; - checksum or repeated frames

\- Use a fixed transmission rate and verify UART settings

---

\## ✅ Conclusion

This project successfully demonstrated joystick-based robotic car control using an Arduino and an HCS12 microcontroller. The system combined UART communication, PWM motor control, GPIO direction control, and differential drive logic to achieve smooth and responsive movement.
