Temp Control with PT100 For Heating Stage:

	In this temperature control system, we are using: Arduino Mega, LCD screen, Joy-stick, SD card, SD card module, MOSFET, 2 push buttons, MAX31865, and a power supply.
	
	Arduino Mega: 		It is used because the program size was too large to load it into the Arduino UNO.
	LCD screen: 		  This screen is used for Graphical User Interface (GUI). Here we can see which state the machine is right now, the current temperature in C, the target temperature, and the log file index number.
	Joy-stick: 		    It is used to control the GUI for changing temperature. (to change the target temperature)
	SD card: 		      It is used to store the temperature data into a CSV file. Every time the Arduino system is switched ON or restarted a new CSV file will be created. The index number of this file is displayed in the top right of the LCD screen. There is also a counter.txt file where the index of the file is updated. File "par.txt" has the parameters for PID control. 
	SD card module: 	It is used to read and write the Sd card.
	MOSFET: 		      It is used to control the voltage supply to the heating stage.
	2 push buttons: 	One button is used for setting the temperature and the other is to pause and start the system.
	MAX31865:		      It is used to read the resistance value of PT100.

Libraries used:

	- SD by Arduino, SparkFun
	- LCD 2-inch display from waveshare. Library link: https://www.waveshare.com/w/upload/e/e9/LCD_Module_code.7z      
		Documentation link: https://www.waveshare.com/wiki/2inch_LCD_Module?Amazon
	- Adafruit MAX31865 library by Adafruit
	- PID by Brett Beauregard

How to use:

	- When power is supplied to the Arduino and after the LCD screen is active the state will be in pause state. 
	- Use the "SET" button to set the target temperature.
	- Use Joy-stick to move around and increase or decrease the target temperature.
	- After setting the temperature press the "PAUSE" button to start the system.
	- Use the "PAUSE" button in case you want to stop the system.
	- And whenever the target temperature is needed to be changed, press the "SET" button and then "PAUSE" to start the system again.

Update PID parameter:
  - To Update the PID parameters use the file "par.txt" file. 
  - For example: "AggKp = 4"  -> change only the value 4 with the value that is you want to add. It is important to have a space after the "=". 
  - ***** Do not change anything else in the file.
