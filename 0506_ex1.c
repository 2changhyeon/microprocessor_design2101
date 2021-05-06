#include "device_registers.h"           // Device header
#include "clocks_and_modes.h"
#include "LPUART.h"
#include <stdio.h>

int num = 3444;
int i = 0;

char data=0;
int lpit0_ch0_flag_counter=0;
void LPIT0_init (uint32_t delay);

void PORT_init (void) {
  PCC->PCCn[PCC_PORTC_INDEX]|=PCC_PCCn_CGC_MASK;
  PORTC->PCR[9]|=PORT_PCR_MUX(2);       
}

void WDOG_disable (void){
  WDOG->CNT=0xD928C520;    
  WDOG->TOVAL=0x0000FFFF;  
  WDOG->CS = 0x00002100;    
}

void delay_ms (uint32_t ms){
	 LPIT0_init(ms);           
   while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK)) {} 
               lpit0_ch0_flag_counter++;         
               LPIT0->MSR |= LPIT_MSR_TIF0_MASK; 
}

void LPIT0_init (uint32_t delay){
   uint32_t timeout;

  PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);   
  PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; 

  LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  

  timeout=delay * 40000;
  LPIT0->TMR[0].TVAL = timeout;    
  LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;

}



int main(void)
{
    WDOG_disable();
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    SystemCoreClockUpdate();

    char msg_init = 0x00;
    char msg_clear = 0xA3;
    char msg_reset = 0xA0;
    char msg_Start = 0xA2;
    char string_array[20];

    PORT_init();
    LPUART1_init();

    LPUART1_transmit_char(msg_Start);
    delay_ms(5);

    sprintf(string_array,"%d",num);

    while(string_array[i] != '\0')
    {
        LPUART1_transmit_char(string_array[i]);
        i++;
    }
    i=0;
    delay_ms(5);
    
    LPUART1_transmit_char(msg_init);
    delay_ms(5);
}


/*
int main(void)
{
  WDOG_disable();       
  SOSC_init_8MHz();     
  SPLL_init_160MHz();    
  NormalRUNmode_80MHz();
  SystemCoreClockUpdate();
	
	char msg_Start = 0xA2;
	char msg_init =0x00;
	char msg_reset =0xA0;
	char msg_clear = 0xA3;
	
	unsigned char msg_array[10]={0xB6,0x81,0x9F,0xA1,0x00};
	uint32_t dtime=5;
	
	PORT_init();           
  LPUART1_init();	
	
	while(1){

		LPUART1_transmit_char(msg_init);  	
	
		LPUART1_transmit_char(msg_Start);  	
		LPUART1_transmit_char(msg_array[0]);  	
		LPUART1_transmit_char(msg_array[1]);  	
		LPUART1_transmit_char(msg_array[2]);
		LPUART1_transmit_char(msg_array[3]);  				
		LPUART1_transmit_char(msg_array[4]);  		
		delay_ms(dtime);
		
		LPUART1_transmit_char(msg_reset);  			
	}	
}
*/