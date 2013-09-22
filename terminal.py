## terminal.py
##
##    Python serial terminal to read the data stream from
##    and arduino running the hallt_sensor_test.ino scketch
##
##    Created September 8, 2013
##    by David Lister
##
#

# Imports

import serial


# Symbilic Constants

COM_PORT = 'COM8'  # Com port the Ardiono is connected to
BAUD_RATE = 19200


# Classes

class DataStream:

    def __init__(self, rawdata):
        self.data = rawdata
        self.packets = []
        
        # Remove leading ! and trailing ;
        tempdata = self.data[1:-1]

        # Seperate packets
        packets = tempdata.split(',')

        for packet in packets:
            self.packets.append(Packet(packet))

    def __str__(self):
        for packet in self.packets:
            print packet,

        return ' '


class Packet:

    def __init__(self, raw_packet):
        self.packet = raw_packet

        # Split the packet
        split_packet = raw_packet.split(':')

        # Store the name and value seperately
        self.name = split_packet[0]
        self.value = split_packet[-1]  ## fix


    def __str__(self):
        return self.packet

        


# Functions

def process(data_stream):
    print data_stream


# Setup
port = serial.Serial(COM_PORT, baudrate = BAUD_RATE)
data = ''


# Main loop

while True:
    if port.inWaiting():
        data += str(port.read())

        # Check for the end of a data stream
        if data[-1] == ';':
            process(DataStream(data))
            data = ''


