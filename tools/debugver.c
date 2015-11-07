#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {
	char str[3] = {0};
	int KERNEL_VERSION_DEBUG;
	
	if (argc < 2) {
		fprintf(stderr, "%s: Need a build number.\n", argv[0]);
		return -1;
	}
	
	KERNEL_VERSION_DEBUG = atoi(argv[1]);
	if (KERNEL_VERSION_DEBUG > 26) {
		str[0] = (KERNEL_VERSION_DEBUG / 26) + 'a' - 1;
		str[1] = (KERNEL_VERSION_DEBUG % 26) + 'a' - 1;
	} else if (KERNEL_VERSION_DEBUG) {
		str[0] = KERNEL_VERSION_DEBUG + 'a' - 1;
	}
	
	printf("%s", str);
	
	return 0;
}
