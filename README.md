# Servo-Tester
**How to test mini and medium servos before use them**

![Servo Tseter](https://github.com/user-attachments/assets/84dcd452-6d03-485d-880f-48bf342956b9)

This project can help a R/C modeller to check and select the best servos for his model testing it's power consumption and work before mounting it into a model or a robot.
To do this we will use a power source to provide about 6.1, 5.4 or 4.7Vcc an ammeter module **INA219** and a TFT color display (128x160px) connected to an **Arduino**.

A potentiometer, an external power supply monitoring (in the range **9-12Vcc**) and some buttons complete the device.

The servo movement under test can be MANUAL or AUTOMATIC and an additional function allows you to estimate the overall consumption of the on-board system (excluding the consumption of any electric engine).

At the moment (due to power consumption) **this device can only be used for SMALL and MEDIUM sized servos** because **the upper limit is about 1A** and in any case:  
**----> USE THIS PROJECT AT YOUR OWN RISK <----**

## How it works
Here's how the system works:

- at the start it asks to choose the output voltage to test the servo (4.7, 5.4 or 6.1V)
- then it prints a menu on the TFT where you can choose AUTO or MANUAL mode 
- in **MANUAL** mode the servo is moved by the potentiometer and the moving range can be adjusted by 2 buttons from 100%=1000uSec (max pulse width) to 200%=2000uSec  
- in **AUTO** mode the device moves 10 times forward and backward the servo measuring the power consumption for each movement and at the end it shows the
  - IDLE Ampere mean value
  - RUN Ampere mean value
  - Watt mean value 
- there is also a third menu item: the **ON BOARD SYSTEM TEST** (see below)

### Manual
In this mode you can test the fluidity of the movement during slow movements looking also at every strange noise. A good servo should move in a fluid way and fairly quietly.
### Automatic
The AUTO mode can be used to compare the power consumption of a servo with it's electrical specifications or to the power consumption of another one of the same size (for example 2 mini-servos) and kind (for example 2 digital servos).\
For example let say that a servo during the test consumes 800mW and another similar servo consumes only 400mW. It is worth investigating the first servo, perhaps making further comparisons with other servos. In the same way a **Iidle** value too high should be investigated.\
The graphic bars show the current of every movement (10 forward and 10 backward) and their heights are proportional to the current drain during each movement in the range 0-1000mA. If the servo is good, **they should have similar heights unless you make it strain mechanically by slowing down the movement with your hands**.\
Power consumption is calculated as **Wrun=Irun*Vout**
### On board system test
Before doing this test, **make sure you have disconnected any ELECTRIC ENGINE or other devices with high current consumption from the on-board system** keeping in mind that **1A is the maximum output current**.  
This test can help you to choose the right on-board battery (Wh) **because you will get the energy supplied in 1 minute** to your on-board system **when your servos keep moving continuously** without mechanical strain.   
To run the test you should use the dedicated function on your TX radio that moves every servo (sequentlially or altogether) or you can manually **move them continuosly for 60 seconds** using the TX. In this way you can test the whole power consumption of your on-board electronics (RX + all servos)

**But is this realistic?**  
**Maybe NOT** because in real life probably your servos sometimes can make some strain (increasing consumption), other times they can sleep for a good time percentage without moving at all and sometimes you move more than one servo together so the real power consumption could be different from this value.

Anyway this test can be useful if you compare some previous and well working on-board electronics with a known battery with a new one to determine the battery for the new on-board system.

## Block diagram
The project is divided in the following modules:
- POWER: using a 7806 voltage regulator and some diodes, converts the Vext (9-12V) in 3 values 6.1, 5.4 and 4.7V  
- INA219: is the ammeter module connected via I2C bus to Arduino
- ARDUINO: even if i used a NANO, **you can use any kind of Arduino board**
- LCD TFT: the lcd is a SPI TFT ST7735 128x160 with an SD schede that you can optionally use for your purpose 
![The architecture](https://github.com/user-attachments/assets/91aa9456-01fd-4069-8024-b8c7c6fb9855)

## What you need to build the project
Here are the main components of the project
![Main components](https://github.com/user-attachments/assets/7374b642-2551-4e90-92f0-ce68356fadef)  
You can buy them here:  
Arduino Nano: https://it.aliexpress.com/item/1005007536628798.html  
INA218 module: https://it.aliexpress.com/item/1005008949074776.html  
LCD TFT SPI 128x160px 1.8" with SD: https://it.aliexpress.com/item/1005004540472656.htm  
7806 Voltage Regulator: https://it.aliexpress.com/item/32948107192.html  






...under construction...
