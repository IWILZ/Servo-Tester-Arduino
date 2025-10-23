# Servo-Tester
**How to test mini and medium servos before use them**

![IMG_20250406_175301](https://github.com/user-attachments/assets/84dcd452-6d03-485d-880f-48bf342956b9)

This project can help a R/C modeller to check and select the best servos for his model testing it's power consumption and work before mounting it into a model or a robot.
To do this we will use a power source to provide about 6, 5.3 or 4.6Vcc an ammeter module **INA219** and a TFT color display (128x160px) connected to an **Arduino**.

A potentiometer, an external power supply monitoring (in the range 9-12Vcc) and some buttons complete the device.

The servo movement under test can be MANUAL or AUTOMATIC and an additional function allows you to estimate the overall consumption of the on-board system (excluding the consumption of any electric engine).

At the moment (due to power consumption) **this device can only be used for SMALL and MEDIUM sized servos** and in any case:  
**----> USE THIS PROJECT AT YOUR OWN RISK <----**

## How it works
Here's how the system works:

- at the start it asks to choose the output voltage to test the servo
- then it print a menu on the TFT where you can choose AUTO or MANUAL mode 
- in MANUAL mode the servo is moved by the potentiometer and the moving range can be adjusted by 2 buttons from 100%=1000uSec (max pulse width) to 200%=2000uSec  
- In this mode you can test the smooth and slow movement of the servo looking also at every strange noise  
- in AUTO mode the device moves 10 times forward and backward the servo measuring the power consumption for each movement and at the end it shows the
  - IDLE Ampere mean value
  - RUN Ampere mean value
  - Watt mean value
- 
 
  






...under construction...
