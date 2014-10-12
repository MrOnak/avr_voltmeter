/**
 * A digital voltmeter based on an analogue input signal.
 *
 * If your measuring voltages exceed 5V make sure that you use
 * a voltage divider to reduce Vmax to something below 5V.
 *
 * The voltage is displayed on three 7-segment displays, where
 * the rightmost display is a decimal. Thus, this voltmeter can
 * display voltages up to 99.9 with 0.1V resolution.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "config.h"
 
volatile uint8_t digit;
static volatile uint16_t voltage;

void init() {
	digit	= 0;
	voltage	= 0;
	// '100.0' to get the value to 10ths of volts for the display
	refDiv  = 100.0 * (1023.0 / (vMaxReal * (vMaxReal / vMaxIn)));
	
	// display
	DDRD = (1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3);	// outputs to the 7447 IC
	DDRB = (1 << PB4) | (1 << PB3) | (1 << PB2); 				// set all digit pins as outputs
	
	// multiplexing timer 
	TCCR0 |= (1<<CS01) | (1<<CS00);								// timer/counter control register 0: clock select 64th frequency
	TIMSK |= (1<<TOIE0);										// timer/counter interrupt mask register: timer/counter 0 overflow interrupt enable

	// analog-digital converter
	ADMUX = 0x00; 												// select channel ADC0
	ADMUX |= (1 << REFS0);										// voltage reference to AREF pin
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);		// pre-scaling to 128 division factor
	ADCSRA |= (1 << ADFR);										// free-running mode
	ADCSRA |= (1 << ADEN);										// enable the ADC
	ADCSRA |= (1 << ADIE);										// enable ADC interrupt
	ADCSRA |= (1 << ADSC);										// start ADC conversions 
	
	sei();
}
 
int main() {
	init();
	
	while(1) {}
	
	return 0;
}

/**
 * Analog-Digital converter interrupt. 
 *
 * Used to read in the adjustable voltage
 */
ISR(ADC_vect) {
	static uint32_t cumulative;
	static uint16_t lastVoltage;
	static uint8_t i;
	uint8_t lo, hi;
	uint16_t ADCval;
	
	uint8_t cSREG = SREG;					// back up the interrupt register
	cli();									// disable interrupts for the time being
	
	// read ADC value
	lo = ADCL; hi = ADCH;
	ADCval = (hi<<8) | lo;					// combine ADC 
	
	// only a minor difference in voltage
	cumulative += ADCval; 		// add ADC value to cumulative
	
	if (i == 49) {
		// reached accumulation
		// assign average of accumulation to voltage
		voltage		= (cumulative / 50)
						/ refDiv;			// convert 10bit sampling to reference voltage
		cumulative	= 0;					// reset accumulation
		i = 0;								// reset counter
	} else {
		// keep accumulating
		i++;
	}
	
	lastVoltage = ADCval;						// remember current voltage
	
	// restore interrupt register
	SREG = cSREG;
}

/**
 * ISR to multiplex the 7-segment displays
 */
ISR(TIMER0_OVF_vect) {
	uint8_t cSREG = SREG; 	// store register
	cli();             		// disable interrupts

	// switch all digits off
	PORTB &= ~(1 << PB2) & ~(1 << PB3) & ~(1 << PB4);
	
	// light right segments and switch the right digit back on
	switch (digit) {
		case 0:
			PORTD = voltage%10;			
			PORTB |= (1 << PB2);
			digit = 1;
			break;
		case 1:
			PORTD = (voltage/10)%10;
			PORTB |= (1 << PB3);
			digit = 2;
			break;
		case 2: 
			PORTD = (voltage/100)%10;
			PORTB |= (1 << PB4);
			digit = 0;
			break;
	}	

	SREG = cSREG;      // restore interrupt setting
}
