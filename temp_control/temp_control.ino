//--------------------Initialization and Libraries for SD card breakout--------------
#include <SD.h>
#include<SPI.h>
const int CSpin = 4; // Connect CS pin to 4. And use ICSP pins of Arduino for MISO, MOSI and SCK. Since other ports are being used by LCD display.
String dataString =""; // holds the data to be written to the SD card
String setString= "";
String fileNameString;
File sensorData;
File header;
File countFile;
File myFile;
long newName = 0;
String file_number ;
//--------------------Initializations and libraries for LCD DISPLAY---------------
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"
//------------------------Initialization of sensor and MOS
float sensorReading = 0; // Sensor value from analog pin "A0"
//int Relay = A1; //relay is connected to
int valSet = 0;
//-------------------Initilization for button-------------------------------------
#define BUTTON_PIN1 2
#define BUTTON_PIN2 3
int bCount = 0;
bool bToggle = 0;
//int temp =0;
float l = 0;
volatile int buttonState = 0;
volatile int buttonState2 = 0;
volatile int prevbuttonState = 0;
//------------------Initialization of clock time--------
int period = 3000;
unsigned long prev_clocktime = 0;
//-------------------Initialization for joystick--------------
int place = 1;
int currentXs= 80; //current xcoordinate start
int currentXe= 97; //current xcoordinate start
int previousXs= 140; //previous xcoordinate start
int previousXe= 157; //previous xcoordinate start
volatile int y = 0;
int a=0;
int b=0;
int c=0;
int d=0;

int xValue = 0 ;
int yValue = 0 ; 

float temp;
int n_array[4] = {0,0,0,0};
void button_press1(){
  /**
  This function is to help read the button press
  */
  if (buttonState == 0){
    buttonState =1;
  }
  else if (buttonState == 1){
    buttonState = 0;
  }
}
void button_press2(){
  /**
  This function is to help read the button press
  */
  if (buttonState2 == 0){
    buttonState2 = 1;
  }
  else if (buttonState2 == 1){
    buttonState2 = 0;
  }
}
//-----------------------PID------------------
#include <PID_v1.h>
double Setpoint ; // will be the desired value
double Input; // photo sensor
double Output ; //LED
double no;
//Define the aggressive and conservative PID Tuning Parameters
//double aggKp=4, aggKi=0.2, aggKd=1;
//double consKp=0.7, consKi=0.1, consKd=0.3;
double aggKp=0, aggKi=0, aggKd=0;
double consKp=0, consKi=0, consKd=0;
//double consKp=1.5, consKi=0.05, consKd=0.25;
//create PID instance 
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);
//------------------------TEMP SENSOR--------------
#include <Adafruit_MAX31865.h>
// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(5, 12, 11,10);
#define RREF      430.0
#define RNOMINAL  100.0
float val;
long previousMillis = 0;
unsigned long currentMillis = 0;

