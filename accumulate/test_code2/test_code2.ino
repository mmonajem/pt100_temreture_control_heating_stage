//--------------------Initialization and Libraries for SD card breakout--------------
#include <SD.h>
#include<SPI.h>
const int CSpin = 4; // Connect CS pin to 4. And use ICSP pins of Arduino for MISO, MOSI and SCK. Since other ports are being used by LCD display.
String dataString =""; // holds the data to be written to the SD card
//int sensorReading = 1; // value read from the temperature sensor
String fileNameString;
File sensorData;
long newName = 0;
//--------------------Initializations and libraries for LCD DISPLAY---------------
//#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

int32_t x =0;
//int32_t y =11;
//------------------------Initialization of sensor and relay
int sensorReading = 0; // Sensor value from analog pin "A0"
int Relay = A1; //relay is connected to analog pin "A1"

//-------------------Initilization for button-------------------------------------
#define BUTTON_PIN1 2
#define BUTTON_PIN2 5
int bCount = 0;
bool bToggle = 0;
int temp =0;
volatile int buttonState = 0;
volatile int prevbuttonState = 0;
//------------------Initialization of clock time--------
//unsigned long curr_locktime;
int period = 3000;
unsigned long prev_clocktime = 0;
//-------------------Initialization for joystick--------------
int place = 0;
int currentXs= 80; //current xcoordinate start
int currentXe= 97; //current xcoordinate start
int previousXs= 140; //previous xcoordinate start
int previousXe= 157; //previous xcoordinate start
//volatile int x = 0;
//volatile int y = 0;
int a=0;
int b=0;
int c=0;

int xValue = 0 ;
int yValue = 0 ; 
int bValue = 0 ;

void button_press1(){
  if (buttonState == 0){
    prevbuttonState = buttonState;
    buttonState = buttonState + 1;
    //x = buttonState-prevbuttonState;
  }
  else if (buttonState == 1){
    prevbuttonState = buttonState;
    buttonState = buttonState - 1;
    //x = buttonState-prevbuttonState;
  }
  /*if((x+(buttonState-prevbuttonState) ==-2)){
      fileNameString = "data"+String(newName)+".csv";
      newName++;
    }*/
}

//----------------------SETUP-----------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(4800);
  delay(500);
  pinMode(CSpin, OUTPUT);
  if (!SD.begin(CSpin)) {
Serial.println("Card failed, or not present");
// don't do anything more:
return;
}
Serial.println("card initialized.");
  // ------for LCD---------------
 Config_Init();
  LCD_Init();
  LCD_Clear(0xffff);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, GRAY);
  Paint_Clear(GRAY);
  //Paint_DrawString_EN(30, 10, "Hello this is first test. :)", &Font24,GRAY, WHITE);
  //Paint_DrawLine(0, 60, 320, 60, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  start_screen();
  LCD_ClearWindow(150, 215, 320, 240, GRAY);
  
  //------------for SD CARD breakout board-----------------
  
  /*
  if (!SD.begin(CSpin)) {//check if SD card is being inserted
    Serial.println("Card failed, or not present");
    //Paint_DrawString_EN(120, 215, "Insert SD-card to save data", &Font16,MAGENTA, WHITE);
    while(1);
}

Serial.println("card initialized.");
*/

  //-----------for BUTTON---------
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN1), button_press1, RISING);
  //--------------for Relay ----------------
  pinMode(Relay, OUTPUT);
  delay(2000);
