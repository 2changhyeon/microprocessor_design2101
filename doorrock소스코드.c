

///////////////////////////////////////////////////////
////////////////////3�� ������ �����/////////////////
/////////////////////////////////////////////////////


#include<iom128.h>
#include<ina90.h>

#define ENABLE  (PORTA_Bit2=1)
#define DISABLE (PORTA_Bit2=0) 

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) 
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) 
#define TESTBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 

int key_IN[8]; //�ԷµǴ� Ű�� ����迭
int sec_num[8]={0x32,0x30,0x30,0x32,0x31,0x32,0x38,0x32};  //��й�ȣ
int arr_cnt=1, right=1;  //��й�ȣ �˻縦 ���� ���� ����
int g=0, s=0;  //��й�ȣ �˻縦 ���� ��������
int i=0;  //LCD����� ���� ����

////////////////////////////////////////////////////////////
////////////////LCD����� ���� �Լ���///////////////////////
///////////////////////////////////////////////////////////
void init_PORTA(void)
{
	DDRA = 0xFF;    // PORTA ���
	PORTA=0xFF;     // �ʱ� ��
}


void wait_busy_flag(void)
{
	unsigned int i;
	
	for(i=0;i<10000;i++);
} 

  
void m_delay(unsigned int m) //������ ��ƾ
{
	unsigned int i, j;
	  
	for(i=0;i<m;i++)
	   for(j=0;j<2650;j++);  //16MHz : 1msec
}

void ddelay(unsigned int p)
{
	unsigned int i;

	for(i=0;i<p;i++);
}

void instruction_out(unsigned char b) //��ɾ� ���� �Լ�
{
	PORTA=b&0xF0;                    //���� 4BIT ��� 

	ENABLE;                          //ENABLE
	DISABLE;                         //DISABLE
	
	PORTA=(b<<4)&0xF0;               //���� 4BIT ���
	
	ENABLE;                          //ENABLE
	DISABLE;                         //DISABLE
	
	//��ɾ� ������ �Ϸ�ɶ����� ����
	m_delay(1);

}


void char_out(unsigned char b)  //LCD�� �ѹ��� ��� �Լ�
{
   
	PORTA=(b&0xF0)|0x01;        //���� 4BIT ���
	
	ENABLE;                     //ENABLE
	DISABLE;                    //DISABLE 
	
	PORTA=((b<<4)&0xF0)|0x01;   //���� 4BIT ���
	
	ENABLE;                     //ENABLE
	DISABLE;                    //DISABLE

   //��ɾ� ������ �Ϸ�ɶ����� ����
    m_delay(1);
}


void string_out(unsigned char b, unsigned char *str) //���ڿ� ��� �Լ�
{
	unsigned int i=0;
	
	instruction_out(b);   //LCD ��ġ ����
	
	do{
	    char_out(str[i]);
	   }while(str[++i]!='\0');   // NULL ���ڸ� ���� �� ����
}


void init_LCD(void)                //LCD�ʱ�ȭ ��ƾ
{   
	init_PORTA();    // LCD�� ������ ��Ʈ �ʱ�ȭ

  	m_delay(8);                         
	            
	ddelay(300);			  
	instruction_out(0x28);  //LCD FUNCTION SET(16X2 LINE, 4 BIT, 5X8 DOT)            
    m_delay(1); 
    instruction_out(0x28);   //LCD FUNCTION SET(16X2 LINE, 4 BIT, 5X8 DOT)    
	m_delay(1); 
	instruction_out(0x0C);   //LCD DISPLAY ON, CURSOR OFF, BLINK OFF 
	instruction_out(0x06);   //LCD ENTRY MODE SET
	instruction_out(0x02);   //RETURN HOME
	instruction_out(0x01);   //LCD CLEAR
	instruction_out(0x01);   //LCD CLEAR 
}




