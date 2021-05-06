#include "device_registers.h"
#include "clocks_and_modes.h"
#include "LPUART.h"
#include <stdio.h>

int i=0, num=34563456;
char data = 0;
char transfer_data[20];
int j=0;
int lpit0_ch0_flag_counter = 0;

void PORT_init(void)
{
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;

    PORTC->PCR[8] |= PORT_PCR_MUX(2);
    PORTC->PCR[9] |= PORT_PCR_MUX(2);
    
    PCC->PCCn[PCC_PORTA_INDEX] |= PCC_PCCn_CGC_MASK;
    PORTA->PCR[3] |= PORT_PCR_MUX(6);
}

void WDOG_disable(void)
{
    WDOG->CNT=0xD928C520;
    WDOG->TOVAL=0x0000FFFF;
    WDOG->CS = 0x00002100;
}

void LPIT0_init(uint32_t delay)
{
    uint32_t timeout;
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;

    LPIT0 -> MCR |= LPIT_MCR_M_CEN_MASK;

    timeout = delay * 40000;
    LPIT0->TMR[0].TVAL = timeout;
    LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
}

void delay_ms (volatile int ms)
{
    LPIT0_init(ms);
    while(0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK)){}
            lpit0_ch0_flag_counter++;
            LPIT0->MSR |= LPIT_MSR_TIF0_MASK;
}

void LPUART0_init(void)
{
    PCC->PCCn[PCC_LPUART0_INDEX] &= ~PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_LPUART0_INDEX] |= PCC_PCCn_PCS(0x001) | PCC_PCCn_CGC_MASK;

    LPUART0->BAUD = 0x0F000068; /* 4800 BAUD */

    LPUART0->CTRL = 0x000C0000;
}

void LPUART0_transmit_char(char send)
{
    while((LPUART0->STAT & LPUART_STAT_TDRE_MASK)>>LPUART_STAT_TDRE_SHIFT==0);

    LPUART0->DATA=send;
}

void LPUART0_transmit_string(char data_string[])
{
    uint32_t i=0;
    while(data_string[i] != '\0')
    {
        LPUART0_transmit_char(data_string[i]);
        i++;
    }
}

char LPUART0_receive_char(void)
{
    static char receive;
    while((LPUART0->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT==0);

    receive = LPUART0 -> DATA;
    return receive;
}

int main(void)
{
    char buffer = 0;
    char array_data[20]="";
    int i =0;
     
    char msg_init = 0x00;
    char msg_clear = 0xA3;
    char msg_reset = 0xA0;
    char msg_Start = 0xA2;
    
    WDOG_disable();
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    SystemCoreClockUpdate();


    PORT_init();
    LPUART0_init();
    LPUART1_init();

    LPUART1_transmit_string("Running LPUART example\n\r");
    LPUART1_transmit_string("Input character to echo...\n\r");
 
    for(;;)
    {
        LPUART1_transmit_char('>');
        buffer=LPUART1_receive_and_echo_char();

        LPUART1_transmit_char(buffer);
        LPUART1_transmit_char("\n\r");

        LPUART0_transmit_char(msg_Start);
        delay_ms(5);
        LPUART0_transmit_char(buffer);
        LPUART0_transmit_char(msg_init);
        delay_ms(5);
    }
}