//Paint_DrawString_EN(120, 215, "Set reqd Temp..", &Font16,MAGENTA, WHITE);
  prev_clocktime = millis();
}
//-------------------------------------------------------------LOOP------------------------------------------------------------------------------------------------------------
void loop() {
  sensorReading = analogRead(A0);
  LCD_ClearWindow(100, 90, 100, 240, GRAY);
  Paint_DrawNum(100, 90, sensorReading , &Font24,BROWN, WHITE);
  if (buttonState == 1){
  set_temp();
  }
Serial.print(buttonState);
    /*
  press_button1();
  delay(100);
  
  if(bToggle == 1){
    set_temp();
  }*/

  if (sensorReading >= temp-5){
    sensorReading = temp;
    digitalWrite(Relay, LOW);
    LCD_ClearWindow(150, 215, 320, 240, GRAY);
    Paint_DrawString_EN(150, 215, "Equal", &Font20,MAGENTA, WHITE);
    LCD_ClearWindow(100, 90, 100, 240, GRAY);
    Paint_DrawNum(100, 90, sensorReading , &Font24,BROWN, WHITE);
  }
  else if (sensorReading < temp+5){ 
    sensorReading = analogRead(A0);
    delay(10);
    digitalWrite(Relay, HIGH);
    //write current temp.
    LCD_ClearWindow(100, 90, 100, 240, GRAY);
    Paint_DrawNum(100, 90, sensorReading , &Font24,BROWN, WHITE);
    //write status
    LCD_ClearWindow(150, 215, 320, 240, GRAY);
    Paint_DrawString_EN(150, 215, "Increase", &Font20,MAGENTA, WHITE);
    //dataString = String(sensorReading); // convert to CSV
    //saveData(); // save to SD card
    
    }
    if(buttonState==0){
      if(millis() >= prev_clocktime + period){
        dataString = String(sensorReading); // convert to CSV
        saveData(); // save to SD card
        prev_clocktime += period;;
      }

    }
    
//dataString = String(millis()); // convert to CSV
//saveData(); // save to SD card
//delay(600); // delay before next write to SD Card, adjust as required
//sensorReading++;

    
  

}

//-------function to set TEMPERATURE--------------------