void lcd_decimal(unsigned char b, unsigned int hex)
{
	unsigned int h;
	
	instruction_out(b);
	
	h = (hex / 1000);
	
	if(h != 0)
		char_out(h + 0x30);
 	 else
	   	char_out(0x30);
		
	h = ((hex % 1000) / 100);
	
	if(h != 0)
		char_out(h + 0x30);
	  else
		char_out(0x30);
		
	h = (((hex % 1000) % 100) /10);
	
	if(h != 0)
		char_out(h + 0x30);
	  else
		char_out(0x30);
		
	h= (((hex % 1000) % 100) % 10);
	
	if(h != 0)
		char_out(h + 0x30);
	  else
		char_out(0x30);
}

void lcd2hex(unsigned char ch)
{
	unsigned char ddata=0x00, dch;

    dch = (ch>>4)&0x0f;
	if( dch <= 9 ) ddata = '0'+dch; 
	else ddata = 'A'+(dch-10); 
	char_out(ddata);

	dch = ch&0x0f;
	if( dch <= 9 ) ddata = '0'+dch; 
	else ddata = 'A'+(dch-10); 
	char_out(ddata);
}

void lcd4hex(unsigned char b, unsigned int ch)
{
	unsigned char dch;
        
    instruction_out(b);
        
	dch = (ch>>8)&0xff;
	lcd2hex(dch);
	dch = ch&0xff;
	lcd2hex(ch);	
}

////////////////////////////////////////////////////////
//////////////Ű�е� ������ �Լ�////////////////////////
///////////////////////////////////////////////////////
void delay(unsigned long k)    
{ 
  unsigned long i; 
  for(i=k;i>0;i--); 
}
 
/////////////////////////////////////////////////////////
///////////////Ű��ĵ �Լ�///////////////////////////////
////////////////////////////////////////////////////////
unsigned char Key(void)                 
{
PORTC = 0x01;
delay(5000);  //��Ʈ�� ����� �����ϰ� �ٲ��Ŀ� �񱳸� �ϱ����� ������
{
if((PINC & 0x10) == 0x10)
return 0x30;    // 0

else if((PINC & 0x20) == 0X20)
return 0x34;   // 4

else if((PINC & 0x40) == 0X40)
return 0x38; // 8

else if((PINC & 0x80) == 0X80)
return 0x41;  // A
}


PORTC = 0x02;
delay(5000);  //��Ʈ�� ����� �����ϰ� �ٲ��Ŀ� �񱳸� �ϱ����� ������
{
if((PINC & 0x10) == 0x10)
return 0x31;  // 1

else if((PINC & 0x20) == 0X20)
return 0x35;   // 5

else if((PINC & 0x40) == 0X40)
return 0x39; // 9

else if((PINC & 0x80) == 0X80)
return 0x42;  // B
}


PORTC = 0x04;
delay(5000);  //��Ʈ�� ����� �����ϰ� �ٲ��Ŀ� �񱳸� �ϱ����� ������
{
if((PINC & 0x10) == 0x10)
return 0x32; // 2

else if((PINC & 0x20) == 0X20)
return 0x36;   // 6

else if((PINC & 0x40) == 0X40)
return 0x2a; // *
   
else if((PINC & 0x80) == 0X80)
return 0x43;  // C
}


PORTC = 0x08;
delay(5000);  //��Ʈ�� ����� �����ϰ� �ٲ��Ŀ� �񱳸� �ϱ����� ������
{
if((PINC & 0x10) == 0x10)
return 0x33; // 3

else if((PINC & 0x20) == 0X20)
return 0x37;   // 7

else if((PINC & 0x40) == 0X40)
return 0x23; // #

else if((PINC & 0x80) == 0X80)
return 0x44;  // D
}

return 0xff; 
}

/////////////////////////////////////////////////////////
///////////////////���ͱ����Լ�//////////////////////////
/////////////////////////////////////////////////////////

void dc_motor(unsigned char dir, unsigned int speed)
{	
    if(dir==1)           // ������
		PORTB |= 0x40;   // ���� ���� DIR(������)
     else
        PORTB &= 0xBF;   // ���� ���� DIR(������)

    OCR1A=speed;
}

////////////////////////////////////////////////////////////
///////////////////////��й�ȣ Ȯ�� �Լ�//////////////////
//////////////////////////////////////////////////////////

