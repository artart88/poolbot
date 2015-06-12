//Pool Control Program		Art Balourdas
//POOL_06_11_2015_a

/* Arduino Interface to the PSC05 X10 Receiver.                       BroHogan 3/24/09
 * SETUP: X10 PSC05/TW523 RJ11 to Arduino (timing for 60Hz)
 * - RJ11 pin 1 (BLK) -> Pin 2 (Interrupt 0) = Zero Crossing
 * - RJ11 pin 2 (RED) -> GND
 * - RJ11 pin 3 (GRN) -> Pin 4 = Arduino receive
 * - RJ11 pin 4 (YEL) -> Pin 13 = Arduino transmit (via X10 Lib)
 * NOTES:
 * - Must detach interrup when transmitting with X10 Lib 
 
 FLAGS
 * hs_pump_flag   //flag from homeseer to be used to test if HS turned on the pump. if yes do not turn it off.
 * pump_on_flag	//internal
 * spa_on_flag	//internal
 
 SEND X10 FLAGS to HS
 * D9 pump on/off
 * D10 spa mode on/off
 
 RECEIVE X10 from HS or X10 Contolers
 * P1 pump on/off
 * P2 spa mode on/off (valves and heater)
 
 */

#include "Arduino.h"
#include <x10.h>                       // X10 lib is used for transmitting X10
#include <x10constants.h>              // X10 Lib constants
/* 
|| | matrix keypad routine
*/
#include <Keypad.h>
#define RPT_SEND 2 

#define ZCROSS_PIN     2               // BLK pin 1 of PSC05
#define RCVE_PIN       13               // GRN pin 3 of PSC05
#define TRANS_PIN      4               // YEL pin 4 of PSC05
#define LED_PIN        99              // for testing 

x10 SX10= x10(ZCROSS_PIN,TRANS_PIN,RCVE_PIN,LED_PIN);  // set up a x10 library instance:

//define # of rows, columns and layout

const byte ROWS = 2; //two rows
const byte COLS = 2; //two columns
char keys[ROWS][COLS] = {
  {'1','2'},      //pump off, pump on
  {'3','4'},      //spa off, spa on
//  {'5','6'},
//  {'7','8'},
//  {'9','10'},
                        };
//byte rowPins[ROWS] = {9, 10, 11, 12}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {9, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//define relay output pins

#define ledpin5 5  //pump
#define ledpin6 6  //spa valves 
#define ledpin22 22 // heater

boolean pump_on_flag = false;
boolean spa_on_flag = false;


void setup() {

// relay board pins setup  
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(22, OUTPUT);

  digitalWrite(ledpin5, LOW);
  digitalWrite(ledpin6, LOW);
  digitalWrite(ledpin22, LOW);
  
  Serial.begin(9600);
  delay(500);
  Serial.print("POOL controler test");
}

// main loop -----------------------

void loop(){

//--------matrix button processing----------
 
  char key = keypad.getKey();
  Serial.println("get key");
  if(key)  // same as if(key != NO_KEY)
  {
    switch (key)
    {

//PUMP     
      case '1': //Pump OFF 
        if (spa_on_flag == false)
        Serial.println("case 1 - Pump OFF");
        pump_off();
        break;
        
      case '2':  //Pump ON 
        Serial.println("case 2 - Pump ON");
        pump_on();
        break;

//SPA MODE
      case '3': //Spa Mode OFF - valves and heater off
        Serial.println("case 3 - Spa OFF"); 
        spa_off();
        //****pump off routine here with check flag and time delay off*****
        break;
        
      case '4': //Spa Mode ON - valves, heater and pump on
        Serial.println("case 4 - Spa ON");
        pump_on();
        spa_on();
        break;

/*      
      case '5': // Relay 3 Off for future use
      digitalWrite(ledpin7, LOW);

      case '6': // Relay 3 on for future use
	digitalWrite(ledpin7, HIGH);

      case '7': // Relay 4 Off for future use
	digitalWrite(ledpin8, LOW);

      case '8': // Relay 4 on for future use
	digitalWrite(ledpin8, HIGH);

      case '9': // Relay 5 Off for future use
	digitalWrite(ledpin9, LOW);

      case '10': // Relay 5 on for future use
	digitalWrite(ledpin9, HIGH);
*/
      default:
        Serial.println(key);
    }
  }
 
 //--------------------------------------------------------------------
 //        X10 received input processing
 //--------------------------------------------------------------------

//   Serial.println("aaaa");
   if (SX10.received()) {                        
//   Serial.println("bbbb   ");            // received a new command
 	SX10.debug();                       // print out the received command
	SX10.reset();

/*
//USE THIS CODE IF HS IS TURNING THE PUMP ON AND OFF INSTEAD OF THE PUMP'S BUILT-IN TIMER
//if P11 was received then set the HB pump flag to TRUE
     if (SX10.unitCode() == 11)
{
      if (SX10.houseCode() == 'P')
//  Serial.println("received P11 (pump on notification) from HS");
    {  
	hb_pump_flag = true;
*/

//-----if P1 is received then turn pump on or off----- 

     if (SX10.unitCode() == 1);
{      
       if (SX10.houseCode() == 'P');
      {
        delay(300);
        byte cmndCode = SX10.cmndCode();
        if(cmndCode == ON) pump_on() ; 
        if(cmndCode == OFF) pump_off() ;      
 //     if(cmndCode == ON) SX10.write(HOUSE_M,ON,RPT_SEND);  
 //     if(cmndCode == OFF) SX10.write(HOUSE_M,OFF,RPT_SEND);
      SX10.reset();
//      SX10.write(HOUSE_M,UNIT_6,RPT_SEND);
//      SX10.write(HOUSE_M,ON,RPT_SEND);  
       }  
 }

//-----if P2 is received then turn spa mode on or off-----

     if (SX10.unitCode() == 2);
{      
       if (SX10.houseCode() == 'P');
 {
        delay(300);
        byte cmndCode = SX10.cmndCode();
        if(cmndCode == ON) spa_on() ; 
        if(cmndCode == OFF) spa_off() ;      
 //     if(cmndCode == ON) SX10.write(HOUSE_M,ON,RPT_SEND);  
 //     if(cmndCode == OFF) SX10.write(HOUSE_M,OFF,RPT_SEND);
      SX10.reset();
//      SX10.write(HOUSE_M,UNIT_6,RPT_SEND);
//      SX10.write(HOUSE_M,ON,RPT_SEND);  
		}  
  	   }
 	}
    }
  } 
  
