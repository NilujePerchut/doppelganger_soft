#include <delay.h>
#include "utils.h"

void delayms(unsigned short milliseconds)
{
	unsigned short n100tcy;
	n100tcy = SYSTEM_PERIPH_FREQUENCY/100000;
	while (milliseconds--)
		delay100tcy(n100tcy-3);
}
