#ifndef CG_H
#define CG_H

/*
 * CG - Character Generator
 */

#include <stdint.h>

// Possible characters, present in the cg table.
extern const char cgIndex[];
extern const uint8_t cgIndexLen;
// Table containing the representation of characters.
extern const char cgTab[][40];

// Gets the index of a character's representation
// in memory, from the cg table (cgTab).
uint8_t cgGetIdx(uint8_t c);


#endif
