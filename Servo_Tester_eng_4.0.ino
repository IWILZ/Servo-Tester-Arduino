/*
  ****************** Servo Tester V4.0 ********************
  This project can help a R/C modeller to check and select 
  the best servos for his model testing it's power consumption
  and work before mounting it into a model or a robot.

  To do this we will use a power source to provide about 6, 
  5.3 or 4.6Vcc an ammeter module INA219 and a TFT color 
  display (128x160px) connected to an Arduino.

  A potentiometer, an external power supply monitoring 
  (in the range 9-12Vcc) and some buttons complete the device.

  The servo movement under test can be MANUAL or AUTOMATIC and 
  an additional function allows you to estimate the overall 
  consumption of the on-board system (excluding the consumption 
  of any electric engine).

  At the moment (due to power consumption) this device can only 
  be used for small and medium-sized servos and in any case: 

      ******* USE THIS PROJECT AT YOUR OWN RISK ******* 

 * ---------------------------------------------------------------
 * ------------------------- CONNECTIONS -------------------------
 * ---------------------------------------------------------------
 * PIN Arduino NANO   
 * -------------------------------
 * Vin                8...12Vcc
 * A0                 potentiometer
 * A7                 Vexternal 
 * A3                 Button UP/LEFT
 * A2                 Button ENTER/ESCAPE
 * A1                 Button DOWN/RIGHT
 * D2                 Buzzer
 * D9                 Servo signal
 * D10                Debug HW
 * GND                Ground
 *
 * -------------------------------------------
 * Display TFT ST7735 128x160 connections
 * -------------------------------------------
 * Arduino			Display 
 * 5V           VCC
 * GND          GND
 * D6           CS
 * D5           RESET
 * D4           A0
 * D11          SDA   (MOSI)
 * D13          SCK
 * 3V3          LED
 *
 * -------------------------------------------
 * Module INA219 connections
 * -------------------------------------------
 * Arduino			INA219 
 * 5V           VCC
 * GND          GND
 * A4           SDA
 * A5           SCL
 *    ...and on INA219...
 *   Vin+ from power module
 *   Vin- to servo and/or RX Vcc pin
 */
#include <Servo.h>
#include <Adafruit_GFX.h>     // Core graphics library for TFT display
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>              // SPI bus library for TFT ST7735 display
#include <Wire.h>

// WARNING!!! -> modified library to match my needs 
// (see: "INA219 - Sensore di corrente con I2C.docx")
#include <Adafruit_INA219.h>

// ********************** Defines ***************************
#define   PGM_NAME        "Servo Tester"
#define   PGM_VERSION     "V 4.0"

// Trim the following parameter to match the right Vexternal (Vin) value
#define   VEXT_DIV        64.67   // 
#define   MIN_VIN         8.0     // Minimum Vin voltage

#define   PIN_RVAR        A0      // Central potentiometer pin
#define   PIN_VEXT        A7      // Vexternal (Vin)

#define   PIN_SERVO       9       // Servo control pin
#define   SERVO_CENTER    1500    // Servo central position (microsec)
#define   SERVO_MIN       1000    // standard servo min pulse value (microsec)
#define   SERVO_MAX       2000    // standard servo max pulse value (microsec)

#define   TFT_CS          6
#define   TFT_RST         5       // Or set to -1 and connect to Arduino RESET pin
#define   TFT_A0          4
#define   TFT_WIDTH       160     // TFT display resolution is...
#define   TFT_HEIGHT      128     // ...160x128 pixel (horizontal)
#define   TFT_CH_WIDTH    6       // Width of a standard character
#define   TFT_MYGREEN     0X0300  // Dark green
#define   TFT_MYRED       0XFC00  // Light red
#define   CHAR_WIDTH_1    6       // horizontal pixels of character size 1
#define   CHAR_WIDTH_2    12      // horizontal pixels of character size 2
#define   ASCII_MICRO     230     // ASCII code for "µ" symbol

