#include "ADC.h"
#include "device_registers.h"
#include "clocks_and_modes.h"

void PORT_init(void){
PCC->PCCn[PCC_PORTD_INDEX]|=PCC_PCCn_CGC_MASK; 
PCC->PCCn[PCC_PORTE_INDEX]|=PCC_PCCn_CGC_MASK;
		
PTD->PDDR |= 1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7;
PTE->PDDR |= 1<<13|1<<14|1<<15|1<<16; 
	
PORTD->PCR[0] = PORT_PCR_MUX(1);
PORTD->PCR[1] = PORT_PCR_MUX(1);
PORTD->PCR[2] = PORT_PCR_MUX(1);
PORTD->PCR[3] = PORT_PCR_MUX(1);
PORTD->PCR[4] = PORT_PCR_MUX(1);
PORTD->PCR[5] = PORT_PCR_MUX(1);
PORTD->PCR[6] = PORT_PCR_MUX(1);
PORTD->PCR[7] = PORT_PCR_MUX(1);

PORTE->PCR[13] = PORT_PCR_MUX(1); 
PORTE->PCR[14] = PORT_PCR_MUX(1); 
PORTE->PCR[15] = PORT_PCR_MUX(1); 
PORTE->PCR[16] = PORT_PCR_MUX(1); 
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
	int num = 0;
	
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
			
		num = adcResultInMv / 42;
			
		if(num>7) 
		{
			PTD->PSOR |= 1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7;
      PTE->PSOR |= 1<<13|1<<14|1<<15|1<<16; 
			num = num + 5;
			PTE->PCOR |= 1<<num; 
		}	
		else
 		{
			PTD->PSOR |= 1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7;
      PTE->PSOR |= 1<<13|1<<14|1<<15|1<<16; 
			PTD->PCOR |= 1<<num;
		}
		
	}
}