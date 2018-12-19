#include <stdio.h>

int main() {
	int t = -5;
	unsigned int ut = (unsigned int)t;
	printf("%x\n", (unsigned int)t);
	unsigned int tt = ut & 0xffff;
	int ttt = (unsigned int)(short)tt;
	printf("%08x\n", ttt);
	unsigned short a = 10;
	int b = -1;
	printf("%08x\n", a-b);

	printf("%u\n", (363729771)*175863475);
	
	return 0;
}
