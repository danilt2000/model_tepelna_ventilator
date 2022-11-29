/*
 Example program for heat plant with fan

 Zobrazuje teplotu na displeji. Pri prekroceni 40C vypne topeni, jinak zapne.
 Stav topeni je na displeji.
 */

#include "MKL25Z4.h"
#include "drv_lcd.h"
#include "heat_fan.h"
#include <stdio.h>

static int i = 0;
char buff[20];

void delay(void) {
	uint32_t i;
	for (i = 0; i < 2000000; i++)
		;
}

int main(void) {
	int temperature, fan_rpm;
	uint8_t pwm_duty = 0;

	LCD_initialize();
	LCD_clear();
	LCD_puts("Start");








	HEATFAN_Init();
	HEATFAN_CtrlSignalSel(HEATFAN_Heater);
	HEATFAN_SetPWMDuty(pwm_duty);

	LCD_clear();

	for (;;) {
		temperature = HEATFAN_GetTemperature();	// Pozor, teplota je v desetinach st.C
		fan_rpm = HEATFAN_GetFanRPM();

		sprintf(buff, "Teplota: %4.1f", (float) temperature / 10);
		LCD_set_cursor(1, 1);
		LCD_puts(buff);

		sprintf(buff, "Otacky: %5d", fan_rpm);
		LCD_set_cursor(2, 1);
		LCD_puts(buff);

		sprintf(buff, "Max Teplota: %4.1f", (float) 400 / 10);
		LCD_set_cursor(4, 1);
		LCD_puts(buff);

		int CurrentTemplature = HEATFAN_GetTemperature();

		LCD_set_cursor(3, 1);
		if (temperature < 1000) {
			pwm_duty = 100;
			LCD_puts("Topeni ON ");
		} else {
			pwm_duty = 0;
			LCD_puts("Topeni OFF");
		}

		HEATFAN_SetPWMDuty(pwm_duty);
		delay();
		LCD_clear();
	}

	/* Never leave main */
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
