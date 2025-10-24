# Servo-Tester
**How to test mini and medium servos before use them**

![Servo Tseter](https://github.com/user-attachments/assets/84dcd452-6d03-485d-880f-48bf342956b9)

This project can help a R/C modeller to check and select the best servos for his model testing it's power consumption and work before mounting it into a model or a robot.
To do this we will use a power source to provide about 6.1, 5.4 or 4.7Vcc an ammeter module **INA219** and a TFT color display (128x160px) connected to an **Arduino**.

A potentiometer, an external power supply monitoring (in the range **9-12Vcc**) and some buttons complete the device.

The servo movement under test can be MANUAL or AUTOMATIC and an additional function allows you to estimate the overall consumption of the on-board system (excluding the consumption of any electric engine).

At the moment (due to power consumption) **this device can only be used for SMALL and MEDIUM sized servos** because **the upper limit is about 1A** and in any case:  

> ***----> USE THIS PROJECT AT YOUR OWN RISK <----***

## How it works
Here's how the system works:

- at the start it asks to choose the output voltage to test the servo (4.7, 5.4 or 6.1V)[^1]
- then it prints a menu on the TFT where you can choose AUTO or MANUAL mode 
- in **MANUAL** mode the servo is moved by the potentiometer and the moving range can be adjusted by 2 buttons from 100%=1000uSec (max pulse width) to 200%=2000uSec  
- in **AUTO** mode the device moves 10 times forward and backward the servo measuring the power consumption for each movement and at the end it shows the
  - IDLE Ampere mean value
  - RUN Ampere mean value
  - Watt mean value 
- there is also a third menu item: the **ON BOARD SYSTEM TEST** (see below)

### Manual
In this mode you can test the fluidity of the movement during slow movements looking also at every strange noise. **A good servo should move in a fluid way and fairly quietly**.
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
**Maybe NOT** because in real life probably your servos sometimes can make some strain (increasing consumption), other times they can sleep for a huge time percentage without moving at all and sometimes you move more than one servo together so the real power consumption could be different from this value.

Anyway this test can be useful if you compare some previous and well working on-board electronics powered with a known battery with a new one to determine the rigth battery for the new on-board system.

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
LCD TFT SPI 128x160px 1.8" with SD: https://it.aliexpress.com/item/1005004540472656.html  
7806 Voltage Regulator: https://it.aliexpress.com/item/32948107192.html  

In addition you also need a **3-position switch, some resistors, capacitors, some connectors and 3 push-buttons** as you can see in the schematic of this project.

## The MAIN schematic and PCB
As you can see, the schematic attached to this project is quite simple.  
For the **power stage**, the external source (9-12Vcc) is connected directly to the 7806 input and to the Arduino's Vin. The output voltage of the 7806 (6.1V) is then reduced to 5.4 by a single diode and to 4.7 by 2 diodes. These 3 voltages go to the 3 position switch (trough JP2 connector) and the switch output is connected to the Vin+ pin of the INA219 module.

The Vin- pin of the INA219 is used as servo power source while the PWM control signal is taken from Arduino's D9 pin.   
The center of the potentiometer is connected to the A0 pin acting as an ADC.  
The 3 push buttons are connected to A1, A2 and A3 as digital inputs with pull-up resistor (see program listing).  
The R1 and R2 resistors are used to generate the voltage that the program uses to evaluate the external power supply.

For the prototype i've used two pre-drilled boards wiring every connection with a soldering iron: 
- the **main board** contains the power module, Arduino, and some in-line connectors[^2] at 2.54mm pin spacing connecting INA219, and the second board
- the **second board** contains the LCD, 3 buttons, the 3-position switch and the potentiometer

![The prototype](https://github.com/user-attachments/assets/581437dc-fc80-4f94-824b-86a7c32a3380)

As you can see at the low-left in the picture above, i've also found an old buzzer so i decided to use it :blush:  

In the last few days **i designed the main board PCB using EasyEDA Standard** and as soon as possible i will replace the "wired" one with the new PCB but you can already find it attached to this project together with the related **gerber files**.

## Schematic and PCB of the LCD & Buttons board
The secondary board contains the LCD and the 3 push-buttons and you can find the schematic, the PCB draw and the GERBER file on the file list of this project but consider that i haven't tested them yet.

## The sketch
Once the project is complete, you can compile and upload the program **Servo_Tester_eng _4.0.ino** using for example the **Arduino IDE** and then you need to do a simple tuning.
In fact the program checks the V external value but this depends on the tollerance of R1 and R2 values so you need to trim it at line 80 of the sketch.
```
// Trim the following parameter to match the right Vexternal (Vin) value
#define   VEXT_DIV        64.67   // 
#define   MIN_VIN         8.0     // Minimum Vin voltage
```
To do this you have to use a tester on the external source and slowly increase or decrease the **VEXT_DIV** recompiling each time, until the Vin shown on the LCD is correct.  
As you can see, you can also refine the MIN_VIN but don't set it below 8.0V (which is the minimum external voltage below which the Servo Tester signals an audio alarm) because there is a voltage drop across the 7806.

The "core" of this project is the **INA219** module with the library **Adafruit_INA219.h** but during the develop **i had to modify a line in that library to match my needs** (see below).  
The reason is that in default mode the INA219 uses a sort of continuous loop reading a set of current samples at the end of which it calculates the average value, but in this way this value can be influenced by the fact that the servo may have stopped during the sampling or may instead have moved continuously producing 2 very different values.  
In my project instead, **i need to start the sampling only when i want it to** (i.e. when the servo starts moving) producing consistent values (**see the call to StartInaSampling() inside the ServoRun() function**).  

### Modified version of Adafruit_INA219.cpp 
As explained above i had to modify the library to allow to start a single sampling collection and this was done setting a **"triggered" mode**[^3].  

To do this you have to:
1. search into your library directory and edit **Adafruit_INA219.cpp** with a text editor
2. search the string **Adafruit_INA219::setCalibration_32V_2A()**
3. scroll down until you find 
```
uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV | INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
```
4. substitute the previous line with 
```
uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV | INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_128S_69MS |
                    INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED;
```
In this way the program will use a **"triggered" sampling at 12bit/sample collecting 128 samples in about 69mSec** :exclamation::exclamation::exclamation:

## Using a LiPo battery for power supply
As you see the Servo Tester needs something like a 9-12Vcc of external power to work. This can be done using a small wall power supply or a battery, and in this case **a small 3S LiPo** can be a good choice, and even better if connected to a **step-up charging module** as in the following picture.
![Battery](https://github.com/user-attachments/assets/a99dad17-4431-4248-a1f5-30a6f1a0b07e)  
So for this purpose you could buy something like these: 
- Step-up charger: https://it.aliexpress.com/item/1005007214123033.html
- LiPo 3S 650mAh: https://www.amazon.it/dp/B08535D7H4

## Conclusions
I hope this project can be nice and useful both for a model-maker or a robotic enthusiast, but maybe can be nice also to try some modules like INA219, LCD TFT SPI 128x160px 1.8" with SD and of course Arduino.  
It's also quite cheap because if you already have some resistors, buttons and a battery in your drawer, **the total cost of the components should be less than 15-20$**  
> ***HAVE FUN!!!***

[^1]: Be careful when using 6.1V because some servos could be damaged by voltages higher than 5V (refer to the servo data sheet)
[^2]: To be sure about the right direction of the connectors on the board, i introduced some reference pins in the connectors (each green cros in the schematic) to ensure that the connection between male and female can only occur in the correct direction.
[^3]: See also https://electronics.stackexchange.com/questions/515455/ina219-current-sensor-not-working-properly-with-dc-motor 

