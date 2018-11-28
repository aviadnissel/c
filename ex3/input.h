/**
 * @file input.h
 * @author Aviad Nissel <aviad.nissel@mail.huji.ac.il>
 *
 * Header for the input data structure.
 */
#ifndef INPUT_H
#define INPUT_H


/* --- Includes --- */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "stack.h"
#include "calculatorUtils.h"


/* --- Constants --- */

/** A number Input. */
#define NUMBER_TYPE 0

/** A char Input. */
#define CHAR_TYPE 1


/* --- Structs --- */

/** An input for a calculation program. */
struct Input
{
	/** The input's type. */
	int type;

	/** The input's value. Can be read as int or char. */
	int value;
};

#endif