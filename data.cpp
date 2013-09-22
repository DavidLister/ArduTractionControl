/* 
  data.cpp

  Library containing the buffer, data stream class and
  the data packet class for the hall_sensor_test sketch.
  
  These are used to send data over serial.

  Created September 7, 2013
  by David Llster
  
  This code is in the public domain.

*/


#include "Arduino.h"
#include "data.h"

#define PRECISION  4  // Precision of the float in numbers 
        // after the decimal point


/* - Buffer Class -

Contains a buffer of the latest n time values 
from the hall sensor latches
*/
Buffer::Buffer(int depth_in){
  _depth = depth_in;
  
  // Fill buffer with values
  for (int i = 0; i < _depth; i++){
    _values[i] = 0;
  }
}
  
void Buffer::append(){
  unsigned long next;
  unsigned long last = millis();
  
  // Shift all values back one
  for (int i = 0; i< _depth; i++){
    next = _values[i];
    _values[i] = last;
    last = next; 
  }
}

// Returns the average interval between the stored values
unsigned long Buffer::average_interval(){
  unsigned long tally = 0;
  unsigned long current;
  
  // Dissable interupts
  noInterrupts();
  
  // Calculate the average interval
  current = _values[_depth - 1];
  
  for (int i = _depth - 2; i > 0; i--){
    tally += _values[i] - current;
    current = _values[i];
  }
  interrupts();
  
  unsigned long out = tally / (_depth - 1);
  return  out;
}

// Returns the latest interval between the stored values
unsigned long Buffer::instant_interval(){
  unsigned long tally;
  
  // Dissable interupts
  noInterrupts();
  tally = _values[0] - _values[1];
  interrupts();
  
  unsigned long out = tally / (_depth - 1);
  return  out;
}

/* 
  - Packet class - 

Contains a data packet object with it's name and value
*/

Packet::Packet(String name_in, float value_in){
  name = name_in;
  value = value_in;
}



/*
  - data class - 

- Contains the data steam to be transmitted via serial.
- Has a transmist function to send data over serial.
*/

Data::Data(){
  _payload = String('!');
  _check_tally = 0;
}

void Data::payloadClear(){
  _payload = String('!');
  _check_tally = 0;  
}

/* Append function to append a packet to the data stream */
void Data::append(Packet packet){
  // Check to see if a comma needs to be added
  if (! _payload.endsWith(String("!"))){
    _payload += String(',');
  }
  
  // Append packet to data steam
  _payload += packet.name;
  _payload += String(':');
  
  // Append the int part of the float
  _payload += String(int(packet.value));
  
  // Append the decimal point
  _payload += String('.');
  
  // Append the decimal portion of the float to specified precition
  if(packet.value >= 0)
    _payload += int((packet.value - int(packet.value)) * pow(10, PRECISION));
  else
    _payload += int((int(packet.value)- packet.value ) * pow(10, PRECISION));
  
  // Tally up the values for the checksum
  _check_tally += packet.value;
}


void Data::transmit(){
  // Calculate Checksum  
  int checksum = int(_check_tally) % 16;
  
  // Append CHECK packet to payload
  _payload += ",CHECK";
  _payload += checksum;
  
  // Append Semicolon to payload
  _payload += ";";
  
  // Send payload via serial
  Serial.print(_payload);  
}

