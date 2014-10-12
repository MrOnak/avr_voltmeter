#ifndef CHARS_H
#define CHARS_H

/**
 * Use the settings for vMaxReal and vMaxIn to 'tune'
 * the display to the correct value.
 *
 * I.e. if you have a vMaxReal of 12V and you're reducing
 * that via a voltage divider to 4.75V then your vMaxIn is
 * equal to 4750.0.
 *
 * This ensures the display shows the correct value.
 */
// IN MILLIVOLTS: your 'real' maximum voltage that will be measured 
#ifndef vMaxReal
float vMaxReal = 24000.0;
#endif
// IN MILLIVOLTS: the maximum voltage that the microcontroller will see
#ifndef vMaxIn
float vMaxIn = 4898.0;
#endif

float refDiv;

#endif // CHARS_H