#include "uart.h"

int main()
{
	char c;
	
	init_uart();
	
	while (1)
	{
		c = getchar();
		putchar(c+1);
	}
	
	return 0;
}
