//alarm sound, steriotypical redneck horn - duks of hazzard horn
// F D B B B C D E F F F D
// Current working: set time and date
// set alarm mode
// normal alarm mode works
//defusable alarm mode does not work
byte zero = 0x00; //workaround for issue #527

#include <avr/interrupt.h>
#include <EEPROM.h>
#include "Wire.h"
#define DS1307_ADDRESS 0x68
#include <LiquidCrystal.h>
#include "pitches.h"
#include "chars.h" //custom characters for large font

char* myMonths[]={"Placeholder", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char* myMonths2[]={"Placeholder", "Jan", "Feb", "March", "April", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec"};
char* myWeekDays[]={"PlaceHolder","Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday","Saturday"};
byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}
int buttonALow = 475;   //alarm set button
int buttonAHigh = 550;
int buttonBLow = 350;
int buttonBHigh = 425;
int buttonCLow = 475;   // add hour
int buttonCHigh = 550;
int buttonDLow = 325;   // add minute
int buttonDHigh = 425;

int buttonABLow = 625;    // set time
int buttonABHigh = 700;
int buttonCDLow = 625;
int buttonCDHigh = 700;

int defusePinGreenLow = 890;
int defusePinGreenHigh = 950;
int defusePinYellowLow = 825;
int defusePinYellowHigh = 875;
int defusePinRedLow = 700;
int defusePinRedHigh = 750;
int defusePinBlackLow = 475;
int defusePinBlackHigh = 525;

// Initialize Pins
int buttonAlarmCancel = 0;//pin 2
int buzzer = 3;
int redgreenLED = 4;
int yellowLED = 5;
int backLight = 6;    // pin 6 will control the backlight
LiquidCrystal lcd(13,12,11,10,9,8,7);

int photoSensor = A0;  // A0 = input for photosensor
int buttonsSet = A1;
int buttonsTime = A2;
int defusePin = A3;
int cutWire; // variable set to read val of DefuseWire
int correctWire = 0;
int countDown = 11;

int LEDbrightness;    //  led brightness variable based on photosensor
int minb = 950;        // minimum brightness
int maxb = 1023;       // maximum brightness

int EEsize = 1024; // size in bytes of your board's EEPROM
int AM;            // 0 = pm, 1 = am
int time[] = {1,2,3,4};  //[hour1, hour2, min1,min2]
int timeOld[] = {0,0,0,0};

int weekDay;
int month;
int monthDay;
int year;
int year2 = 20;
int year3 = year2*100+year;
int hour;
int hour12;
int minute;
int second;

int DLS;
int menuOption;
int debug = 0;

int alarmHour = EEPROM.read(0);
int alarmMin = EEPROM.read(1);
int alarmAM = EEPROM.read(2);     // alarm time is AM = 1 or PM = 0
int alarmStatus = EEPROM.read(3); // alarm off = 0, alarm on and defusable = 1, alarm on with cancel button = 2
int alarming = 0;   // alarm is sounding = 1, not sounding = 0

int setRandPinHour = 6;
int setRandPinMin = 30;

int x = 0;  //starting point for large fonts
int y = 0;  // y = 1 tells DispLgTime it needs to print the time
volatile int interruptVal = 0; // interrupt pin state - 0: has not been pushed, 1: has been pushed


//int DLS = 0; // sets day luight savings to off

//volatile int alarmStatus = 0; //alarm starts off

//F D B B B C D E F F F D
//int melody[] = {NOTE_DS8};

int melody[] = {NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
int noteDurations[] = {4, 8, 8, 4,4,4,4,4 };



// %%%%%%%%%%%%%% SET UP LOOP %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() 
{
  pinMode(yellowLED,OUTPUT);
  pinMode(redgreenLED,INPUT);  
  pinMode(buzzer,OUTPUT);
  pinMode(buttonAlarmCancel, INPUT);
  pinMode(buttonsSet, INPUT);
  pinMode(buttonsTime, INPUT);
  pinMode(defusePin, INPUT);
  debug = digitalRead(redgreenLED);
  attachInterrupt(buttonAlarmCancel, CancelAlarm, RISING);
  
  pinMode(backLight, OUTPUT);
  pinMode(photoSensor, INPUT);
  analogWrite(backLight, 255);
  
  
  Wire.begin();
  Serial.begin(9600);

/////////// Creates 8 custom shapes for large fonts //////////////////////////
  lcd.createChar(8,LT);
  lcd.createChar(1,UB);
  lcd.createChar(2,RT);
  lcd.createChar(3,LL);
  lcd.createChar(4,LB);
  lcd.createChar(5,LR);
  lcd.createChar(6,UMB);
  lcd.createChar(7,LMB);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  
  HelloColby(); // Calls function to display "Hello Colby!" in large font
  lcd.clear();
  
}
                                          
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%% MAIN LOOP %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop(){
   ReadDateTime(); 
 if (debug == 1)
{
  
 debugMode();
}
else
{
 //ReadDateTime(); 
 Hour12(); // takes hour in 24h format and makes hour12 into 12 hour format
  
  y = DispLgTime();//hour, minute, AM, y);

  // sets random pin to be the correct wire 10 mins before the allarm is set to go off
  if (hour == setRandPinHour && minute == setRandPinMin && second == 0){correctWire = random(0,4);}
 
  x=0;
  
 
  y = checkInt();//alarming);
  alarmAM = setAlarm();//hour, minute);  // checks for change to alarm time and returns if its currentlyr set for am or pm
  alarmStatus = EEPROM.read(3);  // checks each loop to see what state the alarm is currentl set for (off, defusable mode, normal mode)
  alarmState();//alarmStatus);  // checks for button press which changes the current alarmState
  cutWire = checkWire(); // checks current state of defusable wires
  checkAlarm();//alarmHour, hour, alarmMin, minute, alarmAM, AM, alarmStatus, second);
  countDown = Defuse();//countDown);  // countDown stays at 11 unless alarm is going off checks for alarming and 
  Bomb(countDown);
  if (countDown == 0)
  {
    x = 11;
    lcd.write("*");
    customBOMB();
    delay(200);
    Explosion();
  }

    
  AM = setDateTime();//minute, hour, weekDay, monthDay, month, year, AM);
  checkAlarm();//alarmHour, hour, alarmMin, minute, alarmAM, AM, alarmStatus, second); 

  
  LEDbrightness = map(analogRead(photoSensor),minb,maxb,25,255);
  if (LEDbrightness > 255)
  {
    LEDbrightness = 255;
  }
  else if (LEDbrightness < 1)
  {
    LEDbrightness = 25;
  }
  
  
  analogWrite(backLight, LEDbrightness); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
  
  Serial.print(addZero(hour12));
  Serial.print(":");
  Serial.print(addZero(minute));
  Serial.print(":");
  Serial.println(addZero(second));
  int val7 = analogRead(photoSensor);
  Serial.println(val7);
  int val4 = analogRead(buttonsSet);
  Serial.println(val4);
  int val5 = analogRead(buttonsTime);
  Serial.println(val5);
  int val6 = analogRead(defusePin);
  Serial.println(val6);
  int val8 = analogRead(redgreenLED);
  Serial.println(val8);
  
  /*
  int buttonsSet = A1;
int buttonsTime = A2;
int defusePin = A3;
*/
     
   // display time in large font, returns y = 0
  //Serial.println(y);
  delay(500);  // refresh every half second
}
}
// %%%%%%%%%%%%%%%%%%%%% END MAIN LOOP %%%%%%%%%%%%%%%%%%%%

String addZero(int val) { 
if (val<10) return "0" + String(val);else return String(val); 
}




int checkInt()//int alarming)
{
    if (interruptVal == 1)
    {
      if (alarmStatus == 1) // 1 = Defusable Mode
      {
        if (alarming  == 1)
        {
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Cut the Correct Wire");
          delay(1000);
        }
        else 
        {
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Defusable Mode");
          lcd.setCursor(0,1);
          lcd.print("Set for: ");
          lcd.print(addZero(alarmHour));
          lcd.print(":");
          lcd.print(addZero(alarmMin));
          if (alarmAM == 0){lcd.print("PM");}
          else {lcd.print("AM");}
          //lcd.print("M");
          delay(2000);
          //melodys();
          lcd.clear();
          interruptVal = 0;
          
          y=1;
        }
      }  
          
        else if (alarmStatus == 2) // 2 = Normal Cancel Button Mode
        {
        if (alarming  == 1)
        {
          lcd.clear();
          alarming = 0;
          lcd.setCursor(1, 0);
          lcd.print("Alarm Cancelled");
          delay(1000);
        }
        else 
        {
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Normal Mode");
          lcd.setCursor(0,1);
          lcd.print("Set for: ");
          lcd.print(addZero(alarmHour));
          lcd.print(":");
          lcd.print(addZero(alarmMin));
          //lcd.print(" ");
          if (alarmAM == 0){lcd.print("P");}
          else {lcd.print("A");}
          lcd.print("M");
          delay(2000);
          //melodys();
          //lcd.clear();
          interruptVal = 0;
          y=1;
        }
        }
       
      else
      {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Alarm is OFF"); 
        delay(1000);
        lcd.clear();
        interruptVal = 0; 
        y = 1;
       //
      }
    DispLgTime();//hour, minute, AM,y);
    }
}

//checkAlarm(alarmHour, hour, alarmMin, minute, alarmAM, AM, alarmStatus, second)
void checkAlarm(){//int alarmHour, int hour, int alarmMin, int minute, int alarmAM, int AM, int alarmStatus, int second){
  if (alarmHour == hour12 && alarmMin == minute && alarmAM ==AM && alarmStatus ==1 && second < 3) // Defusable Mode
  {
    alarming = 1;
  }
  else if (alarmHour == hour12 && alarmMin == minute && alarmAM ==AM && alarmStatus ==2 && second < 3) // Normal Cancel Button Mode
  {
    
    alarming = 1;
    while (alarming == 1) 
    {
      //Serial.println("success");
     
      if (interruptVal == 1){alarming = 0;}
      else 
      {
        alarming = 1;
        melodys();
      }
   }
  }
  else{}
}

int Defuse(){//int countDown){
  if (alarming == 1)
  {
    //start countDown alarm
    if (alarmStatus == 1) // check if defusable setting is on
    {
      cutWire = checkWire(); // checks current state of defusable wires
      if (cutWire == correctWire)
      {
        alarming = 0;
        //display txt saying defused, play zelda tone (discovered new item sound)
      }
      else if (cutWire == 5)
      {
        //make large explosion sound
      }
      else if (cutWire == 0){return 11;} 
      else
      {
        countDown --;
      }
    }
    else{return 11;}
  }
  else{return 11;}
}

// %%%%%%%%%%%%%%%%%%%%%%%%% SET ALARM %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int setAlarm(){//int hour, int minute){
  //checkAlarmTime();
  //int AlarmAM2 = AlarmAM;
  //AlarmAM = EEPROM.read(2);
  
  if (analogRead(buttonsSet) > buttonALow && analogRead(buttonsSet) < buttonAHigh)
  {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Set alarm for:");
    lcd.setCursor(1,1);
    lcd.print(addZero(alarmHour));
    lcd.print(":");
    lcd.print(addZero(alarmMin));
    lcd.print(" ");
    if (alarmAM == 1){lcd.print("AM");}
    else if (alarmAM ==0){lcd.print("PM");}

    delay(100);  // was 200, trying 100 for now see if its less buggy
    while (analogRead(buttonsSet) > buttonALow && analogRead(buttonsSet) < buttonAHigh)
    {
     if (analogRead(buttonsTime) > buttonCLow && analogRead(buttonsTime) < buttonCHigh)
     {
       alarmHour++;
       if (alarmHour > 12) 
       { 
         alarmHour = 1;
         if (alarmAM == 0){alarmAM=1;}
         else {alarmAM=0;}
       }
       
       
       lcd.setCursor(1,1);
       lcd.print(addZero(alarmHour));
       lcd.setCursor(7,1);
       if (alarmAM == 1){lcd.print("AM");}
       else {lcd.print("PM");}
       delay(200);
       y = 1;
     }
     else if (analogRead(buttonsTime) > buttonDLow && analogRead(buttonsTime) < buttonDHigh)
     {
       alarmMin++;
       if (alarmMin > 59) {alarmMin = 00;}
       lcd.setCursor(4,1);
       lcd.print(addZero(alarmMin));
       delay(200);
       y=1;
     }
     else if (analogRead(buttonsTime) > buttonCDLow && analogRead(buttonsTime) < buttonCDHigh)
     {
       lcd.clear();
       lcd.setCursor(1,0);
       lcd.write("STOP BUTTON");
       lcd.setCursor(1,1);
       lcd.write("SMASHING!");
       delay(1000);
       y=1;
     }
     else{y=1;}
    }

  delay(750);
  EEPROM.write(0,alarmHour);
  EEPROM.write(1,alarmMin);
  EEPROM.write(2,alarmAM);
  // sets time in which to assign a random pin to be the correct wire to cut
  lcd.clear();
  if (alarmMin < 10)
  {
    setRandPinHour = alarmHour - 1;
    setRandPinMin = 60 - alarmMin;
  }
  else 
  { 
    setRandPinHour = alarmHour;
    setRandPinMin = alarmMin - 10;
  }
  y=1;
  DispLgTime();//hour, minute, AM, y);
  if (alarmAM == 0) {return 0;}
  else {return 1;}
  
  }
  else 
  {
    y=0;
    return alarmAM; 
  }
}

void alarmState()//int alarmStatus)
{ // check
  
  
  int val = digitalRead(redgreenLED); // button press sets alarm state (uses LED pin as input)
  if (val == 1)
  { // check
    delay(200);
    lcd.clear();
    if (val == 1 && analogRead(buttonsTime) > buttonCDLow && analogRead(buttonsTime) < buttonCDHigh)
    { // check
    
         lcd.setCursor(0,0);
         lcd.print("Set Date");
         lcd.setCursor(0,1);
         lcd.print("[opt][");
         lcd.print(char(127));
         lcd.print("][+][-]");
         int z = 1;
         int DLS2;
         while (z==1)
         {
           delay(500);
           /*     [a] [b] [c] [d]   (button layout: alarm, time, hour, minute)
           b) cancels this mode
           a) toggles from weekday, monthDay, month, year 
           c) adds to whatever variable is selected from a)
           d) subtracts from whatever variable is selected from a)
           */
           
            
           if (analogRead(buttonsSet) > buttonALow && analogRead(buttonsSet) < buttonAHigh) // 335 - 350 for button a 
           { // check
           if (menuOption == 0)
            {
              menuOption = 1;                // monthDay
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Day of the Month:");
              lcd.setCursor(0,1);
              lcd.print(monthDay);
              NumSuffix(monthDay);       //prints suffix for the day. ex if monthDay = 30, print 30th
              lcd.print(" of ");
              lcd.print(myMonths[month]);
            }
            else if (menuOption == 1)
            {
              menuOption = 2;                // month
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Month:");
              lcd.setCursor(0,1);
              lcd.print(myMonths[month]);
            }
            
            else if (menuOption == 2)
            {
              year3 = year2*100+year;
              menuOption = 3;   // year
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Year:");
              lcd.setCursor(0,1);
              lcd.print("                ");
              lcd.setCursor(0,1);
              lcd.print(year3);
               }
            
            else if (menuOption == 3)
            {
              menuOption = 4;              // DLS
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Toggle DLS");
              lcd.setCursor(0,1);
              if (DLS == 1){lcd.print("ON ");}
              else {lcd.print("OFF");}
            }
            
            else if (menuOption == 4)
            {
              menuOption = 0;                // weekDay
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Day of the Week:");
              lcd.setCursor(0,1);
              lcd.print(myWeekDays[weekDay]);       
           }
           
          } // button a
            
           else if (analogRead(buttonsSet) > buttonBLow && analogRead(buttonsSet) < buttonBHigh) // exits this menu  button b
           {
             lcd.clear();
             y=1;
             setDateTime();
             DispLgTime();
             //minute, hour, weekDay, monthDay, month, year, AM);
             
             z=0;  
             //return;
             
           } // button b
            
            
            else if (analogRead(buttonsTime) > buttonCLow && analogRead(buttonsTime) < buttonCHigh) // exits this menu  button c
            {
             
              
              if (menuOption == 0) 
              {
                weekDay++;
                if (weekDay > 7){weekDay = 1;}
                lcd.setCursor(0,1); 
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(myWeekDays[weekDay]);
                delay(200);
                y=1;
              }
              
              if (menuOption == 1) 
              {
              
                monthDay++;
                if (monthDay > 31){monthDay = 1;}
                lcd.setCursor(0,1); 
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(monthDay);
                NumSuffix(monthDay);
                lcd.print(" of ");
                lcd.print(myMonths[month]);
                delay(200);
                y=1;
              }
              
              if (menuOption == 2) 
              {
                month++;;
                if (month > 12){month = 1;}
                lcd.setCursor(0,1); 
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(myMonths[month]);
                delay(200);
                y=1;
              }
              
              if (menuOption == 3) 
              {
                year++;
                year3 = year2*100+year;
                if (year > 99)
                {
                  year2++;
                  year = 0;
                  year3 = year2*100+year;
                }
                lcd.setCursor(0,1); 
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(year3);
                delay(200);
                y=1;
              }
              
              if (menuOption == 4) 
              {
                if (DLS == 1) {DLS2 = 0;}
                else {DLS2 = 1;}
                DLS = DLS2;
                lcd.setCursor(0,1); 
                if (DLS == 1){lcd.print("ON ");}
                else {lcd.print("OFF");}
              
              
              }
              setDateTime();
            
            }  // button c
            else if (analogRead(buttonsTime) > buttonDLow && analogRead(buttonsTime) < buttonDHigh) //button d
            {
              
              if (menuOption == 0) 
              {
                weekDay--;
                if (weekDay < 1){weekDay = 7;}
                lcd.setCursor(0,1); 
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(myWeekDays[weekDay]);
                delay(200);
                y=1;
             
              }
              else if (menuOption == 1) 
              {
                monthDay--;
                if (monthDay < 1){monthDay = 31;}
                lcd.setCursor(0,1); 
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(monthDay);
                NumSuffix(monthDay);
                lcd.print(" of ");
                lcd.print(myMonths[month]);
                delay(200);
                y=1;
              }
              
              
              else if (menuOption == 2) 
              {
                month--;
                if (month < 1){month = 12;}
                lcd.setCursor(0,1); 
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(myMonths[month]);
                delay(200);
                y=1;
              }
              
              else if (menuOption == 3) 
              {
                year--;
                year3 = year2*100+year;
                if (year < 0)
                {
                  year2--;
                  year = 99;
                  year3 = year2*100+year;
                }
                lcd.setCursor(0,1); 
                lcd.print("                ");
                lcd.setCursor(0,1);
                lcd.print(year3);
                delay(200);
                y=1;
              }
              
              else if (menuOption == 4) 
              {
                if (DLS == 1) {DLS2 = 0;}
                else {DLS2 = 1;}
                DLS = DLS2;
                lcd.setCursor(0,1); 
                if (DLS == 1){lcd.print("ON ");}
                else {lcd.print("OFF");}
                delay(200);
              
              
              }
              setDateTime();
            } // end of button d

          
       } //while
    
    } // else if val == 0
      
      else if (val == 1 && analogRead(buttonsTime) < 50 )
      {
        if (alarmStatus == 0)
        {
          lcd.setCursor(0,0);
          lcd.print("Alarm ON:");
          lcd.setCursor(1,1);
          lcd.print("Defusable Mode");
          delay(2000);
          lcd.clear();
          y=1;
          EEPROM.write(3,1);
        }
       
        else if (alarmStatus == 1)
        {
          lcd.setCursor(0,0);
          lcd.print("Alarm ON:");
          lcd.setCursor(2,1);
          lcd.print("Normal Mode");
          delay(2000);
          lcd.clear();
          y=1;
          EEPROM.write(3,2);
        
        }
       
        else if (alarmStatus == 2)
        {
        
          lcd.setCursor(0,0);
          lcd.print("Alarm OFF!");
          delay(2000);
          lcd.clear();
          y=1;
          EEPROM.write(3,0);
        
        }
        else 
        {
          alarmStatus = 0;
        }       
      } 
  } // if (val == 0)
} // void function

int checkWire() // returns which wire has been cut, if none, returns 0, if more than 1 wire, returns 5
{
  if (analogRead(defusePin) >= 0 && analogRead(defusePin) < 100){return 0;}
  else if (analogRead(defusePin) > defusePinGreenLow && analogRead(defusePin) < defusePinGreenHigh){return 1;} // black
  else if (analogRead(defusePin) > defusePinYellowLow && analogRead(defusePin) < defusePinYellowHigh){return 2;} // red
  else if (analogRead(defusePin) > defusePinRedLow && analogRead(defusePin) < defusePinRedHigh){return 3;} // yellow
  else if (analogRead(defusePin) > defusePinBlackLow && analogRead(defusePin) < defusePinBlackHigh){return 4;} // green
  else {return 5;}  // 5 means multiple wires have been cut and bomb should blow up
}

void LED(String color, int Delay){
  
  if (color=="green")
  {
    pinMode(redgreenLED,OUTPUT);
    digitalWrite(redgreenLED,HIGH);
    delay(Delay);
    pinMode(redgreenLED,INPUT);
  }
  else if (color=="red")
  {
    pinMode(redgreenLED,OUTPUT);
    digitalWrite(redgreenLED,LOW);
    delay(Delay);
    pinMode(redgreenLED,INPUT);
  }
  else if (color == "yellow")
  {
    digitalWrite(yellowLED,HIGH);
    delay(Delay);
    digitalWrite(yellowLED,LOW);
  }
}
 
void CancelAlarm(){//int alarmStatus, int alarmHour, int alarmMin){
interruptVal = 1;
y = 1;
} 


void NumSuffix(int  number){  //only works on 1 or 2 digit numbers

  String x = addZero(number); 
  
  int y = int(x[0]-48);
  int z = int(x[1]-48);
  
  
  if (z == 1)
  {
  
    if (y == 1){lcd.print("th");}
    else {lcd.print("st");}
  }
  else if (z == 2)
  {
    if (y == 1){lcd.print("th");}
    else {lcd.print("nd");}
  }
  else if (z == 3)
   { 
    if (y == 1){lcd.print("th");}
    else {lcd.print("rd");}
   }
  else 
  {
    lcd.print("th");
  }
}
void ReadDateTime(){
  
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  
  second = bcdToDec(Wire.read());
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  monthDay = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());
  
}

