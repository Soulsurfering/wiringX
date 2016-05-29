/*
	Copyright (c) 2014 CurlyMo <curlymoo1@gmail.com>

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>   
	
#include "../../soc/soc.h"
#include "../../wiringX.h"	
#include "../platform.h"	
#include "chip.h"			

struct platform_t *chip = NULL;

/*
	TODO
*/

static int map[] = {
	// TODO: fillme
};

// SYSFS GPIO 132 LED D3 (red)

static int chipValidGPIO(int pin) {
	if(pin >= 0 && pin < (sizeof(map)/sizeof(map[0]))) {
		if(map[pin] == -1) {
			return -1;
		}
		return 0;
	} else {
		return -1;
	}
}

static int chipPinMode(int i, enum pinmode_t mode) {
	if(map[i] == -1) {
		return -1;
	}
	if((i == 0 || i == 1) && (mode != PINMODE_OUTPUT)) {
		wiringXLog(LOG_ERR, "The %s pin %d can only be used as output", chip->name, i);
		return -1;
	}
	return chip->soc->pinMode(i, mode);
}

static int chipDigitalWrite(int i, enum digital_value_t value) {
	if(map[i] == -1) {
		return -1;
	}	
	return chip->soc->digitalWrite(i, value);	
}

void chipInit(void) {
	chip = malloc(sizeof(struct platform_t));
	strcpy(chip->name, "chip");

	chip->soc = soc_get("Allwinner", "R8");
	chip->soc->setMap(map);

	chip->digitalRead = chip->soc->digitalRead;
	chip->digitalWrite = &chipDigitalWrite;
	chip->pinMode = &chipPinMode;
	chip->setup = chip->soc->setup;

	chip->isr = NULL;
	chip->waitForInterrupt = NULL;

	chip->selectableFd = chip->soc->selectableFd;
	chip->gc = chip->soc->gc;

	chip->validGPIO = &chipValidGPIO;

	platform_register(chip);
}
