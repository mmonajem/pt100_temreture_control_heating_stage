//--------------------Initialization and Libraries for SD card breakout--------------
#include <SD.h>
#include<SPI.h>
const int CSpin = 4; // Connect CS pin to 4. And use ICSP pins of Arduino for MISO, MOSI and SCK. Since other ports are being used by LCD display.
String dataString =""; // holds the data to be written to the SD card
//int sensorReading = 1; // value read from the temperature sensor

File sensorData;

//--------------------Initializations and libraries for LCD DISPLAY---------------
#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"

int32_t x =0;
int32_t y =11;
int sensorReading = 0;

//-------------------Initilization for button-------------------------------------
#define BUTTON_PIN1 6
#define BUTTON_PIN2 5
int bCount = 0;
bool bToggle = 0;
int temp =0;


void setup() {
  Serial.begin(9600);
  // ------for LCD---------------
 /*Config_Init();
  LCD_Init();
  LCD_Clear(0xffff);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, GRAY);
  Paint_Clear(GRAY);
  //Paint_DrawString_EN(30, 10, "Hello this is first test. :)", &Font24,GRAY, WHITE);
  //Paint_DrawLine(0, 60, 320, 60, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  start_screen();
  LCD_ClearWindow(150, 215, 320, 240, GRAY);*/
  
  //------------for SD CARD breakout board-----------------
  
  pinMode(CSpin, OUTPUT);
  if (!SD.begin(CSpin)) {//check if SD card is being inserted
    Serial.println("Card failed, or not present");
    //Paint_DrawString_EN(120, 215, "Insert SD-card to save data", &Font16,MAGENTA, WHITE);
    return;
}
Serial.println("card initialized.");


  //-----------for BUTTON---------
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  //------------------------------
delay(2000);
//Paint_DrawString_EN(120, 215, "Set reqd Temp..", &Font16,MAGENTA, WHITE);
}

void loop() {
    /*
  press_button1();
  delay(100);
  
  if(bToggle == 1){
    set_temp();
  }

  if (sensorReading == temp){
    sensorReading = temp;
    LCD_ClearWindow(150, 215, 320, 240, GRAY);
    Paint_DrawString_EN(150, 215, "Equal", &Font20,MAGENTA, WHITE);
  }
  else if (sensorReading < temp){
    sensorReading++;
    //write current temp.
    LCD_ClearWindow(100, 90, 100, 240, GRAY);
    Paint_DrawNum(100, 90, sensorReading , &Font24,BROWN, WHITE);
    //write status
    LCD_ClearWindow(150, 215, 320, 240, GRAY);
   Paint_DrawString_EN(150, 215, "Increase", &Font20,MAGENTA, WHITE);
    dataString = String(sensorReading); // convert to CSV
    saveData(); // save to SD card
    
    }*/
dataString = String(millis()); // convert to CSV
saveData(); // save to SD card
//delay(600); // delay before next write to SD Card, adjust as required
//sensorReading++;
    
  

}

//-------function to set TEMPERATURE--------------------

void set_temp(){
  
  set_temp_screen();
    delay(500);
    
    temp = 0;
    while(true){
      press_button2_inc();
      press_button1();
      if (bToggle == 0){
        break;
      }
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
  Paint_DrawLine(0, 200, 320, 200, GREEN,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
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
  if(SD.exists("data.csv")){ // check the card is still there
  // now append new data file
  sensorData = SD.open("data.csv", FILE_WRITE);
  if (sensorData){
    sensorData.println(dataString);
    sensorData.close(); // close the file
    }
  }
  else{
    //Serial.println("Error writing to file !");
    }
}