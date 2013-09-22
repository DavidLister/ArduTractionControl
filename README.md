ArduTractionControl
===================

Arduino traction control for RC vehicles with independantly driven wheels 

This is a long term project at not near completition.

The ultimate goal is to have a traction control system where an ARM mini computers controls multiple motors via serial to steer an RC vehicle.  Each wheel is to have it's own dedicated Arduino monitering and setting the speed.

Current status:
  Hall sensor interface with the arduino and a serial data stream works.
  
To Do:
  - Control motor speed with PWM
  - Add PID loop to set wheel speed
  - Have the target speed set via serial commands
  - Write controlling software that interfaces multiple Arduinos with an ARM mini computer
  - Test, test, and test again
