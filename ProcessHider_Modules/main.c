#include "stdlib.h"

int main()
{
	system("make");
	system("insmod ./hider.ko");
	return 0;
}
