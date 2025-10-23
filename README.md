# Servo-Tester
**How to test mini and medium servos before use them**

![IMG_20250406_175301](https://github.com/user-attachments/assets/84dcd452-6d03-485d-880f-48bf342956b9)

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
In this mode you can test the smooth and slow movement of the servo looking also at every strange noise 
### Automatic
The AUTO mode can be used to compare the power consumption of a servo with it's electrical specifications or to the power consumption of another one of the same size (for example 2 mini-servos) and kind (for example 2 digital servos).\
For example let say that a servo during the test consumes 800mW and another similar servo consumes only 400mW. It is worth investigating the first servo, perhaps making further comparisons with other servos.\
In the same way a **Iidle** value too high should be investigated.\
The graphic bars show the current of every movement (10 forward and 10 backward) and their heights are proportional to the current drain during each movement in the range 0-1000mA. If the servo is good, **they should have similar heights unless you make it strain mechanically by slowing down the movement with your hands**.\
Power consumption is calculated as **Wrun=Irun*Vout**
### On borad system test
To test the total power consumption (**EXCLUDING ANY ELECTRIC ENGINE**) you should use the dedicated function on your TX radio that moves every servo (sequentlially or altogether) or you can move them manually continuosly for 60 seconds using the TX. In this way you can test the whole power consumption of your on-board electronics (RX + all servos) keeping in mind that **1A is the maximum output current** and this information is usefull to choose the right on-board battery capacity (mAh)
 
  






...under construction...