#define   PIN_BTN_UP      A3      // UP/LEFT button
#define   PIN_BTN_ENTER   A2      // ENTER button (ESCAPE with long press)
#define   PIN_BTN_DOWN    A1      // DOWN/RIGHT button
#define   TIME_PRESS      80      // Min time for a valid button press (msec)
#define   TIME_LONGPRESS  500     // Min time for a valid long press (msec)
#define   BTN_NOPRESS     0       // 
#define   BTN_PRESS       1       // 3 status of a button -> see ButtonActive()
#define   BTN_LONGPRESS   2       //

// ----------- Menu defines --------------
#define   TEST_MANUAL     0
#define   TEST_AUTO       1
#define   TEST_ONBOARD         2
#define   N_AUTO_REPEAT   10      // N. on cycles for auto test

#define   PIN_BUZZER      2       // 
#define   PIN_DEBUG       10      // For debugging with oscilloscope

// ********************** Global Vars ***************************
char  Menu[3][25]={"1. Manual servo test", 
                   "2. Auto servo test", 
                   "3. On-board system test"};
float InaShuntVoltage = 0;
float InaBusVoltage = 0;
float InaCurrent_mA = 0;
float InaLoadVoltage = 0;
float InaPower_mW = 0;
float PrevInaLoadVoltage=0.0;
int   PrevVext=0;

// Array to store I and W vaules produced by an auto test
struct {
  float Current;
  float Power;
} ServoData [N_AUTO_REPEAT*2];

// ******************* Define objects ************************
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_A0, TFT_RST);
Adafruit_INA219 ina219;
Servo           MyServo;

/***********************************************************
 setup()
 ***********************************************************/ 
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n***** Program start *****");

  pinMode(PIN_DEBUG, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_ENTER, INPUT_PULLUP);

  InitTFT();

  // Initialize the INA219 as 32V, 2A and single conversion 
  // triggered by StartInaSampling()
  if (! ina219.begin()) {
    Beep(3);
    Serial.println("Failed to find INA219 module - PROGRAM STOPPING");
    tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
    CenterPrintStr("Module INA219 not found",70,1,false);
    while (true) { delay(10); }     // Program stop
  }
  Serial.println("INA219 Initialized");
  SetServoVoltage();                // Select the right Vout
  MyServo.attach(PIN_SERVO);
  MyServo.writeMicroseconds(SERVO_CENTER);
} // setup()

/***********************************************************
 PrintTitle()
 Clear the LCD and prints the title of the screen
 ***********************************************************/      
void PrintTitle(char *str){
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
  CenterPrintStr(str,10,1,false);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
} // PrintTitle()

/***********************************************************
 PrintDataFirstRaw()
 Prints Vin and Vout values on the top of LCD
 ***********************************************************/    
void PrintDataFirstRaw(){
  PrevInaLoadVoltage=0.0;   // forces writing of Vout value
  PrevVext=0;               // forces writing of Vin value
  PrintStr("Vin=", 10, 25, 1, false);
  PrintVin();  
  PrintStr("Vout=", 95, 25, 1, false);  
  PrintVout();
  tft.drawRect(1,40,TFT_WIDTH-1,TFT_HEIGHT-41,ST77XX_GREEN);
 } // PrintDataFirstRaw()

/***********************************************************
 PrintVout()
 If it's changed, prints Vout value on the top of LCD
 ***********************************************************/    
void PrintVout(){
char buff[10];
  StartInaSampling();               // Start INA219 sampling...
  delay(70);                        // ...and wait 70msec
  GetInaVal();
  if (PrevInaLoadVoltage!=InaLoadVoltage){
    dtostrf(InaLoadVoltage, 4, 2, buff);
    PrintStr(buff, 126, 25, 1, false);
    PrevInaLoadVoltage=InaLoadVoltage;
  }
} // PrintVout()

/***********************************************************
 PrintVin()
 If it's changed, prints Vin value on the top of LCD
 ***********************************************************/    
