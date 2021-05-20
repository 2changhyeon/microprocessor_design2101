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
	while(0==(LPIT0->MSR & LPIT_MSR_TIF0_MASK)) {}
	lpit0_ch0_flag_counter++;
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK;
}

void FTM_init(void)
{
	PCC->PCCn[PCC_FTM0_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_FTM0_INDEX] |= PCC_PCCn_PCS(0b010) | 	PCC_PCCn_CGC_MASK;
	
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

}

void PORT_init(void)
{
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
	PORTD->PCR[16] |= PORT_PCR_MUX(2);
}


int main(void)
{
  uint32_t adcResultInMv=0;
	int flag=0;
	SOSC_init_8MHz();
	SPLL_init_160MHz();
	NormalRUNmode_80MHz();

	FTM_init();
	PORT_init();
	ADC_init();


	for(;;)
	{
		convertAdcChan(12);
		while(adc_complete()==0){}
    adcResultInMv=read_adc_chx();
		duty=adcResultInMv*1.6;
		FTM0_CH1_PWM(duty);	
	}
}