void sec_test(unsigned char b)
{
  if(arr_cnt==8)
  {
    for(g=0;g<8;g++)
    {
      if(key_IN[g]==sec_num[g])
        right++;
    }
    if(right==8)
    {
      instruction_out(0x01);
      string_out(0x80, "OPEN");
      dc_motor(0,0x3ff);
      m_delay(500);
      OCR1A=0x000;
               
      instruction_out(0x01);                // LCD�� �����.
      string_out(0x80, "Password");        // LCD�� ù��° ����
      
      arr_cnt=1;
      right=1;
      g=0; 
      s=0;
      i=0;
    }
    
    else
    {
      instruction_out(0x01);
      string_out(0x80, "Retry");
      m_delay(500);
     
      instruction_out(0x01);                // LCD�� �����.
      string_out(0x80, "Password");        // LCD�� ù��° ����
      
      arr_cnt=1;
      right=1;
      g=0; 
      s=0;
      i=0;
     }
  }
  else
  {
      key_IN[s]=b;
      s++;
      arr_cnt++;
  }
}



/////////////////////////////////////////////////////////////
///////////////////////////���� �Լ�////////////////////////
///////////////////////////////////////////////////////////

void main(void)
{
    unsigned char key=0xff; 
    
    DDRB |= 0xE0;    // 111* **** -> ������* ****
    PORTB=0xFF;
    DDRC=0x0f; 
    PORTC = 0x00; 
    init_LCD();

    instruction_out(0x01);                // LCD�� �����.
 
       
      
    TCCR1A |= 0x83;  // (10** **11)
    TCCR1B |= 0x02;  // (***0 0010) Phase correct PWM 10bit, Non-inverting, 8 prescaler
                     // f=16MHz/(2*N*0x3FF), N=8 prescaler factor  
                     // f= 997Hz, T=1.003msec
				     // PWM�� �ֱ�� 1.003msec�� �ȴ�.
    TCCR1C=0x00;     // FOC1A=0
    
    TCNT1=0x0000;    // T/C1 value=0
    OCR1A=0x000;

    
    string_out(0x80, "Password");        // LCD�� ù��° ����
    PORTB |= 0x80;   // Motor Enable
    m_delay(1);  // 1�� ���
   
   
    DDRE_Bit4=0;   // PE4 �Է� DDRE4 ��Ʈ�� �ʱⰪ�� 0���� �����Ǿ� �ֱ� ������
	               // "DDRE_Bit4=0;"�� ���� �����ϴ�. INT4(PE4)��
				   // DDRE&=0xEF;

    EIMSK|=0x10;   // INT4 ���� ���ͷ�Ʈ �ο��̺�
    EICRB|=0x03;   // INT4 ��� ���� 
    EIFR|=0x10;     // INTF4 Ŭ���� 
   
     __enable_interrupt();    // Global interrupt enable
                              // �Ǵ� _SEI(); ����
     
     
    while(1) 
    {     
      
      key=Key();                         // Ű �Լ��� ���ϰ��� Ű�� ����
      if(key!=0xff)                      // �׷��� �� Ű�� ���� 0xff�� �ƴ϶��?
      {
      instruction_out(0xc0+i);           // LCD�� ��� ��ġ
      delay(1000); 
      char_out(key);                      // ���� Ű ���� ����ϴ� �Լ�.
      m_delay(500);
      i++;
      
      sec_test(key);                       //��й�ȣ Ȯ�� �Լ�
      }
    }
}   
    
    #pragma vector=INT4_vect              // iom128.h�� �����Ѵ�. 
__interrupt void INT4_interrrupt(void)   
{
    __disable_interrupt();   
     
  dc_motor(1,0x3ff);
      m_delay(1000);
      OCR1A=0x000;
      
      instruction_out(0x01);                // LCD�� �����.
      string_out(0x80, "Password");        // LCD�� ù��° ����
      
      arr_cnt=1;
      right=1;
      g=0; 
      s=0;
      i=0;
            
    __enable_interrupt();
}
