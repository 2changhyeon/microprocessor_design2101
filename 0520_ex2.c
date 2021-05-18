#include "device_registers.h"
#include "clocks_and_modes.h"
#include "ADC.h"
int lpit0_ch0_flag_counter = 0;
int duty = 0;

void LPIT0_init (uint32_t delay)
{
	uint32_t timeout;
	
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;

	LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;

	timeout = delay * 40;
	LPIT0->TMR[0].TVAL = timeout;
	LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
}

void delay_us(volatile int us)
{
	LPIT0_init(us);
	while(0==(LPIT->MSR & LPIT_MSR_TIF0_MASK)) {}
	lpit0_ch0_flag_counter++;
	LPIT->MSR |= LPIT_MSR_TIF0_MASK;
}

void FTM_init(void)
{
	PCC->PCCn[PCC_FTM0_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_FTM0_INDEX] |= PCC_PCCn_PCS(0b010) | 					PCC_PCCn_CGC_MASK;
	
	FTM0->SC = FTM_SC_PWMEN1_MASK | FTM_SC_PS(1);

	FTM0->MOD = 8000-1;

	FTM0->CNTIN = FTM_CNTIN_INIT(0);

	FTM0->CONTROLS[1].CnSC |= FTM_CnSC_MSB_MASK;

	FTM0->CONTROLS[1].CnSC |= FTM_CnSC_ELSA_MASK;

}

void FTM0_CH1_PWM (int i)
{
	FTM0->CONTROLS[1].CnV = i;
	FTM0->SC|=FTM_SC_CLKS(3);
/*
FTM0->SC = FTM_SC_PWMEN1_MASK|FTM_SC_PS(1);
//Enable PWM channel1 output, define 0x20000u
// PS bit ¡®1¡¯ set, Divide by 2
// Define on S32K144.h
FTM0->MOD = 8000-1;
// FTM0 counter final value (used for PWM mode)
// FTM0 Period = MOD-CNTIN+0x0001~=16000 ctr clks=8ms
// 8Mhz /2 =4MHz
Define On S32K144.h
#define FTM_CNTIN_INIT 
(((uint32_t)(((uint32_t)(x))<<FTM_CNTIN_INIT_SHIFT))&FTM_CNTIN_INIT_MASK)
FTM_CNTIN_INIT_SHIFT = 0x0u
FTM_CNTIN_INIT_MASK = 0xFFFFu
*/

}

void PORT_init(void)
{
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
	PORTD->PCR[16] |= PORT_PCR_MUX(2);
}
void  ADC_init()
{
//
}
int main(void)
{
	int flag=0;
	SOCS_init_8MHz();
	SPLL_init_160MHz();
	NormalRUNmode_80MHz();

	FTM_init();
	PORT_init();
	ADC_init();


	for(;;)
	{
		covertAdcChan(12);
		while(adc_complete()==0){}
		duty=adcResultInMv*1.6;
		FTM0_CH1_PWM(duty);	
	}
}