# Auto-Blinds
I like to consider myself a proactive lazy person in that I will often put so much more effort into a project then it is worth if it 
means I can be sightly more lazy because of it. This project is a great example of this exact characteristic.

So here is the issue: my desk is located directly in front of a window and so around mid-day I have to get up and close the blinds to stop 
the sun from shining into my eyes which often interrupts my workflow. Some might say this is a nonissue and that I "should just keep them 
closed", but then what is the point of a window? Might as well be a brick wall. And so I decided to tackle this issue with an arduino, a 
stepper motor and driver, a photoresistor, and an IR reciver and transmitter.

And with the minimal tools that I had, and no access to a 3D printer, this is what I came up with.

### Hardware:
  * The stepper motor was attached using hot glue to the tilt mechanism of the blinds and then nailed to the spine of a binder which was 
  used as a wedge to be fixed in placed between a wall and a chest of drawers right next to the window.
  * The photoresister was attached to a 220 ohm resistor creating a voltage divider for light intensity inputs. This was then wired up and 
  placed in the middle of the windowsill.
  * The IR reciver was scrapped from old christmas lights and wired up with the arduino for reciving IR transmissions from the remote 
  that came with the lights. 

### Software/ Capabilites:
I have set up two modes for the blinds which is toggles using the IR remote.
  1. *Automatic*: This mode is controlled entirely by the intensity of the light coming through the windows. It will close the blinds if 
  the light is too intense and if it is dark out. Consequently it also opens by itself in the morning and when it is dawn when the light 
  isn't as intense. The light intensity is calculated over 10 seconds to get an accurate measurement, and the blinds will only move if 
  the threshold is met more than a certain amount of times (this is to pervent it from opening and closing every second because of a 
  couple clouds).
  2. *Manual*: This mode is controled by the IR remote with the following 4 options: tilt up, tilt down, close blinds (tilt all the way 
  up), and open blinds.
  
The IR remote sends signals to the arduino through an interrupt, this suspends the current execution and updates that program counter 
to be the start of a interrupt service routine which alters some flags depending on the IR signal sent.
  
  
