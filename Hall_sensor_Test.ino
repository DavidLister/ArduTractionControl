/* 
Hall Sensor Test

Reads values from a latching hall sensor to measure the RPM and
surface speed of a wheel and prints it over serical.

The format of the serial data is as follows:
  - The data is transmitted in ASCII text
  - The data stream starts with an exclamation mark (!)
  - The data steam ends with a semi colon (;)
  - A data packet is presented with the variable name 
    in CAPS followed by a collon and it's value
  - There is a comma between each data packet
  - The packet is a checksum of the sum of all previous
    data values with decimals floored mod 16
    
  Example:
  !RPM:500,SPEED:7.5,CHECK:11;
  

Created September 7, 2013
by David Lister

Thie code is in the public domain

*/

#include "data.h"



// Symbolic Constants

#define WHEEL_DIAMETER 0.1  // Diameter of the wheel in meters

#define SWITCHES_PER_ROTATION 8 // Number of times the hall sensors
        // will switch it's latch per rotation
        
#define HALL_SENSOR_PIN 2 // Arduino pin with the hall sensor. If 
        // the hall sensor pin is changed, ensure the interrupt 
        // pin value is changed.
                                
#define BUFFER_SIZE 5 // Number most recent values to keep in buffer 
        // for calculating RPM and speed
                      
#define SERIAL_UPDATE_INTERVAL 100 // interval in miliseconds the
        // RPM and surface speed of the wheel is sent over serial

#define SERIAL_BAUD_RATE 19200 // Baud rate for serial comunication

#define SERIAL_GREETING "!START:1,CHECK:1;" // String sent over serial upon 
        // startup, this is to alert a main controller the Arduino 
        // has started and which position it's in.


// Global Variables

Data stream;
int hall_sensor = int(HALL_SENSOR_PIN);
int LatchState = 0;
Buffer buffer(BUFFER_SIZE);
unsigned long last_update = micros();
int led = 13;
unsigned long blinkTime = millis();
int iter = 0;

// Setup

void setup(){
 // Setup serial comunications
 Serial.begin(SERIAL_BAUD_RATE);
 Serial.print(SERIAL_GREETING);
 pinMode(led, OUTPUT);     
 
 
 // Setup digital input for hall sensor
 pinMode(HALL_SENSOR_PIN, INPUT);
 LatchState = digitalRead(hall_sensor);
 
 // Attach interrupts
 attachInterrupt(0, latch, CHANGE); // If the hall sensor pin is changed,
        // ensure this pin value is changed.
  
}



// Main loop

void loop(){
  // Blink LED
  /*
  if(millis() - blinkTime < 1000){
    digitalWrite(led, HIGH);
  }
  
  else if(millis() - blinkTime > 1000){
    digitalWrite(led, LOW);
  }
  
  if(millis() - blinkTime > 2000){
    digitalWrite(led, LOW);
    blinkTime = millis();
  }
  
  // Ensure a latch hasn't been missed
  if (micros() % 4 == 0){
    latch();
  }
  */
  
  // Has the serial update interval elapsed?
  if (millis() - last_update > SERIAL_UPDATE_INTERVAL){
    unsigned long average_interval = buffer.average_interval();
    unsigned long instant_interval = buffer.instant_interval();
    
    // Send Data
    stream.payloadClear();
    stream.append(Packet(String("INST_RPM"), RPM(instant_interval)));
    stream.append(Packet(String("AVG_RPM"), RPM(average_interval)));
    stream.append(Packet(String("INST_SPEED"), surface_speed(instant_interval)));
    stream.append(Packet(String("AVG_SPEED"), surface_speed(average_interval)));
    stream.append(Packet(String("AVG_INTERVAL"), average_interval));
    stream.append(Packet(String("INST_INTERVAL"), instant_interval));
    stream.transmit();
    last_update = millis();
  }
  
}


// Function to process changes in the hall sensor latching

void latch(){
  buffer.append();
  LatchState = digitalRead(hall_sensor);
  iter += 1;
  if(iter == 2){
    digitalWrite(led, LOW);
    iter = 0;
  }
  
  else{
    digitalWrite(led, HIGH);
  }
  
}

// Calculates RPM from the length of an interval in microseconds
float RPM(unsigned long interval){
  // Convert the interval into seconds, find the seconds/rotation, then convert to RPS then to RPM
  float out = 1/(0.001 * interval * SWITCHES_PER_ROTATION) * 60;
  return out;
}

// Calculates surface speed in m/s based off the length of an interval in microseconds
float surface_speed(unsigned long interval){
  float rpm = RPM(interval);
  float rps = rpm/60;
  
  float circumfrence = WHEEL_DIAMETER * 3.1415926535898;
  
  float out = rps * circumfrence;
  
  return out;
}
