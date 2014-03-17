#ifndef BUTTON_FLASH_H
#define BUTTON_FLASH_H

#define ON 1
#define OFF 0

void bfInitialize(void);
void button_press(void);

int orange_status(void);
int yellow_status(void);
int green_status(void);
int blue_status(void);

#endif