void set_temp(){
  Paint_Clear(GRAY);
  
  set_temp_screen();
    delay(50);
    
    temp = 0;
    while(true){
      //press_button2_inc();
      //press_button1();
      if (buttonState == 0){
        temp = (100*a)+(10*b)+c;
        break;
      }
      //LCD_ClearWindow(100, 170, 320, 200, GRAY);
      xValue = analogRead(A4);	
      yValue = analogRead(A5);
      Paint_DrawNum(100, 170, a , &Font24,BLUE, WHITE);
      Paint_DrawNum(120, 170, b , &Font24,BLUE, WHITE);
      Paint_DrawNum(140, 170, c , &Font24,BLUE, WHITE);
      /*
      Paint_DrawLine(100, 200, 117, 200, YELLOW,   DOT_PIXEL_2X2,LINE_STYLE_SOLID); // line at bottom to indicate "a"
      delay(500);
      LCD_ClearWindow(97, 195, 320, 205, GRAY);
      
      Paint_DrawLine(120, 200, 137, 200, YELLOW,   DOT_PIXEL_2X2,LINE_STYLE_SOLID); // line at bottom to indicate "b"
      delay(500);
      LCD_ClearWindow(97, 195, 320, 205, GRAY);
      Paint_DrawLine(140, 200, 157, 200, YELLOW,   DOT_PIXEL_2X2,LINE_STYLE_SOLID); // line at bottom to indicate "c"
      delay(500);
      LCD_ClearWindow(97, 195, 320, 205, GRAY);
      */

      if ( place == 1){ 
        if ((xValue > 750) && (yValue >495)){// If we move UP in joystick
          a=a+1;
          if(a>9){
            a=0;
          }
          Paint_DrawNum(100, 170, a , &Font24,BLUE, WHITE);
        }
        else if ((xValue < 260) && (yValue > 495)){
          a = a-1;
          if(a<0){
            a=9;
          }
          Paint_DrawNum(100, 170, a , &Font24,BLUE, WHITE);
        }
        
      }
      else if (place ==2){
        if ((xValue > 750) && (yValue >495)){// If we move UP in joystick
          b=b+1;
          if(b>9){
            b=0;
          }
          Paint_DrawNum(120, 170, b , &Font24,BLUE, WHITE);
          
        }
        else if ((xValue < 260) && (yValue > 495)){
          b = b-1;
          if (b<0){
            b=9;
          }
          Paint_DrawNum(120, 170, b , &Font24,BLUE, WHITE);
          
        }
      }
      else if (place ==3){
        if ((xValue > 750) && (yValue >495)){// If we move UP in joystick
          c=c+1;
          if (c>9){
            c=0;
          }
          Paint_DrawNum(140, 170, c , &Font24,BLUE, WHITE);
          
        }
        else if ((xValue < 260) && (yValue > 495)){
          c = c-1;
          if (c<0){
            c=9;
          }
          Paint_DrawNum(140, 170, c , &Font24,BLUE, WHITE);
          
        }
      }
      
      if ((xValue>495) && (yValue <245)){ // move right
        place = place+1;
        if (place > 3){
          place = 3;
        }
        delay(500);
        
        int xs = currentXs+(20*place);
        int xe = currentXe + (20*place);
        LCD_ClearWindow(97, 195, 320, 205, GRAY);
        Paint_DrawLine(xs, 200, xe, 200, YELLOW,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
        //previousXs = xs;
        //previousXe = xe;
      }
      else if((xValue>495) && (yValue >775)){ // move left
        place = place-1;
        if (place <1){
          place = 1;
        }
        delay(500);
        
        int xs = previousXs-(20*(3-place));
        int xe = previousXe - (20*(3-place));
        LCD_ClearWindow(97, 195, 320, 205, GRAY);
        Paint_DrawLine(xs, 200, xe, 200, YELLOW,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
        //previousXs = xs;
        //previousXe = xe;

      }
      Paint_DrawNum(20, 10, xValue , &Font24,BLUE, WHITE);

      //delay(100);
    }
    LCD_Clear(GRAY);

    start_screen();

    //write current temperature
    LCD_ClearWindow(100, 90, 100, 240, GRAY);
    Paint_DrawNum(100, 90, x , &Font24,BROWN, WHITE);
    //write set temperature
    LCD_ClearWindow(100, 170, 100, 240, GRAY);
    Paint_DrawNum(100, 170, temp , &Font24,BLUE, WHITE);
    
    
}

//------------------------function for START SCREEN ------------------------
void start_screen(){
  Paint_DrawString_EN(20, 10, "TEMPERATURE in 'C ", &Font24,GRAY, WHITE);
  Paint_DrawLine(0, 40, 320, 40, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 60, "Current temperature:", &Font20,GRAY, WHITE);
  Paint_DrawLine(0, 125, 320, 125, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 140, "Set temperature:", &Font20,GRAY, WHITE);
  Paint_DrawLine(0, 200, 320, 200, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 215, "Status:", &Font20,GRAY, WHITE);
}
//----------------------function SET TEMP. SCREEN----------------------------
void set_temp_screen(){
  //clear the CURRENT TEMPERATURE window
  LCD_ClearWindow(0, 0, 320, 122, GRAY);
  //current temperature window
  //Paint_DrawLine(0, 40, 320, 40, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  //Paint_DrawString_EN(20, 60, "Current temperature:", &Font20,GRAY, WHITE);
  //set temperature window
  Paint_DrawLine(0, 125, 320, 125, GREEN,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 140, "Set temperature:", &Font20,GRAY, WHITE);
  //-----window for STATUS
  LCD_ClearWindow(0, 195, 320, 205, GRAY);
  Paint_DrawLine(0, 210, 320, 210, GREEN,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 215, "Status:", &Font20,GRAY, WHITE);
  // writing "Setting Temp....." in the status of the screen
  LCD_ClearWindow(150, 215, 320, 240, GRAY);
  Paint_DrawString_EN(120, 215, "Setting Temp..", &Font20,MAGENTA, WHITE);
}

//--------------------------function for BUTTON1 VALUE INCREMENT ----------------------
void press_button1_inc(){
  if (digitalRead(BUTTON_PIN1) == 0){
    if (digitalRead(BUTTON_PIN1) == 1){
      x++;
      Serial.println(x);
    }
    
  }
}

//-------------------------function for BUTTON1 PRESS------------------------
void press_button1(){
  if (digitalRead(BUTTON_PIN1) == 0){
    delay(100);
    if (digitalRead(BUTTON_PIN1) == 1){
      if (bToggle == 0){
        bToggle = 1;
      }
      else{
        bToggle = 0;
      }
      // print the toggle value
      Serial.println(bToggle);
    }
    
  }
}
//--------------------------function for BUTTON2 VALUE INCREMENT ----------------------
void press_button2_inc(){
  if (digitalRead(BUTTON_PIN2) == 0){
    delay(100);
    if (digitalRead(BUTTON_PIN2) == 1){
      temp++;
      delay(100);
      LCD_ClearWindow(100, 170, 320, 200, GRAY);
      Paint_DrawNum(100, 170, temp , &Font24,BLUE, WHITE);
      Serial.println(temp);
    }
    
  }
}

//-------------------------function to write data into SD Card--------------------------
void saveData(){
  // check the card is still there
  // now append new data file
  sensorData = SD.open("data.csv", FILE_WRITE);
  if (sensorData){
    sensorData.print(dataString);
    sensorData.print("\t");
    sensorData.println(millis());
    sensorData.close(); // close the file
    }
}