int setDateTime(){
//(minute, hour, weekDay, monthDay, month, year, AM)
  if (analogRead(buttonsSet) > buttonABLow && analogRead(buttonsSet) < buttonABHigh)
  {
    
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Set time to:");
    lcd.setCursor(1,1);
    lcd.print(addZero(hour12));
    lcd.print(":");
    lcd.print(addZero(minute));
    lcd.print(":");
    lcd.print("00");  //always sets seconds to zero
    lcd.print(" ");
    if (AM == 1){lcd.print("AM");}
    else if (AM ==0){lcd.print("PM");}
    
    delay(200);
    while (analogRead(buttonsSet) > buttonABLow && analogRead(buttonsSet) < buttonABHigh)
    {
     if (analogRead(buttonsTime) > buttonCLow && analogRead(buttonsTime) < buttonCHigh)
     { 
       hour++;
       //hour12++;
       if (hour > 23){hour = 0;}
       if (hour > 12){hour12 = hour - 12;}
       else if (hour == 0){hour12 = 12;}
       else {hour12 = hour;}
    
       if (hour < 12){AM = 1;}
       else {AM = 0;}

       lcd.setCursor(1,1);
       lcd.print(addZero(hour12));
       lcd.setCursor(10,1);
       if (AM == 1){lcd.print("AM");}
       else {lcd.print("PM");}
       delay(200);
       y=1;
     }
     else if (analogRead(buttonsTime) > buttonDLow && analogRead(buttonsTime) < buttonDHigh)
     {
       
       minute++;
       if (minute > 59) {minute = 00;}
       lcd.setCursor(4,1);
       lcd.print(addZero(minute));
       delay(200);
       y=1;
     }
     else if (analogRead(buttonsTime) > buttonCDLow && analogRead(buttonsTime) < buttonCDHigh)
     {
       lcd.clear();
       lcd.setCursor(1,0);
       lcd.write("STOP BUTTON");
       lcd.setCursor(1,1);
       lcd.write("SMASHING!");
       delay(1000);
       y=1;
    }
    else {y=1;}
  }
  
  second=0;
  
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(monthDay));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.write(zero); //start 
  Wire.endTransmission();
  }
}

void Hour12(){
    if (hour > 12){hour12 = hour - 12;}
    else if (hour == 0){hour12 = 12;}
    else {hour12 = hour;}
    if (hour < 12){AM = 1;}
    else {AM = 0;}
}
