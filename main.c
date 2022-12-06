/*
 Example program for heat plant with fan

 Zobrazuje teplotu na displeji. Pri prekroceni 40C vypne topeni, jinak zapne.
 Stav topeni je na displeji.
 */

#include "MKL25Z4.h"
#include "drv_lcd.h"
#include "heat_fan.h"
#include <stdio.h>
#include "MKL25Z4.h"
#include "FreeRTOS.h"
#include "semphr.h" // FreeRTOS semafory a mutexy
#include "drv_lcd.h"

static int i = 0;

char buff[20];

void ADCInit(void);

uint32_t ADCCalibrate(void);

void MainTask( void* pvParameters );

void delay(void) {
	uint32_t i;
	for (i = 0; i < 2000000; i++)
		;
}

int main(void) {
	int temperature, fan_rpm;
	uint8_t pwm_duty = 0;

	BaseType_t status = xTaskCreate(
	MainTask, /* ukazatel na task */
	"Main", /* jmeno tasku pro ladeni - kernel awareness debugging */
	configMINIMAL_STACK_SIZE, /* velikost zasobniku = task stack size */
	(void*)NULL, /* pripadny parametr pro task = optional task startup argument */
	tskIDLE_PRIORITY, /* priorita tasku = initial priority */
	(xTaskHandle*)NULL /* pripadne handle na task, pokud ma byt vytvoreno */
	);



	if ( status != pdPASS) {

	while(1) {
	; /* error! probably out of memory */
		}
	}





	LCD_initialize();
	LCD_clear();
	LCD_puts("Start");


	// Inicializace A/D prevodniku
		ADCInit();

		// Kalibrace a nova inicializace
		// Pro dosazeni udavane presnosti musi byt prevodnik kalibrovan po
		// kazdem resetu.
		// Nova inicializace je potreba protoze pri kalibraci
		// je prevodnik prenastaven.
		ADCCalibrate();
		ADCInit();

		// Nastaveni pinu, kde je pripojen potenciometr,
		// do rezimu vstupu pro A/D prevodnik: pin PTC2, funkce ALT0
		// 1. Povolime hodinovy signal pro port C
		SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
		// 2. NAstavime funkci pinu na ALT0 = vstup A/D prevodniku
		PORTC->PCR[2] = PORT_PCR_MUX(0);





	HEATFAN_Init();
	HEATFAN_CtrlSignalSel(HEATFAN_Heater);
	HEATFAN_SetPWMDuty(pwm_duty);

	LCD_clear();

	for (;;) {

		// Spusteni prevodu na kanalu 11.
				// Protoze ostatni nastaveni v registru SC1 mame na 0, muzeme si dovolit
				// primo v nem prepsat hodnotu cislem kanalu. Lepsi reseni by bylo
				// "namaskovat" cislo kanalu bez zmeny hodnoty registru.
				ADC0->SC1[0] = ADC_SC1_ADCH(11);

				// Cekame na dokonceni prevodu
				while ( (ADC0->SC1[0] & ADC_SC1_COCO_MASK) == 0 )
					;

				// Ulozime vysledek prevodu
				uint16_t vysledek = ADC0->R[0];



		temperature = HEATFAN_GetTemperature();	// Pozor, teplota je v desetinach st.C
		fan_rpm = HEATFAN_GetFanRPM();

		sprintf(buff, "Teplota: %4.1f", (float) temperature / 10);
		LCD_set_cursor(1, 1);
		LCD_puts(buff);

		sprintf(buff, "Otacky: %5d", fan_rpm);
		LCD_set_cursor(2, 1);
		LCD_puts(buff);

		int MaxTemperature=400;

		sprintf(buff, "Max Teplota: %4.1f", (float) vysledek  / 10);
		LCD_set_cursor(4, 1);
		LCD_puts(buff);

		int CurrentTemplature = HEATFAN_GetTemperature();

		LCD_set_cursor(3, 1);
		if (temperature < vysledek) {
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
void MainTask( void* pvParameters )
{
(void) pvParameters; /* parameter not used */


//
// 1. varianta kodu: pozastaveni na danou dobu (nepresne)
//
/*
for (;;) {
// Prepnuti stavu LED
RED_LED_TOGGLE();

// Pozastaveni procesu na dany pocet tiku.
// Pro vypocet doby pozastaveni v milisekundach se pouziva makro portTICK_RATE_MS
// coz je asi 1/(pocet tiku za milisekundu)
// POZOR: vTaskDelay se nedoporucuje ulohy, ktere maji byt spousteny
// s presnou periodou, protoze doba pozastaveni je relativni - task je
// pozastaven na dany pocet tiku od okamziku volani.
// Pro presne periodicke casovani je doporucena vTaskDelayUntil.
vTaskDelay(1000 / portTICK_RATE_MS);
}
*/


//
// 2. varianta kodu: zajisteni spousteni kodu v presnych intervalech
//

const TickType_t xFrequency = 500 / portTICK_RATE_MS;
TickType_t xLastWakeTime;

// Initialise the xLastWakeTime variable with the current time.
xLastWakeTime = xTaskGetTickCount();
for (;;) {

// Prepnuti stavu LED
RED_LED_TOGGLE();
vTaskDelay(200 / portTICK_RATE_MS);

// Postaveni do doby dalsiho spusteni s konstantni periodou
vTaskDelayUntil(&xLastWakeTime, xFrequency);
}

}
void ADCInit(void)
{
	// Povolit hodinovy signal pro ADC
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

	// Zakazeme preruseni, nastavime kanal 31 = A/D prevodnik vypnut, jinak by zapisem
	// doslo ke spusteni prevodu
	// Vybereme single-ended mode
	ADC0->SC1[0] =  ADC_SC1_ADCH(31);

	// Vyber hodinoveho signalu, preddelicky a rozliseni
	// Clock pro ADC nastavime <= 4 MHz, coz je doporuceno pro kalibraci.
	// Pri max. CPU frekvenci 48 MHz je bus clock 24 MHz, pri delicce = 8
	// bude clock pro ADC 3 MHz
	ADC0->CFG1 = ADC_CFG1_ADICLK(0)		/* ADICLK = 0 -> bus clock */
		| ADC_CFG1_ADIV(3)				/* ADIV = 3 -> clock/8 */
		| ADC_CFG1_MODE(2);				/* MODE = 2 -> rozliseni 10-bit */

	// Do ostatnich registru zapiseme vychozi hodnoty:
	// Vybereme sadu kanalu "a", vychozi nejdelsi cas prevodu (24 clocks)
	ADC0->CFG2 = 0;

	// Softwarove spousteni prevodu, vychozi reference
	ADC0->SC2 = 0;

	// Hardwarove prumerovani vypnuto
	ADC0->SC3 = 0;	/* default values, no averaging */

}
uint32_t ADCCalibrate(void)
{
	 unsigned short cal_var;

	  ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK;	/* Enable Software Conversion Trigger for Calibration Process */
	  ADC0->SC3 &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK );    /* set single conversion, clear avgs bitfield for next writing */

	  ADC0->SC3 |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(32) ); /* turn averaging ON and set desired value */

	  ADC0->SC3 |= ADC_SC3_CAL_MASK;      /* Start CAL */

	  /* Wait calibration end */
	  while ( (ADC0->SC1[0] & ADC_SC1_COCO_MASK) == 0 )
		  ;

	  /* Check for Calibration fail error and return */
	  if ( (ADC0->SC3 & ADC_SC3_CALF_MASK) != 0 )
		  return 1;

	  // Calculate plus-side calibration
	  cal_var = 0;
	  cal_var =  ADC0->CLP0;
	  cal_var += ADC0->CLP1;
	  cal_var += ADC0->CLP2;
	  cal_var += ADC0->CLP3;
	  cal_var += ADC0->CLP4;
	  cal_var += ADC0->CLPS;

	  cal_var = cal_var/2;
	  cal_var |= 0x8000; // Set MSB
	  ADC0->PG = ADC_PG_PG(cal_var);

	  // Calculate minus-side calibration
	  cal_var = 0;
	  cal_var =  ADC0->CLM0;
	  cal_var += ADC0->CLM1;
	  cal_var += ADC0->CLM2;
	  cal_var += ADC0->CLM3;
	  cal_var += ADC0->CLM4;
	  cal_var += ADC0->CLMS;

	  cal_var = cal_var/2;
	  cal_var |= 0x8000; // Set MSB
	  ADC0->MG = ADC_MG_MG(cal_var);

	  ADC0->SC3 &= ~ADC_SC3_CAL_MASK;

	  return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
