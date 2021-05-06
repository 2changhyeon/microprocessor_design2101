#include "device_registers.h"
#include "clocks_and_modes.h"
#include "LPUART.h"
#include <stdio.h>

int i=0, num=34563456;
char data = 0;
char transfer_data[20];
int j=0;

void PORT_init(void)
{
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;

    PORTC->PCR[8] |= PORT_PCR_MUX(2);
    PORTC->PCR[9] |= PORT_PCR_MUX(2);
}

void WDOG_disable(void)
{
    WDOG->CNT=0xD928C520;
    WDOG->TOVAL=0x0000FFFF;
    WDOG->CS = 0x00002100;
}



int main(void)
{
    WDOG_disable();
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    SystemCoreClockUpdate();


    PORT_init();
    LPUART1_init();
    LPUART1_transmit_string("\n\rRunning LPUART example\n\r");
    LPUART1_transmit_string("Display Variable Start...\n\r");

  
		for(;;)
    {		
        while(transfer_data[i] != '\0')
        {
            LPUART1_transmit_char(transfer_data[i]);
            i++;
        }
        i=0;

        LPUART1_transmit_char("\r\n");
        transfer_data[20]="";
        sprintf(transfer_data,"%d",num);
				LPUART1_transmit_char('\0');
				
    }
}