void PrintVin(){
char buff[10];
int Vext;
  Vext=analogRead(PIN_VEXT);
  if (PrevVext!=Vext){
    dtostrf((float)Vext/VEXT_DIV, 4, 2, buff);
    // Checking minimum Vin voltage
    if ((float)Vext/VEXT_DIV<MIN_VIN){
      Beep(3);
      tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
    }
    PrintStr(buff, 35, 25, 1, false);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    PrevVext=Vext;
  }
} // PrintVin()

/***********************************************************
 OnBoardTest()
 ***********************************************************/      
void OnBoardTest(){
byte secs;
char str[30], buff[10];
float MeanCurrent=0.0;
float MeanPower=0.0;

  CenterPrintStr("Check the Vout voltage,", 45, 1, false);
  CenterPrintStr("connect your RX,", 56, 1, false);
  CenterPrintStr("start moving servos,", 67, 1, false);
  CenterPrintStr("press ENTER to START", 78, 1, false);
  do{} while(ButtonActive(PIN_BTN_ENTER)!=BTN_PRESS);

  // Starting the test...
  for (secs=60; secs>0; secs--){
    sprintf(str, " %02d sec ", secs);
    tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
    CenterPrintStr(str, 93, 1, true);
    StartInaSampling();       // Start INA219 sampling
    delay(1000);              // wait 1 sec
    GetInaVal();
    // update total I and W values
    MeanCurrent=MeanCurrent+InaCurrent_mA;
    MeanPower=MeanPower+InaPower_mW;
  } // for
  
  Beep(1);
  // print I and W mean values
  MeanCurrent=MeanCurrent/60.0;
  dtostrf(MeanCurrent, 3, 2, buff);
  sprintf(str, " I=%smA ", buff);
  CenterPrintStr(str, 93, 1, true);
  MeanPower=MeanPower/60.0;
  dtostrf(MeanPower, 3, 2, buff);
  sprintf(str, " W=%smW ", buff);
  CenterPrintStr(str, 108, 1, true);

  do{} while(ButtonActive(PIN_BTN_ENTER)!=BTN_PRESS);
} // OnBoardTest()

/***********************************************************
 Beep()
 ***********************************************************/      
void Beep(int n){
byte i;
  for (i=0; i<n; i++){
    digitalWrite(PIN_BUZZER, true);
    delay(100);
    digitalWrite(PIN_BUZZER, false);
    delay(100);
  }  
} // Beep()

/***********************************************************
 DrawOnBoardDisplay()
 ***********************************************************/      
void DrawOnBoardDisplay(){
  PrintTitle(" ON-BOARD SYSTEM TEST ");
  PrintDataFirstRaw();
  OnBoardTest();
} // DrawOnBoardDisplay()

/***********************************************************
 DrawAutoDisplay()
 ***********************************************************/      
void DrawAutoDisplay(){
  PrintTitle(" AUTOMATIC SERVO TEST ");
  PrintDataFirstRaw();
  AutoTestServo();  
} // DrawAutoDisplay()

/***********************************************************
 DrawManualDisplay()
 ***********************************************************/      
void DrawManualDisplay(){
  PrintTitle(" MANUAL SERVO TEST ");
  PrintDataFirstRaw();
  ManualTestServo();  
} // DrawManualDisplay()
  
/***********************************************************
 DrawINAbar()
 Draws a vertical colored bar proportional to the current
 drawn by the servo
***********************************************************/
void DrawINAbar(byte n_bar){
unsigned long h_bar;
unsigned int color;

  h_bar=(TFT_HEIGHT-43)*InaCurrent_mA/1000;
  // the color depends on the current drawn
  color=ST77XX_GREEN;
  if (InaCurrent_mA>333.0) color=ST77XX_ORANGE;
  if (InaCurrent_mA>666.0) color=ST77XX_RED;
  tft.fillRect(n_bar*8+1,TFT_HEIGHT-(int)h_bar-2,6,(int)h_bar,color);
} // DrawINAbar()

