/*
	Copyright (c) 2016 CurlyMo <curlymoo1@gmail.com>

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "wiringX.h"
#include "../src/platform/platform.h"

char *usage =
	"Usage: %s platform\n"
	"Example: %s raspberrypi2\n";

int main(int argc, char *argv[]) {
	char *str = NULL, *platform = NULL;
	char usagestr[130];
	int gpio = 0, i = 0;

	memset(usagestr, '\0', 130);

	// expect only 1 argument => argc must be 2
	if(argc != 2) {
		snprintf(usagestr, 129, usage, argv[0], argv[0]);
		puts(usagestr);
		return -1;
	}

	// check for a valid, numeric argument
	platform = argv[1];
	
	if(wiringXSetup(platform, NULL) == -1) {
		wiringXGC();
		return -1;
	}
	
	printf("GPIO mapping for %s\n", platform);
	for (i=0; i<=999; i++) {
		if(wiringXValidGPIO(i) == 0) {
			printf("GPIO %03d => ", i);
			str = getPinName(i);
			if (str && *str) {
				printf("%s\n", str);
			} else {
				printf("Valid but unknown\n");
			} 
		}
	}
}	

	
	