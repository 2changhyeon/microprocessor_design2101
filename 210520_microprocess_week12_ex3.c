#include "device_registers.h"
#include "clocks_and_modes.h"
#include "ADC.h"
#include "math.h"

int lpit0_ch0_flag_counter = 0;
int duty = 0;

void FTM_init(void)
{
	PCC->PCCn[PCC_FTM0_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_FTM0_INDEX] |= PCC_PCCn_PCS(0b010) | 	PCC_PCCn_CGC_MASK;
	
	FTM0->SC = FTM_SC_PWMEN0_MASK |FTM_SC_PWMEN1_MASK |FTM_SC_PWMEN2_MASK | FTM_SC_PS(1);

	FTM0->MOD = 8000-1;

	FTM0->CNTIN = FTM_CNTIN_INIT(0);

	FTM0->CONTROLS[0].CnSC |= FTM_CnSC_MSB_MASK;
	FTM0->CONTROLS[0].CnSC |= FTM_CnSC_ELSA_MASK;

  FTM0->CONTROLS[1].CnSC |= FTM_CnSC_MSB_MASK;
	FTM0->CONTROLS[1].CnSC |= FTM_CnSC_ELSA_MASK;

  FTM0->CONTROLS[2].CnSC |= FTM_CnSC_MSB_MASK;
	FTM0->CONTROLS[2].CnSC |= FTM_CnSC_ELSA_MASK;

}

void FTM0_CH0_PWM (int i)
{
	FTM0->CONTROLS[0].CnV = i;
	FTM0->SC|=FTM_SC_CLKS(3);

}

void FTM0_CH1_PWM (int i)
{
	FTM0->CONTROLS[1].CnV = i;
	FTM0->SC|=FTM_SC_CLKS(3);

}

void FTM0_CH2_PWM (int i)
{
	FTM0->CONTROLS[2].CnV = i;
	FTM0->SC|=FTM_SC_CLKS(3);

}

void PORT_init(void)
{
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

	PORTD->PCR[0] |= PORT_PCR_MUX(2);
	PORTD->PCR[15] |= PORT_PCR_MUX(2);
	PORTD->PCR[16] |= PORT_PCR_MUX(2);
}

int main(void)
{
    uint32_t adcResultInMv=0;
    int R1=0;
    int G1=0;
    int B1=0;
    float Pi=3.141592653589;
    float Lotate=0; 
    float H, S, I;
    float r, g, b;

    SOSC_init_8MHz(); 
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
   
    FTM_init();
    ADC_init(); 
    PORT_init(); 

    for(;;)
    {
		convertAdcChan(12);
		while(adc_complete()==0){}
        adcResultInMv=read_adc_chx();
		Lotate=adcResultInMv/13.88888889;

		H=Lotate/45;
        I=1;
        S=0.5;

        if(S>1) S=1;
        if(I>1) I=1;
        if(S==0) r=g=b=I;
        else
        {

            if ( (H >= 0) && (H <2*Pi/3) )
	        { 
                b=(1-S)/3;
	            r=(1+S*cos(H)/cos(Pi/3-H))/3;
	            g=1-r-b;
	        }
            else if ( (H>=2*Pi/3) && (H<4*Pi/3) )
	        { 
                H=H-2*Pi/3;
	            r=(1-S)/3;
	            g=(1+S*cos(H)/cos(Pi/3-H))/3;
	            b=1-r-g;
	        } 
            else if ( (H>=4*Pi/3) && (H<2*Pi) )
	        {
                H=H-4*Pi/3;
	            g=(1-S)/3;
	            b=(1+S*cos(H)/cos(Pi/3-H))/3;
	            r=1-b-g;
	        }

					      if(r<0) r=0;
                if(g<0) g=0;
                if(b<0) b=0;

        }
        

        R1=r*8000*I;
        G1=g*8000*I;
        B1=b*8000*I;

        FTM0_CH0_PWM(R1);
        FTM0_CH1_PWM(G1);
        FTM0_CH2_PWM(B1);                
    }
}