/***********************************************************
 loop()
 ***********************************************************/
void loop() {
int pot, servo_position; 
char str[50], buff[10];
byte btn_status;

  switch (MainMenu()) {
    case TEST_MANUAL:
      DrawManualDisplay();
      break;
    case TEST_AUTO:
      DrawAutoDisplay();
      break;
    case TEST_ONBOARD:
      DrawOnBoardDisplay();
      break;
  }
} // main()

/***********************************************************
 MainMenu()
 ***********************************************************/
byte MainMenu(){
int i, current=0, prev_current=2;

  tft.fillScreen(ST77XX_BLACK);
  // Print program name on TFT
  tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  CenterPrintStr(PGM_NAME,10,2,false);

  // Print menu voices
  do{
    for (i=0; i<3 && current!=prev_current; i++){
      if (current==i) tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
        else tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      PrintStr(Menu[i], 13, 45+i*18, 1, false);
    }
    prev_current=current;
    if (ButtonActive(PIN_BTN_UP)==BTN_PRESS) current--;
    if (ButtonActive(PIN_BTN_DOWN)==BTN_PRESS) current++;
    if (current>2) current=0;
    if (current<0) current=2;
  } while (ButtonActive(PIN_BTN_ENTER)!=BTN_PRESS);
  return current;
} // MainMenu()

/***********************************************************
 ServoRun()
 Makes one run only in CCW or CW direction.
 At the end of the run, reads values from INA219 and calls 
 DrawINAbar().
 The INA is triggered to start just before each run calling 
 StartInaSampling() and each run lasts about 150msec while 
 the INA collects the data in about 69msec.   
 ***********************************************************/
void ServoRun(byte index, int start, int dir){
byte j;
    StartInaSampling();               // Start INA219 sampling
    // This loop is needed to slow down the servo movement just enough
    for(j=1;j<=10;j++){
      MyServo.writeMicroseconds(start+dir*j*100);
      delay(15);
    } // for
    GetInaVal();
    PrintVout();
    PrintVin();
    SaveData(index);            // save INA data for further compute 
    DrawINAbar(index);
    delay(100);
} // ServoRun()

/***********************************************************
 DrawServoSignal()
 Draws the servo "impulse" shape for manual test
 ***********************************************************/
void DrawServoSignal(){
  tft.drawFastHLine(10,73,14,ST77XX_YELLOW);
  tft.drawFastVLine(23,45,28,ST77XX_YELLOW);
  tft.drawFastHLine(23,45,32,ST77XX_YELLOW);
  tft.drawFastVLine(55,45,28,ST77XX_YELLOW);
  tft.drawFastHLine(55,73,96,ST77XX_YELLOW);
  PrintStr("Position=", 62, 48, 1, false);
  PrintStr("Pulse W.=", 62, 60, 1, false);
} // DrawServoSignal()

/***********************************************************
 DrawEditRange()
 draws the screen edit "zone" for manual test 
 ***********************************************************/
void DrawEditRange(){
  tft.drawFastVLine(30,85,12,ST77XX_GREEN);
  tft.drawFastHLine(30,85,100,ST77XX_GREEN);
  tft.drawFastVLine(130,85,12,ST77XX_GREEN);
  // draw 3 buttons
  PrintStr("-",28,107,1,false);
  PrintStr("+",128,107,1,false);
  tft.drawCircle(30,110,5,ST77XX_GREEN);
  tft.drawCircle(130,110,5,ST77XX_GREEN);

  CenterPrintStr("Exit",98,1,false);
  tft.fillCircle(80,112,5,ST77XX_RED);
} // DrawEditRange()

/***********************************************************
 PrintPulseWidth()
 ***********************************************************/
