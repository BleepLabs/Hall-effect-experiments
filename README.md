Using two hall effect sensors and a magnet on a pendulum to control a synth and send MIDI

The circuit is simply two DRV5053VAQLPG sensors read by a Teesny 3.2
Their outputs go through a 220 ohm resistor and 2.2u cap low pass filter. This helps for the midi trigger mode but the synth mode works fine without them.

The code uses the teensy audio library to for the synthesis which is two fixed oscillators going through filters controlled by the hall sensors. Pots control the offset and range of this modulation. 
At the same time MIDI notes are sent when the pendulum crosses a threshold. MIDI velocity data is sent based on a crude speed reading. 
