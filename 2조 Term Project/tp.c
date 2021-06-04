#include "device_registers.h"
#include "clocks_and_modes.h"
#include "LPUART.h"
#include <stdio.h>
#include <stdlib.h>

unsigned int FND_DATA[10]={0x3F, 0x06,0x5b,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x67};
unsigned int FND_SEL[5]={0x2000,0x4000,0x8000,0x10000};
unsigned int FND_ENG[13]={0b01110001,0b01110111,0b00110000, 0b00111000, 0b01011100,0b01110011,0b01111001, 0b01010101, 0b01011110, 0b01101010, 0b01111000,0b01010100};//failopemdwtN

unsigned int num=0,num0=0,num1=0,num2=0,num3=0;
unsigned int Dtime= 0;
unsigned int uart1 = 0;

int i=0;
char data = 0;
char transfer_data[20];
int j=0;
int lpit0_ch0_flag_counter = 0;

unsigned int mode = 0;
unsigned int pw = 1234;
unsigned int pw1 = 0;
unsigned int pw2 = 0;
unsigned int number = 0;
unsigned int money = 5000;
unsigned int wait = 0;


int num4 = 0;


void PORT_init(void)
{
	  PCC->PCCn[PCC_PORTA_INDEX]|=PCC_PCCn_CGC_MASK;
		PCC->PCCn[PCC_PORTC_INDEX]|=PCC_PCCn_CGC_MASK; /* EnableclockforPORTC */
		PCC->PCCn[PCC_PORTD_INDEX]|=PCC_PCCn_CGC_MASK; /* EnableclockforPORTD */
		PCC->PCCn[PCC_PORTE_INDEX]|=PCC_PCCn_CGC_MASK; /* EnableclockforPORTE */
		
		PTD->PDDR |= 1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7;/* PortD0~7: FND Data Direction= output*/
		PTE->PDDR |= 1<<13|1<<14|1<<15|1<<16; /*FND SelectDirection*/
    PTC->PDDR &= ~(1<<4);

		PORTC->PCR[4] |= PORT_PCR_MUX(1);	
		PORTC->PCR[4] |=(10<<16);
	
		PORTC->PCR[8] |= PORT_PCR_MUX(2);
   	PORTC->PCR[9] |= PORT_PCR_MUX(2);
    
    PORTA->PCR[3] |= PORT_PCR_MUX(6);
	
	
		PORTD->PCR[0] = PORT_PCR_MUX(1);/* PortD0: MUX = GPIO */
		PORTD->PCR[1] = PORT_PCR_MUX(1);/* PortD1: MUX = GPIO */
		PORTD->PCR[2] = PORT_PCR_MUX(1);/* PortD2: MUX = GPIO */
		PORTD->PCR[3] = PORT_PCR_MUX(1);/* PortD3: MUX = GPIO */
		PORTD->PCR[4] = PORT_PCR_MUX(1);/* PortD4: MUX = GPIO */
		PORTD->PCR[5] = PORT_PCR_MUX(1);/* PortD5: MUX = GPIO */
		PORTD->PCR[6] = PORT_PCR_MUX(1);/* PortD6: MUX = GPIO */
		PORTD->PCR[7] = PORT_PCR_MUX(1);/* PortD7: MUX = GPIO */

		PORTE->PCR[13] = PORT_PCR_MUX(1); /*PortD13: mux= GPIO */	
		PORTE->PCR[14] = PORT_PCR_MUX(1); /*PortD14: mux= GPIO */
		PORTE->PCR[15] = PORT_PCR_MUX(1); /*PortD15: mux= GPIO */
		PORTE->PCR[16] = PORT_PCR_MUX(1); /*PortD16: mux= GPIO */
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

void NVIC_init_IRQs(void){
	S32_NVIC->ICPR[1] |= 1<<(61%32); // ClearanypendingIRQ61
	S32_NVIC->ISER[1] |= 1<<(61%32); // EnableIRQ61
	S32_NVIC->IP[61] =0xB; //Priority11 of 15
}


void PORTC_IRQHandler(void)
{
	PORTC->PCR[4] &= ~(0x01000000);

	if(((PORTC->ISFR&(1<<4)) != 0))
	{
		delay_ms(10);
		num4 += 1;
		
		if(num4 % 2 == 1)
		{
			wait = 1;	
		}
		
		else if(num4 % 2 == 0)
		{
			wait = 0;
		}
	}
	PORTC->PCR[4] |= 0x01000000;
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

void fnd_fail()
{
	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[0];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[1];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[2];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;
	
	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[3];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j=0;
}

void fnd_open()
{
	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[4];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[5];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[6];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;
	
	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[11];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j=0;
}

void fnd_mode()
{
	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[7];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[4];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[8];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;
	
	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[6];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j=0;
}

void fnd_wait()
{

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[9];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[1];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[2];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;
	
	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_ENG[10];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j=0;
}


void fnd_sucess()
{
	unsigned int FND_SUCDATA[15]={0b00000000,0b00000000,0b00000000,0b00000000,0b01101101,0b00011100,0b01011000,0b01011000,0b01111001,0b01101101,0b01101101,0b00000000,0b00000000,0b00000000,0b00000000};//off s u c c e s s
	for(int x = 1; x < 12; x++){
		for(int y = 0; y<4 ; y++){
		
			
			PTE->PSOR = FND_SEL[y];
			PTD->PCOR = FND_SUCDATA[x+y];
			delay_ms(100);
			PTD->PSOR =0xff;
			PTE->PCOR = 0xfffff;
		}
	}
}

void FND_select(int num)
{
	num3=(num/1000) % 10;
	num2=(num/100) % 10;
	num1=(num/10) % 10;
	num0= num % 10;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_DATA[num3];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_DATA[num2];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;

	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_DATA[num1];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j++;
	
	PTE->PSOR = FND_SEL[j];
	PTD->PCOR = FND_DATA[num0];
	delay_ms(Dtime/300);
	PTD->PSOR =0xff;
	PTE->PCOR = 0xfffff;
	j=0;
}

void use_uart(void)
{   
    char msg_init = 0x00;
    char msg_clear = 0xA3;
    char msg_reset = 0xA0;
    char msg_Start = 0xA2;

    char buffer = 0;
    char array_data[20]="";

    LPUART1_transmit_char('>');
	
    buffer=LPUART1_receive_and_echo_char();
    switch (buffer)
    {
    case '0':
				number = (number*10);
        break;
		
		case '1':
				number = (number*10)+ 1;
        break;
		
		case '2':
				number = (number*10)+ 2;
        break;
				
		case '3':
				number = (number*10)+ 3;
        break;
						
		case '4':
				number = (number*10)+ 4;
        break;
								
		case '5':
				number = (number*10)+ 5;
        break;
										
		case '6':
				number = (number*10)+ 6;
        break;
	
		case '7':
				number = (number*10)+ 7;
        break;
														
		case '8':
				number = (number*10)+ 8;
        break;
																
		case '9':
				number = (number*10)+ 9;
        break;
	
		case 'q':
        mode = 1;
        break;
	
		case 'w':
        mode = 2;
        break;
	
		case 'e':
				mode = 3;
        break;
	
		case 'r':
				mode = 4;
        break;
	
		case 't':
				uart1 = 1;
						
        break;
				
		default:
        break;
    }
		
		
    
           
    LPUART1_transmit_char(buffer);		
    LPUART1_transmit_char("\n\r");
		/*
    LPUART0_transmit_char(msg_Start);
    delay_ms(5);
    LPUART0_transmit_char(number);
    LPUART0_transmit_char(msg_init);
    delay_ms(5);
		*/
}


int main(void)
{
    WDOG_disable();
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    SystemCoreClockUpdate();
		NVIC_init_IRQs(); /*InterruptPending, Endable, PrioritySet*/

    PORT_init();
    LPUART0_init();
    LPUART1_init();
		Dtime = 500;
   // LPUART1_transmit_string("Running LPUART example\n\r");
   // LPUART1_transmit_string("Input character to echo...\n\r"); 

 for(;;)
	{
    for(;;)
    {
        if(wait == 0)
        {
          fnd_wait();
					number = 0;
					uart1 = 0;
        }

        else if(wait == 1)
        {		
					use_uart();
					pw1 = number;
				}
				if(uart1 == 1)
				{
					break;
				}
		}
		
		switch(mode)
		{
			case 1:
				number = 0;
				uart1 = 0;
				for(;;)
				{
					if(pw != pw1)
					{
						break;
					}
					else if(pw == pw1)
					{
						use_uart();													
					}
					if(uart1 == 1)
					{
						money += number;
						break;
					}				
				}
				break;
				
			case 2:
				number = 0;
				uart1 = 0;
				for(;;)
				{
					if(pw != pw1)
					{
						break;
					}
					else if(pw == pw1)
					{
						use_uart();													
					}
					if(uart1 == 1)
					{
						money -= number;
						break;
					}
					
				}
				break;
				
			case 3:
				number = 0;
				uart1 = 0;
				for(;;)
				{
					if(pw != pw1)
					{
						break;
					}
					else if(pw == pw1)
					{
						use_uart();												
					}
					if(uart1 == 1)
					{
						pw2 = number;
						break;
					}				
				}
				break;
			default :
					break;
				
		}
		
		for(;;)
		{		
				if(pw != pw1)
				{
					fnd_fail();
				}
				else if(mode == 3)
				{
					pw = pw2;
					fnd_sucess();
				}
				else if(money <0)
				{
					fnd_fail();
				}
				else if(money > 9999)
				{
					fnd_fail();				
				}
				else
				{
				FND_select(money);
				}
				
			  if(wait == 0)
        {
					break;
        }			
		}
	}        		
}


