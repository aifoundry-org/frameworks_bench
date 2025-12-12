#include "main_functions.h"

/* Increase number of loops to see repeatable timing output */
#define NUM_LOOPS 10

int main(void)
{
	setup();
	/* Note: Modified from original while(true) to accommodate CI */
	for (int i = 0; i < NUM_LOOPS; i++) {
		loop();
	}
	return 0;
}
