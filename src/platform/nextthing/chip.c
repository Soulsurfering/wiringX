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
gpio_pin location          Name
408, //  Physical U14-13 - XIO-P0  // access > 120탎 (I2C proxy)
409, //  Physical U14-14 - XIO-P1  // access > 120탎 (I2C proxy)
410, //  Physical U14-15 - XIO-P2  // access > 120탎 (I2C proxy)
411, //  Physical U14-16 - XIO-P3  // access > 120탎 (I2C proxy)
412, //  Physical U14-17 - XIO-P4  // access > 120탎 (I2C proxy)
413, //  Physical U14-18 - XIO-P5  // access > 120탎 (I2C proxy)
414, //  Physical U14-19 - XIO-P6  // access > 120탎 (I2C proxy)
415, //  Physical U14-20 - XIO-P7  // access > 120탎 (I2C proxy)

128, //  Physical U14-27 - CSIPCK   (PE-0) input only
129, //  Physical U14-28 - CSICK    (PE-1) input only
130, //  Physical U14-29 - CSIHSYNC (PE-2) input only
131, //  Physical U14-30 - CSIVSYNC (PE-3) input only ??
132, //  Physical U14-31 - CSID0 (PE-4)
133, //  Physical U14-32 - CSID1 (PE-5)
134, //  Physical U14-33 - CSID2 (PE-6) 
135, //  Physical U14-34 - CSID3 (PE-7)
136, //  Physical U14-35 - CSID4 (PE-8)
137, //  Physical U14-36 - CSID5 (PE-9)
138, //  Physical U14-37 - CSID6 (PE-10)
139, //  Physical U14-38 - CSID7 (PE-11)

 99, //  Physical U13-20 - LCD-D3 (PD-3)
100, //  Physical U13-19 - LCD-D4 ..
101, //  Physical U13-22 - LCD-D5
102, //  Physical U13-21 - LCD-D6
103, //  Physical U13-24 - LCD-D7
106, //  Physical U13-23 - LCD-D10
107, //  Physical U13-26 - LCD-D11
108, //  Physical U13-25 - LCD-D12
109, //  Physical U13-28 - LCD-D13
110, //  Physical U13-27 - LCD-D14
111, //  Physical U13-30 - LCD-D15
114, //  Physical U13-29 - LCD-D18
115, //  Physical U13-32 - LCD-D19
116, //  Physical U13-31 - LCD-D20
117, //  Physical U13-34 - LCD-D21
118, //  Physical U13-33 - LCD-D22 ..
119, //  Physical U13-36 - LCD-D23 (PD-23)

120, //  Physical U13-35 - LCD-CLK   (PD-24)
122, //  Physical U13-38 - LCD-HSYNC (PD-26)
123, //  Physical U13-37 - LCD-VSYNC (PD-27)
 121 //  Physical U13-40 - LCD-DE    (PD-25)	
*/

//static int mymap[] = {
//	-408, -409, -410, -411, -412, -413, -414, -415, // XIO0..7 - negative value, not memory mapped
//	  53,   54,   55,   56,   57,   58,   59,   60, // PE4..11
//	  28,   29,   30,   31,   32,   33,   34,   35, // PD3..7, 10..12
//	  36,   37,   38,   39,   40,   41,   42,   43, // PD13..15, 18..22
//	  44,   45,   46,   47,   48			// PD23, 24, 25, 26, 27
//};

static int map[] = {
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //   0..  9
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  10.. 19
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  20.. 29
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  30.. 39
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  40.. 49
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  50.. 59
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  60.. 69
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  70.. 79
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, //  80.. 89
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   28, //  90.. 99
	  29,   30,   31,   32,   -1,   -1,   33,   34,   35,   36, // 100..109
	  37,   38,   -1,   -1,   39,   40,   41,   42,   43,   44, // 110..119
	  45,   46,   47,   48,   -1,   -1,   -1,   -1,   49,   50, // 120..129
	  51,   52,   53,   54,   55,   56,   57,   58,   59,   60  // 130..139	
};

/*
 * special platform pins accessed via I2C
 * slow performance ~120탎
 */
static int isXIOPin(int pin) {
	if (pin >= 408 && pin <= 415)
		return 0;
	return -1;
}

static int chipValidGPIO(int pin) {
	if (isXIOPin(pin))
		return 0;
	
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
	if (isXIOPin(i)) {
		// TODO handle XIO pins via I2C
		return 1;
	}
	if(map[i] == -1) {
		return -1;
	}
	if((i >= 128 && i <= 131) && (mode != PINMODE_INPUT)) {
		wiringXLog(LOG_ERR, "The %s pin %d can only be used as input", chip->name, i);
		return -1;
	}
	return chip->soc->pinMode(i, mode);
}

static int digitalRead(int pin) {
	if (isXIOPin(pin)) {
		// TODO handle XIO pins via I2C
		return -1;
	}
	if(map[pin] == -1) {
		return -1;
	}	
	return chip->soc->digitalRead(pin);	
}

static int chipDigitalWrite(int i, enum digital_value_t value) {
	if (isXIOPin(i)) {
		// TODO handle XIO pins via I2C
		return -1;
	}
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

	chip->digitalRead = &chipDigitalRead;
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