//------------------------------------------
//    Proceedures
//------------------------------------------


void pump_on()
{
        // **** on by relay
       pump_on_flag = true;
       digitalWrite(ledpin5, HIGH); 
       Serial.println("pump relay on"); 	  
       // **** on by x10
       // SX10.write(HOUSE_D,UNIT_1,2);  // 
        // send a "on" command 3 times:
        // SX10.write(HOUSE_D,ON,3);  //
	//set pump flag P9 on
	SX10.write(HOUSE_P,UNIT_9,2);  // 
	SX10.write(HOUSE_P,ON,3);  // 
        Serial.println("pump x10 flag P9 on");
*/        
}

void pump_off()
{
        pump_on_flag = false;
        digitalWrite(ledpin5, LOW);  
        Serial.println("pump relay off");   // off by relay
        // off by x10	  
//        SX10.write(HOUSE_D,UNIT_1,2);  // 
        // send an "off" command 3 times:
 //       SX10.write(HOUSE_D,OFF,3);  // 
        //set pump flag P9 off
	SX10.write(HOUSE_P,UNIT_9,2);  // 
	SX10.write(HOUSE_P,OFF,3);  // 
	Serial.println("pump x10 flag P9 off");
}

void spa_on()
{
      // Spa Mode ON Relay
         spa_on_flag = true;
	 digitalWrite(ledpin6, HIGH);
        Serial.println("spa valves relay on");
       // Heater ON Relay
          digitalWrite(ledpin22, HIGH);
        Serial.println("heater relay on");

      // Spa Mode ON D2 (valves and heater) x10
	 // SX10.write(HOUSE_D,UNIT_2,2); // Send D2
        // send a "on" command 3 times:
        //  SX10.write(HOUSE_D,ON,3);   // D2 ON
        //
        //set pump flag D10 on
	SX10.write(HOUSE_D,UNIT_10,2);  // Send D10
	SX10.write(HOUSE_D,ON,3);  //  Send D10 ON Spa Flag
        Serial.println("spa x10 flag D10 on");
}

void spa_off()
{
// Spa Mode OFF Relay
	spa_on_flag = false;
	digitalWrite(ledpin6, LOW);
        Serial.println("spa off relay");
        // Heater OFF Relay
        digitalWrite(ledpin22, LOW);
        Serial.println("heater off relay");
        
        // Spa Mode OFF D2 (valves and heater)
        // SX10.write(HOUSE_D,UNIT_2,2);  // Send D2
        // send a "off" command 3 times:
       // SX10.write(HOUSE_D,OFF,3);  // Send D2 OFF      
       // set spa x10 flag D10 off
        SX10.write(HOUSE_D,UNIT_10,2);  // Send D10
	SX10.write(HOUSE_D,OFF,3);  // Send D10 OFF Spa Flag
        Serial.println("spa x10 flag D10 off");


	
// FOR NOW DO NOT TURN PUMP OFF HERE        
// ****NEED CHECK FOR PUMP FLAGS AND PUMP SHUT OFF DELAY HERE
//if the pump was turned on by HB then do not turn it off
//otherwise delay 3 minutes and then shut off pump
/*	if (hb_pump_flag);
        // Pump OFF D1
	//****add 3 min delay here
{	  delay (500);	
	  pump_off();
    }
*/
}
