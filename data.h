/* 
  data.h

  Library containing the buffer, data stream class and
  the data packet class for the hall_sensor_test sketch.
  
  These are used to send data over serial.

  Created September 7, 2013
  by David Llster
  
  This code is in the public domain.

*/


/* 
  - Packet class - 

Contains a data packet object with it's name and value
*/

#ifndef data_h
#define data_h

#include "Arduino.h"

class Buffer
{
  public:
    Buffer(int depth_in);
    void append();
    unsigned long average_interval();
    unsigned long instant_interval();
    
  private:
    int _depth;
    unsigned long _values[100];
};


class Packet
{
  public:
    Packet(String name_in, float value_in);
    String name;
    float value;
};


/*
  - data class - 

- Contains the data steam to be transmitted via serial.
- Has a transmist function to send data over serial.
*/
class Data
{
  public:
    Data();
    void append(Packet packet);
    void transmit();
    void payloadClear();
    
  private:
    float _check_tally;
    String _payload;
};


#endif
