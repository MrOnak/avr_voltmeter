# Voltage Display

This project uses an ATMega8 to display nearly arbitary voltages.

I'm personally using it as display inside a work bench power supply made out
of an old ATX computer power supply where one of the 12V rails is run through
a poti to provide a variable voltage output.

## Technical Outline
I've split the circuit up into two PCBs which are connected through stacking
headers in order to save space in one dimension. 

### Voltage Measurement
The ADC0 channel is used to read the input voltage.

Since any AVR pin can only cope with 5V a simple voltage divider is used to
reduce the voltage to 5V. 

In my case I want to be able to display voltages up to 24V so a 3.9k + 1k
Ohm voltage divider is used. This results in 4.898V on the ADC pin.
For fine-tuning I've put an additional 500 Ohm trimmer in front of the 3.8k
resistor. 

The voltage is read via `ISR(ADC_vect)`. The interrupt routine is written in 
such a way that it will read the voltage on ADC0 fifty times, then calculate
the average of the accumulated readouts and provide the average of that
as voltage variable. 

### 7-Segment Displays
Three 7-segment displays to display the voltage are used. The rightmost
display is used to display tenths of volts, whereas the two leftmost displays
show whole volts.

The decimal dot pin of display 2 is hardwired to ground, which lights it up.

A 7447 IC is used to drive the 7 segments of the individual displays since
the displays I have are common anode ones. If you happen to have super-bright
common cathode displays you can very likely drive them directly from the 
ATMega8.

The displays are driven by multiplexing them through three 2n3904 NPN 
transistors. The multiplexing is done by a counter0 overflow interrupt routine
(`ISR(TIMER0_OVF_vect)`), nothing really spectacular there.

## Configuration
In `config.h` you can adjust the code to work with other maximum voltages. 

All you have to do is 
- change `vMaxReal` to the max. voltage that you're expecting on the VSENS pin
- change `vMaxIn` to the max. voltage on the ADC pin, behind your voltage
divider.

Also, make sure your voltage divider is adapted to convert your maximum VSENS
voltage to no more than 5V!

By the way, the ATmega8 in use runs at 8Mhz, you might want to set your fuses
so that your chip runs at the same frequency, I've had some flicker issues 
on the default 1Mhz.

## License
This code is released under the GNU General Public License, version 3. See 
the License.txt file for details.