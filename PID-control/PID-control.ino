
#include <PID_v1.h>

double Setpoint ; // will be the desired value
double Input; // photo sensor
double Output ; //LED
//PID parameters
double Kp=0, Ki=10, Kd=0; 
 
//create PID instance 
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
 
void setup()
{
  
  Serial.begin(9600);   
  //Hardcode the brigdness value
  Setpoint = 350;
  //Turn the PID on
  myPID.SetMode(AUTOMATIC);
  //Adjust PID values
  myPID.SetTunings(Kp, Ki, Kd);
}
 
void loop()
{
  Input = map(analogRead(A0), 0, 1024, 0, 1024);  
  //PID calculation
  myPID.Compute();
  //Write the output as calculated by the PID function
  analogWrite(A1,Output); 
  //Send data by serial for plotting 
  /*
  Serial.print(Input);
  Serial.print(" ");
  Serial.println(Output);
  Serial.print(" ");  
  Serial.println(Setpoint);*/
  Serial.println(analogRead(A0));
//  delay(100); 
}