void PrintPulseWidth(int range){
char str[10];
unsigned long perc;

  // Pulse width value
  sprintf(str, "%d%cs", range, ASCII_MICRO);
  PrintStr(str, 116, 60, 1, false);
  
  // compute % of movement from 100 to 200%
  perc=range*100/SERVO_MIN;
  sprintf(str, "%d%%", (int)perc);
  PrintStr(str, 28, 60, 1, false);
} // PrintPulseWidth()

/***********************************************************
 UpdateServoPosition()
 **********************************************************/
void UpdateServoPosition(int p, int range){
unsigned long micro_sec;
char str[10];
  micro_sec=1500L+range*(p-500L)/1000L;
  MyServo.writeMicroseconds((int)micro_sec);
  // print actual Position value
  sprintf(str, "%4d%cs", (int)micro_sec, ASCII_MICRO);
  // if we are in the middle of servo range
  // print white on red  
  if (micro_sec>1480 && micro_sec<1520)
    tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
  PrintStr(str, 116, 48, 1, false);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
} // UpdateServoPosition()

/***********************************************************
 ManualTestServo()
 Allow to move the servo using a potentiometer starting with
 the standard range of control signal (1000...2000µsec -> 90°) 
 but then the range can be increased until the 200% 
 (500...2500µsec -> 180°)
 ***********************************************************/
void ManualTestServo(){
int pot, range=1000, prev_pot=0;
bool refresh=false;
unsigned long time;

  DrawServoSignal();
  DrawEditRange();
  CenterPrintStr(" Pulse Width ",83,1,true);
  PrintPulseWidth(range); // Print Pulse width value
  // update and print position value  
  pot=GetRvarPosition();
  UpdateServoPosition(pot, range);

  time=millis();
  do{
    pot=GetRvarPosition();
    if (pot<=prev_pot-5 || pot>prev_pot+5){ // just to prevent some flicker
      UpdateServoPosition(pot, range);
      prev_pot=pot;
    }
    // is DOWN/RIGHT button pressed?
    if (ButtonActive(PIN_BTN_DOWN)==BTN_PRESS){
      range=range+50;
      if(range>SERVO_MAX) range=SERVO_MAX; 
      refresh=true;
    } 
    // is UP/LEFT button pressed?
    if (ButtonActive(PIN_BTN_UP)==BTN_PRESS){
      range=range-50;    
      if(range<SERVO_MIN) range=SERVO_MIN;  
      refresh=true;
    } 
    
    // is 1 second last from the previous refresh? 
    if(millis()>=time+1000){
      refresh=true;
    }

    if(refresh){
      PrintVout();
      PrintVin();
      UpdateServoPosition(pot, range);
      PrintPulseWidth(range);
      refresh=false;
      time=millis();
    }
  } while (ButtonActive(PIN_BTN_ENTER)!=BTN_PRESS);
} // ManualTestServo()

/***********************************************************
 AutoTestServo()
 Moves N_AUTO_REPEAT times the servo from 1000 to 2000usec 
 back and forth.
 ***********************************************************/
void AutoTestServo(){
byte i;
  // Starting from SERVO_MIN position
  MyServo.writeMicroseconds(SERVO_MIN);
  delay(150);

  for(i=0;i<N_AUTO_REPEAT;i++){
    ServoRun(i*2, SERVO_MIN, 1);      // CCW
    ServoRun(i*2+1, SERVO_MAX, -1);   // CW
  } 
  MyServo.writeMicroseconds(SERVO_CENTER);
  // wait for ENTER to continue
  do{} while (ButtonActive(PIN_BTN_ENTER)!=BTN_PRESS); 
  ShowMeanValues();
} // AutoTestServo()

/***********************************************************
 ShowMeanValues()
 Displays mean values over the bar diagram of the test
 ***********************************************************/