//----------------------SETUP-----------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  delay(15);
  pinMode(CSpin, OUTPUT);
  if (!SD.begin(CSpin)) {
  return;
  }
  //---------for PID--------
  //Turn the PID on
  myPID.SetMode(AUTOMATIC);

  // ------for LCD---------------
  Config_Init();
  LCD_Init();
  LCD_Clear(0xffff);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, GRAY);
  Paint_Clear(GRAY);
  start_screen();
  LCD_ClearWindow(150, 215, 320, 240, GRAY);
  
  //-----------for BUTTON---------
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN1), button_press1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN2), button_press2, FALLING);
  //--------------for Relay ----------------
  //pinMode(Relay, OUTPUT);
  delay(2000);
  prev_clocktime = millis();
  //---------------------TEMP SENSOR-----------
  Serial.begin(115200);
  Serial.println("Adafruit MAX31865 PT100 Sensor Test!");
  thermo.begin(MAX31865_4WIRE);  // set to 2WIRE or 4WIRE as necessary
  thermo.enable50Hz(0);
  //------------------file count read---------- 
  // Read the newest file number from count.txt file.
  countFile = SD.open("count.txt");
  if (countFile){
    while(countFile.available()){
      file_number = countFile.readString();
    }
    countFile.close();
  }
  delay(100);
  int temporary = file_number.toInt();
  // Increase the file_number by on overwrite the calue to count.txt file.
  temporary = temporary+1;
  countFile = SD.open("count.txt", FILE_WRITE | O_TRUNC);
  if (countFile){
    file_number = String(temporary);
    countFile.println(file_number);
    countFile.close();
  }
  // create a new .csv file based on the file_number to store the temperature datas 
  fileNameString = "DATA_"+String(file_number)+".csv";
  header = SD.open(fileNameString, FILE_WRITE);
  if(header){
    header.print("\n");
    header.print("Temp.");
    header.print("\t");
    header.print("Target Temp.");
    header.print("\t");
    header.print("Time(millis)");
    header.close();
  }
  //-----------Read PID parameter from "par.txt" file from SD card-------------
  myFile = SD.open("par.txt");
  if (myFile) {
    //Serial.println("try.txt:");
    readParameters();
  }
  Paint_DrawString_EN(220, 10, "F:(" , &Font20,GRAY, WHITE);
  Paint_DrawNum(258, 10, temporary , &Font20,GRAY, WHITE);
  Paint_DrawString_EN(300, 10, ")" , &Font20,GRAY, WHITE);
}
//-------------------------------------------------------------LOOP------------------------------------------------------------------------------------------------------------
void loop() {
  currentMillis = millis();
  //----temperature sensor-----
  uint16_t rtd = thermo.readRTD();
  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  sensorReading = thermo.temperature(RNOMINAL, RREF);
  // Update the temperature every after 2 seconds. This update only happens when the 
  if (millis() - previousMillis > 2000){
    previousMillis = millis();
    LCD_ClearWindow(100, 90, 300, 120, GRAY);
    Paint_DrawFloatNum(100, 90, sensorReading,1 , &Font24, BROWN , WHITE);
    
  }
  // press the button to set the temperature
  if (buttonState == 1){
    set_temp();
    //Set buttonState2 = 0, i.e; the system in pause state
    buttonState2 = 0;
    while(1){
      LCD_ClearWindow(120, 215, 320, 240, GRAY);
      Paint_DrawString_EN(120, 215, "Pause", &Font20,MAGENTA, WHITE);
      // set the MOS to OFF
      analogWrite(13,LOW);
      // Update the current temperature
      LCD_ClearWindow(100, 90, 300, 120, GRAY);
      Paint_DrawFloatNum(100, 90, sensorReading,1 , &Font24,BROWN, WHITE);
      // check the buttonState and if the buttonState for temperature change do not change then do this
      if (buttonState == 1){
        set_temp();
      }
      // If buttonState2 == 1. Then, come out of the pause state. And the target temperature is already being set.
      if(buttonState2 == 1){
        break;
      }

    }
  }
// If buttonState2 ==  0 then the system is PAUSED  
  if (buttonState2 ==0){
    // set the MOS to OFF
    analogWrite(13,LOW);
    while(1){
      sensorReading = thermo.temperature(RNOMINAL, RREF);
      LCD_ClearWindow(120, 215, 320, 240, GRAY);
      Paint_DrawString_EN(120, 215, "Pause", &Font20,MAGENTA, WHITE);
      
      // Update the current temperature
      LCD_ClearWindow(100, 90, 300, 120, GRAY);
      Paint_DrawFloatNum(100, 90, sensorReading,1 , &Font24,BROWN, WHITE);
      // In this PAUSED state, set the temperature
      if (buttonState == 1){
        set_temp();
      }
      // Come out of the PAUSED state
      if(buttonState2 == 1){
        break;
      }
    }
  }
  //--------------PID control of Relay-----
  Setpoint = l/10;
  delay(50);
  double gap = Setpoint-sensorReading; //Current temperature far from target temperature
  if (gap < 0){
    gap = 0;
  }
  Input = map(sensorReading, 0, 1024, 0, 1024);
  if ((gap < 25) && (gap >1)) 
  {  //Use conservative when the gap is below 25 C but more than 1 C
    myPID.SetTunings(consKp, consKi, consKd);
    myPID.Compute();
    // Map the output value from PID to range 0 - 170
    no = map(Output, 0, 255, 0,170);
    analogWrite(13,no);
    //LCD_ClearWindow(150, 215, 320, 240, GRAY);
    //LCD_ClearWindow(150, 215, 320, 240, GRAY);
    //Paint_DrawString_EN(120, 215, "conservative", &Font16,MAGENTA, GREEN);
  }
  // If gap is smaller than 1 then set the MOS to OFF
  else if (gap<= 1){
    Output =0;
    // In this function Setpoint+3 is to make the prpevious values in PID library zero as fast as possible
    Input = map(Setpoint+3, 0, 1024, 0, 1024);
    no = map(Output, 0, 255, 0,170);
    analogWrite(13,LOW);
    //LCD_ClearWindow(150, 215, 320, 240, GRAY);
    //Paint_DrawString_EN(120, 215, "decreasing", &Font16,MAGENTA, RED);
  }
  else {
     //we're far from target temperature, so use aggressive tuning parameters
     myPID.SetTunings(aggKp, aggKi, aggKd);
     myPID.Compute();
     no = map(Output, 0, 255, 0,170);
     analogWrite(13,no);
     //LCD_ClearWindow(150, 215, 320, 240, GRAY);
     //Paint_DrawString_EN(120, 215, "aggresive", &Font16,MAGENTA, BLUE);
  }
  Serial.println(Output);
  /** If the system is started without setting target temperature then allert to set the temperature and the heating will not be active. This is done by checking valSet. 
  If valSat==0, that means the user has not even set the target temperature once 
  */
  if (valSet == 0){
    buttonState2 = 0;
    while(1){
      LCD_ClearWindow(120, 215, 320, 240, GRAY);
      Paint_DrawString_EN(120, 215, "Set target temp.", &Font16,MAGENTA, WHITE);
      // set the MOS to OFF
      analogWrite(13,LOW);
      // Update the current temperature
      LCD_ClearWindow(100, 90, 300, 120, GRAY);
      Paint_DrawFloatNum(100, 90, sensorReading,1 , &Font24,BROWN, WHITE);
      // set the temperature by pressing the button
      if (buttonState == 1){
        set_temp();
      }
      // Come out of PAUSED after setting temperature
      if(buttonState2 == 1){
        break;
      }
    }
  }
  if (sensorReading < l/10 ){ 
    //write current temp.
    LCD_ClearWindow(100, 90, 190, 110, GRAY);
    Paint_DrawFloatNum(100, 90, sensorReading ,1, &Font24,BROWN, WHITE);
    //write status
    LCD_ClearWindow(150, 215, 320, 240, GRAY);
    Paint_DrawString_EN(120, 215, "Increasing", &Font20,MAGENTA, WHITE);
    
    }
    else if (sensorReading > l/10){
      LCD_ClearWindow(100, 90, 300, 120, GRAY);
      Paint_DrawFloatNum(100, 90, sensorReading,1 , &Font24,BROWN, WHITE);
      LCD_ClearWindow(100, 215, 320, 270, GRAY);
      Paint_DrawString_EN(120, 215, "Decreasing", &Font20,MAGENTA, WHITE);
    }
    else if (sensorReading == l/10){
      LCD_ClearWindow(150, 215, 320, 240, GRAY);
      Paint_DrawString_EN(150, 215, "Equal", &Font20,MAGENTA, WHITE);
    }
    
    // Whenever the buttonState = 0 then write the data into a text file in SD card every after 3 second
    if(buttonState==0){
      if(millis() >= prev_clocktime + period){
        dataString = String(sensorReading); // convert to CSV
        setString = String(temp);
        //Function to write data into txt file
        saveData(); // save to SD card
        prev_clocktime += period;;
      }
    }
}

