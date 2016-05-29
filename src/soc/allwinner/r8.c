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
#include <ctype.h>

#include "r8.h"
#include "../../wiringX.h"
#include "../soc.h"

struct soc_t *allwinnerR8 = NULL;

static struct layout_t {
	char *name;

	int addr;

	struct {
		unsigned long offset;
		unsigned long bit;
	} select;

	struct {
		unsigned long offset;
		unsigned long bit;
	} data;

	int support;

	enum pinmode_t mode;

	int fd;

} layout[] = {
 { "PB0",  0, { 0x24,  0 }, { 0x34,  0 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PB1",  0, { 0x24,  4 }, { 0x34,  1 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PB2",  0, { 0x24,  8 }, { 0x34,  2 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT16
 { "PB3",  0, { 0x24, 12 }, { 0x34,  3 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT17
 { "PB4",  0, { 0x24, 16 }, { 0x34,  4 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT18
 { "PB10", 0, { 0x28,  8 }, { 0x34, 10 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT24
 { "PB15", 0, { 0x28, 28 }, { 0x34, 15 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PB16", 0, { 0x2C,  0 }, { 0x34, 16 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PB17", 0, { 0x2C,  4 }, { 0x34, 17 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PB18", 0, { 0x2C,  8 }, { 0x34, 18 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC0",  0, { 0x48,  0 }, { 0x58,  0 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC1",  0, { 0x48,  4 }, { 0x58,  1 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC2",  0, { 0x48,  8 }, { 0x58,  2 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC3",  0, { 0x48, 12 }, { 0x58,  3 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC4",  0, { 0x48, 16 }, { 0x58,  4 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC5",  0, { 0x48, 20 }, { 0x58,  5 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC6",  0, { 0x48, 24 }, { 0x58,  6 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC7",  0, { 0x48, 28 }, { 0x58,  7 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC8",  0, { 0x4C,  0 }, { 0x58,  8 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC9",  0, { 0x4C,  4 }, { 0x58,  9 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC10", 0, { 0x4C,  8 }, { 0x58, 10 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC11", 0, { 0x4C, 12 }, { 0x58, 11 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC12", 0, { 0x4C, 16 }, { 0x58, 12 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC13", 0, { 0x4C, 20 }, { 0x58, 13 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC14", 0, { 0x4C, 24 }, { 0x58, 14 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC15", 0, { 0x4C, 28 }, { 0x58, 15 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PC19", 0, { 0x50, 12 }, { 0x58, 19 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD2",  0, { 0x6C,  8 }, { 0x7C,  2 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD3",  0, { 0x6C, 12 }, { 0x7C,  3 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD4",  0, { 0x6C, 16 }, { 0x7C,  4 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD5",  0, { 0x6C, 20 }, { 0x7C,  5 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD6",  0, { 0x6C, 24 }, { 0x7C,  6 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD7",  0, { 0x6C, 28 }, { 0x7C,  7 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD10", 0, { 0x70,  8 }, { 0x7C, 10 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD11", 0, { 0x70, 12 }, { 0x7C, 11 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD12", 0, { 0x70, 16 }, { 0x7C, 12 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD13", 0, { 0x70, 20 }, { 0x7C, 13 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD14", 0, { 0x70, 24 }, { 0x7C, 14 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD15", 0, { 0x70, 28 }, { 0x7C, 15 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD18", 0, { 0x74,  8 }, { 0x7C, 18 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD19", 0, { 0x74, 12 }, { 0x7C, 19 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD20", 0, { 0x74, 16 }, { 0x7C, 20 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD21", 0, { 0x74, 20 }, { 0x7C, 21 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD22", 0, { 0x74, 24 }, { 0x7C, 22 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD23", 0, { 0x74, 28 }, { 0x7C, 23 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD24", 0, { 0x78,  0 }, { 0x7C, 24 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD25", 0, { 0x78,  4 }, { 0x7C, 25 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD26", 0, { 0x78,  8 }, { 0x7C, 26 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PD27", 0, { 0x78, 12 }, { 0x7C, 27 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE0",  0, { 0x90,  0 }, { 0xA0,  0 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT14
 { "PE1",  0, { 0x90,  4 }, { 0xA0,  1 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT15
 { "PE2",  0, { 0x90,  8 }, { 0xA0,  2 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE3",  0, { 0x90, 12 }, { 0xA0,  3 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE4",  0, { 0x90, 16 }, { 0xA0,  4 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE5",  0, { 0x90, 20 }, { 0xA0,  5 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE6",  0, { 0x90, 24 }, { 0xA0,  6 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE7",  0, { 0x90, 28 }, { 0xA0,  7 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE8",  0, { 0x94,  0 }, { 0xA0,  8 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE9",  0, { 0x94,  4 }, { 0xA0,  9 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE10", 0, { 0x94,  8 }, { 0xA0, 10 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PE11", 0, { 0x94, 12 }, { 0xA0, 11 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PF0",  0, { 0xB4,  0 }, { 0xC4,  0 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PF1",  0, { 0xB4,  4 }, { 0xC4,  1 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PF2",  0, { 0xB4,  8 }, { 0xC4,  2 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PF3",  0, { 0xB4, 12 }, { 0xC4,  3 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PF4",  0, { 0xB4, 16 }, { 0xC4,  4 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PF5",  0, { 0xB4, 20 }, { 0xC4,  5 }, FUNCTION_DIGITAL, PINMODE_NOT_SET, 0 },
 { "PG0",  0, { 0xD8,  0 }, { 0xE8,  0 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT0
 { "PG1",  0, { 0xD8,  4 }, { 0xE8,  1 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT1
 { "PG2",  0, { 0xD8,  8 }, { 0xE8,  2 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT2
 { "PG3",  0, { 0xD8, 12 }, { 0xE8,  3 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT3
 { "PG4",  0, { 0xD8, 16 }, { 0xE8,  4 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT4
 { "PG9",  0, { 0xDC,  4 }, { 0xE8,  9 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT9
 { "PG10", 0, { 0xDC,  8 }, { 0xE8, 10 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT10
 { "PG11", 0, { 0xDC, 12 }, { 0xE8, 11 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }, // INT11
 { "PG12", 0, { 0xDC, 16 }, { 0xE8, 12 }, FUNCTION_DIGITAL | FUNCTION_INTERRUPT, PINMODE_NOT_SET, 0 }  // INT12
};

static int allwinnerR8Setup(void) {
	if((allwinnerR8->fd = open("/dev/mem", O_RDWR | O_SYNC )) < 0) {
		wiringXLog(LOG_ERR, "wiringX failed to open /dev/mem for raw memory access");
		return -1;
	}

	if((allwinnerR8->gpio[0] = (unsigned char *)mmap(0, allwinnerR8->page_size, PROT_READ|PROT_WRITE, MAP_SHARED, allwinnerR8->fd, allwinnerR8->base_addr[0])) == NULL) {
		wiringXLog(LOG_ERR, "wiringX failed to map the %s %s GPIO memory address", allwinnerR8->brand, allwinnerR8->chip);
		return -1;
	}

	return 0;
}

static char *allwinnerR8GetPinName(int pin) {
	return allwinnerR8->layout[pin].name;
}

static void allwinnerR8SetMap(int *map) {
	allwinnerR8->map = map;
}

static int allwinnerR8DigitalWrite(int i, enum digital_value_t value) {
	struct layout_t *pin = NULL;
	unsigned long addr = 0;
	unsigned long val = 0; 

	pin = &allwinnerR8->layout[allwinnerR8->map[i]];

	if(allwinnerR8->map == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been mapped", allwinnerR8->brand, allwinnerR8->chip);
		return -1; 
	}
	if(allwinnerR8->fd <= 0 || allwinnerR8->gpio == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been setup by wiringX", allwinnerR8->brand, allwinnerR8->chip);
		return -1;
	}
	if(pin->mode != PINMODE_OUTPUT) {
		wiringXLog(LOG_ERR, "The %s %s GPIO %d is not set to output mode", allwinnerR8->brand, allwinnerR8->chip, i);
		return -1;
	}

	addr = (unsigned long)(allwinnerR8->gpio[pin->addr] + allwinnerR8->base_offs[pin->addr] + pin->data.offset);

	val = soc_readl(addr);
	if(value == HIGH) {
		soc_writel(addr, val | (1 << pin->data.bit));
	} else {
		soc_writel(addr, val & ~(1 << pin->data.bit)); 
	}
	return 0;
}

static int allwinnerR8DigitalRead(int i) {
	void *gpio = NULL;
	struct layout_t *pin = NULL;
	unsigned long addr = 0;
	unsigned long val = 0;

	gpio = allwinnerR8->gpio[pin->addr];
	pin = &allwinnerR8->layout[allwinnerR8->map[i]];
	addr = (unsigned long)(gpio + allwinnerR8->base_offs[pin->addr] + pin->select.offset);

	if(allwinnerR8->map == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been mapped", allwinnerR8->brand, allwinnerR8->chip);
		return -1; 
	}
	if(allwinnerR8->fd <= 0 || allwinnerR8->gpio == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been setup by wiringX", allwinnerR8->brand, allwinnerR8->chip);
		return -1;
	}
	if(pin->mode != PINMODE_INPUT) {
		wiringXLog(LOG_ERR, "The %s %s GPIO %d is not set to input mode", allwinnerR8->brand, allwinnerR8->chip, i);
		return -1;
	}

	val = soc_readl(addr);

	return (int)((val & (1 << pin->data.bit)) >> pin->data.bit);
}

static int allwinnerR8PinMode(int i, enum pinmode_t mode) {
	struct layout_t *pin = NULL;
	unsigned long addr = 0;
	unsigned long val = 0;

	if(allwinnerR8->map == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been mapped", allwinnerR8->brand, allwinnerR8->chip);
		return -1; 
	} 
	if(allwinnerR8->fd <= 0 || allwinnerR8->gpio == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been setup by wiringX", allwinnerR8->brand, allwinnerR8->chip);
		return -1;
	}

	pin = &allwinnerR8->layout[allwinnerR8->map[i]];
	addr = (unsigned long)(allwinnerR8->gpio[pin->addr] + allwinnerR8->base_offs[pin->addr] + pin->select.offset);
	pin->mode = mode;

	val = soc_readl(addr);
	if(mode == PINMODE_OUTPUT) {
		soc_writel(addr, val | (1 << pin->select.bit));
	} else if(mode == PINMODE_INPUT) {
		soc_writel(addr, val & ~(1 << pin->select.bit));
	}
	soc_writel(addr, val & ~(1 << (pin->select.bit+1)));
	soc_writel(addr, val & ~(1 << (pin->select.bit+2)));
	return 0;
}


static int allwinnerR8ISR(int i, enum isr_mode_t mode) {
	struct layout_t *pin = NULL;
	char path[PATH_MAX];
	int x = 0;

	if(allwinnerR8->map == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been mapped", allwinnerR8->brand, allwinnerR8->chip);
		return -1; 
	} 
	if(allwinnerR8->fd <= 0 || allwinnerR8->gpio == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been setup by wiringX", allwinnerR8->brand, allwinnerR8->chip);
		return -1;
	}

	pin = &allwinnerR8->layout[allwinnerR8->map[i]];
	char name[strlen(pin->name)+1];

	memset(&name, '\0', strlen(pin->name)+1);
	for(x = 0; pin->name[x]; x++){
		name[x] = tolower(pin->name[x]);
	} 

	sprintf(path, "/sys/class/gpio/gpio%d_%s", i, name);
	if((soc_sysfs_check_gpio(allwinnerR8, path)) == -1) {
		sprintf(path, "/sys/class/gpio/export");
		if(soc_sysfs_gpio_export(allwinnerR8, path, i) == -1) {
			return -1;
		}
	}

	sprintf(path, "/sys/class/gpio/gpio%d_%s/direction", i, name); 
	if(soc_sysfs_set_gpio_direction(allwinnerR8, path, "in") == -1) {
		return -1;
	}

	sprintf(path, "/sys/class/gpio/gpio%d_%s/edge", i, name);
	if(soc_sysfs_set_gpio_interrupt_mode(allwinnerR8, path, mode) == -1) {
		return -1;
	}

	sprintf(path, "/sys/class/gpio/gpio%d_%s/value", i, name);
	if((pin->fd = soc_sysfs_gpio_reset_value(allwinnerR8, path)) == -1) {
		return -1;
	}
	pin->mode = PINMODE_INTERRUPT; 

	return 0;
}

static int allwinnerR8WaitForInterrupt(int i, int ms) {
	struct layout_t *pin = &allwinnerR8->layout[allwinnerR8->map[i]];

	if(pin->mode != PINMODE_INTERRUPT) {
		wiringXLog(LOG_ERR, "The %s %s GPIO %d is not set to interrupt mode", allwinnerR8->brand, allwinnerR8->chip, i);
		return -1;
	}
	if(pin->fd <= 0) {
		wiringXLog(LOG_ERR, "The %s %s GPIO %d has not been opened for reading", allwinnerR8->brand, allwinnerR8->chip, i);
		return -1; 
	}

	return soc_wait_for_interrupt(allwinnerR8, pin->fd, ms);
}

static int allwinnerR8GC(void) {
	struct layout_t *pin = NULL;
	char path[PATH_MAX];
	int i = 0, l = 0, x = 0;

	if(allwinnerR8->map != NULL) {
		l = sizeof(allwinnerR8->map)/sizeof(allwinnerR8->map[0]);

		for(i=0;i<l;i++) {
			pin = &allwinnerR8->layout[allwinnerR8->map[i]];
			if(pin->mode == PINMODE_OUTPUT) {
				pinMode(i, PINMODE_INPUT);
			} else if(pin->mode == PINMODE_INTERRUPT) {
				char name[strlen(pin->name)+1];

				memset(&name, '\0', strlen(pin->name)+1);
				for(x = 0; pin->name[x]; x++){
					name[x] = tolower(pin->name[x]);
				}
				sprintf(path, "/sys/class/gpio/gpio%d_%s", i, name);
				if((soc_sysfs_check_gpio(allwinnerR8, path)) == 0) {
					sprintf(path, "/sys/class/gpio/unexport");
					soc_sysfs_gpio_unexport(allwinnerR8, path, i);
				}
			}
			if(pin->fd > 0) {
				close(pin->fd);
				pin->fd = 0;
			}
		}
	}
	if(allwinnerR8->gpio != NULL) {
		munmap(allwinnerR8->gpio, allwinnerR8->page_size);
	} 
	return 0;
}

static int allwinnerR8SelectableFd(int i) {
	struct layout_t *pin = NULL;

	if(allwinnerR8->map == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been mapped", allwinnerR8->brand, allwinnerR8->chip);
		return -1; 
	} 
	if(allwinnerR8->fd <= 0 || allwinnerR8->gpio == NULL) {
		wiringXLog(LOG_ERR, "The %s %s has not yet been setup by wiringX", allwinnerR8->brand, allwinnerR8->chip);
		return -1;
	}

	pin = &allwinnerR8->layout[allwinnerR8->map[i]];
	return pin->fd;
}

void allwinnerR8Init(void) {
	allwinnerR8 = malloc(sizeof(struct soc_t));

	strcpy(allwinnerR8->brand, "Allwinner");
	strcpy(allwinnerR8->chip, "R8");

	allwinnerR8->map = NULL;
	allwinnerR8->layout = layout;

	allwinnerR8->support.isr_modes = ISR_MODE_RISING | ISR_MODE_FALLING | ISR_MODE_BOTH | ISR_MODE_NONE;

	allwinnerR8->page_size = (4*1024);
	allwinnerR8->base_addr[0] = 0x01C20000;
	allwinnerR8->base_offs[0] = 0x00000800;

	allwinnerR8->gc = &allwinnerR8GC;
	allwinnerR8->selectableFd = &allwinnerR8SelectableFd;

	allwinnerR8->pinMode = &allwinnerR8PinMode;
	allwinnerR8->setup = &allwinnerR8Setup;
	allwinnerR8->digitalRead = &allwinnerR8DigitalRead;
	allwinnerR8->digitalWrite = &allwinnerR8DigitalWrite;
	allwinnerR8->getPinName = &allwinnerR8GetPinName;
	allwinnerR8->setMap = &allwinnerR8SetMap;
	allwinnerR8->isr = &allwinnerR8ISR;
	allwinnerR8->waitForInterrupt = &allwinnerR8WaitForInterrupt;

	soc_register(allwinnerR8);
}