void ShowMeanValues(){
float m_cur=0.0, m_pow=0.0;
char buff[10], str[30];
byte i;

  // Get Iidle from INA219 module
  StartInaSampling();
  delay(75);    // wait more than 69msec to have a valid value
  GetInaVal();

  for (i=0;i<(N_AUTO_REPEAT*2); i++){
    m_cur=m_cur+ServoData[i].Current;
    m_pow=m_pow+ServoData[i].Power;
  }
  m_cur=m_cur/(N_AUTO_REPEAT*2);
  m_pow=m_pow/(N_AUTO_REPEAT*2);

  // draw a black rectangle in the middle of the LCD
  tft.fillRect(30,48,TFT_WIDTH-60,52,ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
  CenterPrintStr(" Mean values ",50,1,false);

  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  dtostrf(InaCurrent_mA, 3, 2, buff);
  sprintf(str, "Iidle=%smA", buff);
  CenterPrintStr(str, 65, 1, false);

  dtostrf(m_cur, 3, 2, buff);
  sprintf(str, "Irun=%smA", buff);
  CenterPrintStr(str, 77, 1, false);

  dtostrf(m_pow, 3, 2, buff);
  sprintf(str, "Wrun=%smW", buff);
  CenterPrintStr(str, 89, 1, false);

  // wait for ENTER
  do{} while (ButtonActive(PIN_BTN_ENTER)!=BTN_PRESS);
} // ShowMeanValues()

/***********************************************************
 SaveData()
 ***********************************************************/
void SaveData(int n){
  ServoData[n].Current=InaCurrent_mA;
  ServoData[n].Power=InaPower_mW;
} // SaveData()

/***********************************************************
 StartInaSampling() 
 The function "triggers" the sampling done by INA219 so it 
 needs to be called before each GetInaVal().
 Most important: each sample collection last 69msec using 
 128 samples so the call to GetInaVal() should be done 
 at least 69msec after StartInaSampling()
 **********************************************************/
void StartInaSampling() {
  ina219.setCalibration_32V_2A();     // Start the sampling
} // StartInaSampling()  
  
/***********************************************************
 PrintInaVal()
 Prints INA vals on the serial monitor
 **********************************************************/
void PrintInaVal() {
  Serial.print("Bus Voltage:   "); Serial.print(InaBusVoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(InaShuntVoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(InaLoadVoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(InaCurrent_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(InaPower_mW); Serial.println(" mW");
  Serial.println("");
} // PrintInaVal()

/***********************************************************
 GetInaVal()
 **********************************************************/
void GetInaVal(){
  InaShuntVoltage = ina219.getShuntVoltage_mV();   // voltage at the shunt resitor ("Vin+" - "Vin-")
  InaBusVoltage = ina219.getBusVoltage_V();        // voltage seen by the circuit under test ("Vin-" - GND)
  InaCurrent_mA = ina219.getCurrent_mA();
  InaPower_mW = ina219.getPower_mW();
  InaLoadVoltage = InaBusVoltage + (InaShuntVoltage / 1000);  // Total Voltage provided at the circuit
} //GetInaVal

/***********************************************************
 GetRvarPosition()
 Return the variable Resistor position in the
 range 0...999 excluding 12 starting and trailing 
 values just to reduce the edge electrical noises. 
 ***********************************************************/
int GetRvarPosition() {
int rvalue;
  rvalue=analogRead(PIN_RVAR);
  if (rvalue<12) rvalue=0;
  if (rvalue>1000) rvalue=1000;
  return rvalue;
} // GetRvarPosition()

/***********************************************************
 InitTFT()
 ***********************************************************/
void InitTFT(){
int i;
  
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.cp437(true);                // Eanable special char set

  Serial.println(F("TFT Initialized"));
  tft.setRotation(3);   // 0 thru 3 corresponding to 4 cardinal rotations
  tft.fillScreen(ST77XX_BLACK);
  delay(100);

  // Print program name on TFT
  tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  CenterPrintStr(PGM_NAME,15,2,false);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  CenterPrintStr(PGM_VERSION,45,1,false);

  // blink display 2 times
  for (i=0; i<2; i++){
    tft.invertDisplay(true);
    delay(400);
    tft.invertDisplay(false);
    delay(400);
  }
  delay(1500);
} // InitTFT()

/***********************************************************
 CenterPrintStr()
 Print a string centered on the TFT display at the y coord.
 "strsize" must be 1 or 2
***********************************************************/
void CenterPrintStr(char * str, int y, byte strsize, bool cls){
int x, maxchars;
char strtmp[30];

  tft.setTextSize(strsize);
  // Is str too much long?
  maxchars=tft.width()/CHAR_WIDTH_1;
  if (strsize==2) maxchars=tft.width()/CHAR_WIDTH_2;
  
  if (strlen(str)>maxchars){
    strncpy(strtmp, str, maxchars);
    strtmp[maxchars]=0;
  }
  else
    strcpy(strtmp, str);

  // Compute "x" position and print strtmp
  x=(TFT_WIDTH-strlen(strtmp)*TFT_CH_WIDTH*strsize)/2;
  PrintStr(strtmp, x, y, strsize, cls);
} // CenterPrintStr()

/***********************************************************
 PrintStr()
 Print a string at x,y on the TFT display.
 "strsize" must be 1 or 2 and if cls=true the area 
 will be cleared
***********************************************************/
void PrintStr(char * str, int x, int y, byte strsize, bool cls){
int xr,yr,wr,hr;

  tft.setTextSize(strsize);
  // Clear of the text area needed?
  if (cls){
    tft.getTextBounds(str,x,y,xr,yr,wr,hr);
    tft.fillRect(xr,yr,wr,hr,ST77XX_BLACK);
  }
  tft.setCursor(x, y);
  tft.print(str);
} // PrintStr()

/***********************************************************
 ButtonActive 
 Checks the "Button" status implementing a simple anti-bounce
 return: BTN_NOPRESS=no pressed, BTN_PRESS=pressed, 
 BTN_LONGPRESS=long pressed
 ***********************************************************/
byte ButtonActive(byte button){
byte btn_status=BTN_NOPRESS;
unsigned long msec, delta_t;

  msec=millis();
  do {
    // wait for the button release
  } while (digitalRead(button)==LOW);
  delta_t=millis()-msec;
  if (delta_t>TIME_PRESS) btn_status=BTN_PRESS;
  if (delta_t>TIME_LONGPRESS) btn_status=BTN_LONGPRESS;

  // If Button was pressed, i will discharge 
  // possible opening noises before return
  if (btn_status!=BTN_NOPRESS) delay(10);

  return (btn_status);
} // ButtonActive()

/***********************************************************
 SetServoVoltage()
 Prints some instructions on the screen and then wait for
 an ENTER to return
 ***********************************************************/
void SetServoVoltage(){
char buff[10];
float prev=0.0;

  PrintTitle("  SET Vout VOLTAGE  ");
  CenterPrintStr("Select the RIGHT Vout", 30, 1, false);
  CenterPrintStr("using the switch and", 42, 1, false);
  CenterPrintStr("then don't exceed", 54, 1, false);
  tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
  CenterPrintStr(" 1A MAX ", 66, 1, false);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  CenterPrintStr("during your tests", 78, 1, false);

  PrintStr("Vout=", 25, 100, 2, false);
  do{
    GetInaVal();          // Read new values from INA219
    StartInaSampling();   // Restart INA219 for the next reading and...
    delay(75);            // wait more that 60msec that is the sampling time
    
    // InaLoadVoltage can have some "noise" so i will set a range
    if (InaLoadVoltage<(prev-0.1) || InaLoadVoltage>(prev+0.1)){
      dtostrf(InaLoadVoltage, 3, 2, buff);
      PrintStr(buff, 90, 100, 2, true);
      prev=InaLoadVoltage;
    }
  } while (ButtonActive(PIN_BTN_ENTER)!=BTN_PRESS);
} // SetServoVoltage()