//-------function to set TEMPERATURE--------------------

void set_temp(){
  /**
  This Function creates an UI to set temperature and also collects input value from the user. Joystick is used set the temeperature. It takes input values "abc.d" 
  Input values are stored in an array "n_array[]" . To better understand how the target temp is calculated assume; a = n_array[0], b= n_array[1], c = n_array[2], d= n_array[3]

  l: l = 1000*a + 100*b + 10*c + d   and to get the actual value in decimal format do "l/10"
  xValue, yValue: x and y coordinates of the joystick. These coordinate values are used to set the target temperature
  place: This value is used to indicate with value(a/b/c/d) can be changed at the moment  
  */

  valSet++;
  set_temp_screen();
    delay(50);
    
    //temp = 0;
    analogWrite(13,LOW);
    Paint_DrawNum(100, 170, n_array[0] , &Font24,BLUE, WHITE); // a
      Paint_DrawNum(120, 170, n_array[1] , &Font24,BLUE, WHITE); // b
      Paint_DrawNum(140, 170, n_array[2] , &Font24,BLUE, WHITE); // c
      Paint_DrawString_EN(153, 170, ".", &Font20,BLUE, WHITE);
      Paint_DrawNum(164, 170, n_array[3] , &Font24,BLUE, WHITE);
    while(true){
      sensorReading = thermo.temperature(RNOMINAL, RREF);
      if (buttonState == 0){
        l = (1000*n_array[0])+(100*n_array[1])+(10*n_array[2])+n_array[3];
        //temp = (100*a)+(10*b)+c;
        temp = l/10;
        break;
      }
      xValue = analogRead(A2);	
      yValue = analogRead(A3);
      // Draw an yellow line for the first time at the place where user input value can be changed using joystick.
      if ((place >= 1) && (place <5)){
        yellow_under(place);
      }
      // Move UP in joystick. Increase the user input value
      if (yValue <270){
        a=n_array[place-1]+1;
        if(a>9){
        a=0;
        }
        if (place == 4){
          Paint_DrawString_EN(153, 170, ".", &Font20,BLUE, WHITE);
          Paint_DrawNum(164, 170, a , &Font24,BLUE, WHITE);
        }
        else{
          Paint_DrawNum(100+(20*place)-20, 170, a , &Font24,BLUE, WHITE);
        }
        n_array[place-1] = a;
        //yellow_under(place);
      }
      // Move DOWN in joystick. Decrease the user input value
      else if (yValue > 750){
        a = n_array[place-1]-1;
        if(a<0){
          a=9;
        }
        if (place == 4){
          Paint_DrawString_EN(153, 170, ".", &Font20,BLUE, WHITE);
          Paint_DrawNum(164, 170, a , &Font24,BLUE, WHITE);
        }
        else{
          Paint_DrawNum(100+(20*place)-20, 170, a , &Font24,BLUE, WHITE);
        }
        n_array[place-1] = a;
      }
      // Move RIGHT in joystick. Move RIGHT in the UI to change the user input value
      if ((xValue>750)){ 
        place = place+1;
        if (place > 4){
          place = 4;
        }
        delay(500);
        
        yellow_under(place);
      }
      //Move LEFT in joystick. Move LEFT in the UI to change the user input value
      else if((xValue<270)){ 
        place = place-1;
        if (place <1){
          place = 1;
        }
        delay(500);
        
        yellow_under(place);
      }
      //Paint_DrawNum(20, 10, place , &Font24,BLUE, WHITE);
      LCD_ClearWindow(120, 215, 320, 240, GRAY);
      Paint_DrawString_EN(120, 215, "Setting temp", &Font20,MAGENTA, WHITE);
      //delay(100);
    }
    Paint_DrawLine(0, 125, 320, 125, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
    Paint_DrawLine(0, 210, 320, 210, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
    //LCD_ClearWindow(0, 155, 320, 208, GRAY);
    //LCD_ClearWindow(0, 0, 320, 122, GRAY);
    //write current temperature
    LCD_ClearWindow(100, 90, 300, 120, GRAY);
    sensorReading = thermo.temperature(RNOMINAL, RREF);
    Paint_DrawFloatNum(100, 90, sensorReading,1 , &Font24,BROWN, WHITE);
    //write set temperature
    LCD_ClearWindow(100, 170, 320, 208, GRAY);
    Paint_DrawFloatNum(100, 170, l/10,1 , &Font24,BLUE, WHITE);
    
    
}

//------------------------function for yellow under line ------------------------
void yellow_under(int place){
  /**
  This function is for visual identification. It draws a yellow line below the user input place where an input can be inserted.

  currentXs: This is a predefined x coordinate to calculate from where the yellow line is to be drawned
  currentXe: This is a predefined x coordinate to calculate untill where the yellow line is to be drawned
  
  Arguments:
    place: This is used to indicate which value user can give input 
  */

  int xs = currentXs+(20*place);
  int xe = currentXe + (20*place);
  LCD_ClearWindow(97, 195, 320, 205, GRAY);
  Paint_DrawLine(xs, 200, xe, 200, YELLOW,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);

}
//------------------------function for START SCREEN ------------------------
void start_screen(){
  /*
  This function creates the starting UI 
  */
  Paint_DrawString_EN(20, 10, "TEMP. in 'C ", &Font20,GRAY, WHITE);
  Paint_DrawLine(0, 40, 320, 40, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 60, "Current temp:", &Font20,GRAY, WHITE);
  Paint_DrawLine(0, 125, 320, 125, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 140, "Set temp:", &Font20,GRAY, WHITE);
  Paint_DrawLine(0, 200, 320, 200, BLACK,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 215, "Status:", &Font20,GRAY, WHITE);
}
//----------------------function SET TEMP. SCREEN----------------------------
void set_temp_screen(){
  /*
  This function creates the UI for setting temperature
  */
  LCD_ClearWindow(0, 155, 320, 208, GRAY);
  Paint_DrawLine(0, 125, 320, 125, GREEN,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 140, "Set temp:", &Font20,GRAY, WHITE);
  //-----window for STATUS
  LCD_ClearWindow(0, 195, 320, 205, GRAY);
  Paint_DrawLine(0, 210, 320, 210, GREEN,   DOT_PIXEL_2X2,LINE_STYLE_SOLID);
  Paint_DrawString_EN(20, 215, "Status:", &Font20,GRAY, WHITE);
  // writing "Setting Temp....." in the status of the screen
  LCD_ClearWindow(120, 215, 320, 240, GRAY);
  Paint_DrawString_EN(120, 215, "Setting", &Font20,MAGENTA, WHITE);
}

//-------------------------function to write data into SD Card--------------------------
void saveData(){
  /*
  This function saves the current temperature, target temperature and time using millis() (but not the actual time) in a txt file in the SD card.
  */
  
  sensorData = SD.open(fileNameString, FILE_WRITE);
  if (sensorData){
    sensorData.print("\n");
    sensorData.print(dataString);
    sensorData.print("\t \t");
    sensorData.print(setString);
    sensorData.print("\t \t");
    sensorData.print(currentMillis);
    //sensorData.print("\t");
   // sensorData.print(file_number);
    sensorData.close(); // close the file
    }
}

//-------------------Read parameters from txt file----------------------
void readParameters(){
      String cKp = myFile.readStringUntil('\n');
      // Read the third line
      String cKi = myFile.readStringUntil('\n');
      String cKd = myFile.readStringUntil('\n');

      String aKp = myFile.readStringUntil('\n');
      String aKi = myFile.readStringUntil('\n');
      String aKd = myFile.readStringUntil('\n');
      

      // Convert the third line to an integer
      cKp = &cKp[9];
      consKp = cKp.toDouble();
      cKi = &cKi[9];
      consKi = cKi.toDouble();
      cKd = &cKd[9];
      consKd = cKd.toDouble();

      aKp = &aKp[8];
      aggKp = aKp.toDouble();
      aKi = &aKi[8];
      aggKi = aKi.toDouble();
      aKd = &aKd[8];
      aggKd = aKd.toDouble();

      Serial.print("cKp: ");
      Serial.println(consKp);
      Serial.print("cKi: ");
      Serial.println(consKi);
      Serial.print("cKd: ");
      Serial.println(consKd);
      Serial.print("aKp: ");
      Serial.println(aggKp);
      Serial.print("aKi: ");
      Serial.println(aggKi);
      Serial.print("aKd: ");
      Serial.println(aggKd);
      myFile.close();
}
