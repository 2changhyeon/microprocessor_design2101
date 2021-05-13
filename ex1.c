#include "ADC.h"
#include "device_registers.h"
#include "clocks_and_modes.h"

void PORT_init(void)
{
	PCC->PCCn[PCC_PORTD_INDEX]|=PCC_PCCn_CGC_MASK;
	PORTD->PCR[0] = PORT_PCR_MUX(1);
	PORTD->PCR[15] = PORT_PCR_MUX(1);
	PORTD->PCR[16] = PORT_PCR_MUX(1);
	
	PTD->PDDR |= 1<<0 | 1<<15 | 1<<16;
}

void WDOG_disable(void)
{
	WDOG->CNT=0xD928C520; /*Unlock watchdog*/
	WDOG->TOVAL=0x0000FFFF; /*Maximum timeout value*/
	WDOG->CS = 0x00002100; /*Disable watchdog*/
}

int main(void) 
{
	uint32_t adcResultInMv = 0;
	
	
	WDOG_disable();
	PORT_init(); /* Configure ports */
	SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal*/
	SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz(); /* Initclocks: 80 MHz sysclk& core, 40 MHz bus, 20 MHz flash */
	ADC_init();
	
	for(;;)
	{
		convertAdcChan(12);
		while(adc_complete()==0){}
		adcResultInMv = read_adc_chx();
			
		if(adcResultInMv > 3750)
		{
			PTD->PSOR |= 1<<0 | 1<<16;
			PTD->PCOR |= 1<<15;
		}
		else if(adcResultInMv > 2500)
		{
			PTD->PSOR |= 1<<0 | 1<<15;
			PTD->PCOR |= 1<<16;
		}
		else if(adcResultInMv > 1250)
		{
			PTD->PSOR |= 1<<15 | 1<<16;
			PTD->PCOR |= 1<<0;
		}
		else
		{
			PTD->PSOR |= 1<<0 | 1<<15 | 1<<16;
		}
	}
}