

///////////////////////////////////////////////////////
////////////////////3조 디지털 도어락/////////////////
/////////////////////////////////////////////////////


#include<iom128.h>
#include<ina90.h>

#define ENABLE  (PORTA_Bit2=1)
#define DISABLE (PORTA_Bit2=0) 

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) 
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) 
#define TESTBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 

int key_IN[8]; //입력되는 키값 저장배열
int sec_num[8]={0x32,0x30,0x30,0x32,0x31,0x32,0x38,0x32};  //비밀번호
int arr_cnt=1, right=1;  //비밀번호 검사를 위한 변수 선언
int g=0, s=0;  //비밀번호 검사를 위한 변수선언
int i=0;  //LCD출력을 위한 변수

////////////////////////////////////////////////////////////
////////////////LCD출력을 위한 함수들///////////////////////
///////////////////////////////////////////////////////////
void init_PORTA(void)
{
	DDRA = 0xFF;    // PORTA 출력
	PORTA=0xFF;     // 초기 값
}


void wait_busy_flag(void)
{
	unsigned int i;
	
	for(i=0;i<10000;i++);
} 

  
void m_delay(unsigned int m) //딜레이 루틴
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

void instruction_out(unsigned char b) //명령어 쓰기 함수
{
	PORTA=b&0xF0;                    //상위 4BIT 출력 

	ENABLE;                          //ENABLE
	DISABLE;                         //DISABLE
	
	PORTA=(b<<4)&0xF0;               //하위 4BIT 출력
	
	ENABLE;                          //ENABLE
	DISABLE;                         //DISABLE
	
	//명령어 수행이 완료될때까지 지연
	m_delay(1);

}


void char_out(unsigned char b)  //LCD에 한문자 출력 함수
{
   
	PORTA=(b&0xF0)|0x01;        //상위 4BIT 출력
	
	ENABLE;                     //ENABLE
	DISABLE;                    //DISABLE 
	
	PORTA=((b<<4)&0xF0)|0x01;   //하위 4BIT 출력
	
	ENABLE;                     //ENABLE
	DISABLE;                    //DISABLE

   //명령어 수행이 완료될때까지 지연
    m_delay(1);
}


void string_out(unsigned char b, unsigned char *str) //문자열 출력 함수
{
	unsigned int i=0;
	
	instruction_out(b);   //LCD 위치 지정
	
	do{
	    char_out(str[i]);
	   }while(str[++i]!='\0');   // NULL 문자를 만날 때 까지
}


void init_LCD(void)                //LCD초기화 루틴
{   
	init_PORTA();    // LCD를 연결한 포트 초기화

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
//////////////키패드 딜레이 함수////////////////////////
///////////////////////////////////////////////////////
void delay(unsigned long k)    
{ 
  unsigned long i; 
  for(i=k;i>0;i--); 
}
 
/////////////////////////////////////////////////////////
///////////////키스캔 함수///////////////////////////////
////////////////////////////////////////////////////////
unsigned char Key(void)                 
{
PORTC = 0x01;
delay(5000);  //포트의 출력이 완전하게 바뀐후에 비교를 하기위한 딜레이
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
delay(5000);  //포트의 출력이 완전하게 바뀐후에 비교를 하기위한 딜레이
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
delay(5000);  //포트의 출력이 완전하게 바뀐후에 비교를 하기위한 딜레이
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
delay(5000);  //포트의 출력이 완전하게 바뀐후에 비교를 하기위한 딜레이
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
///////////////////모터구동함수//////////////////////////
/////////////////////////////////////////////////////////

void dc_motor(unsigned char dir, unsigned int speed)
{	
    if(dir==1)           // 정방향
		PORTB |= 0x40;   // 모터 방향 DIR(정방향)
     else
        PORTB &= 0xBF;   // 모터 방향 DIR(역방향)

    OCR1A=speed;
}

////////////////////////////////////////////////////////////
///////////////////////비밀번호 확인 함수//////////////////
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
               
      instruction_out(0x01);                // LCD를 지운다.
      string_out(0x80, "Password");        // LCD의 첫번째 라인
      
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
     
      instruction_out(0x01);                // LCD를 지운다.
      string_out(0x80, "Password");        // LCD의 첫번째 라인
      
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
///////////////////////////메인 함수////////////////////////
///////////////////////////////////////////////////////////

void main(void)
{
    unsigned char key=0xff; 
    
    DDRB |= 0xE0;    // 111* **** -> 출출출* ****
    PORTB=0xFF;
    DDRC=0x0f; 
    PORTC = 0x00; 
    init_LCD();

    instruction_out(0x01);                // LCD를 지운다.
 
       
      
    TCCR1A |= 0x83;  // (10** **11)
    TCCR1B |= 0x02;  // (***0 0010) Phase correct PWM 10bit, Non-inverting, 8 prescaler
                     // f=16MHz/(2*N*0x3FF), N=8 prescaler factor  
                     // f= 997Hz, T=1.003msec
				     // PWM의 주기는 1.003msec가 된다.
    TCCR1C=0x00;     // FOC1A=0
    
    TCNT1=0x0000;    // T/C1 value=0
    OCR1A=0x000;

    
    string_out(0x80, "Password");        // LCD의 첫번째 라인
    PORTB |= 0x80;   // Motor Enable
    m_delay(1);  // 1초 대기
   
   
    DDRE_Bit4=0;   // PE4 입력 DDRE4 비트는 초기값이 0으로 설정되어 있기 때문에
	               // "DDRE_Bit4=0;"는 생략 가능하다. INT4(PE4)핀
				   // DDRE&=0xEF;

    EIMSK|=0x10;   // INT4 개별 인터럽트 인에이블
    EICRB|=0x03;   // INT4 상승 에지 
    EIFR|=0x10;     // INTF4 클리어 
   
     __enable_interrupt();    // Global interrupt enable
                              // 또는 _SEI(); 가능
     
     
    while(1) 
    {     
      
      key=Key();                         // 키 함수의 리턴값을 키에 받음
      if(key!=0xff)                      // 그런데 그 키의 값이 0xff가 아니라면?
      {
      instruction_out(0xc0+i);           // LCD에 출력 위치
      delay(1000); 
      char_out(key);                      // 리턴 키 값을 출력하는 함수.
      m_delay(500);
      i++;
      
      sec_test(key);                       //비밀번호 확인 함수
      }
    }
}   
    
    #pragma vector=INT4_vect              // iom128.h를 참고한다. 
__interrupt void INT4_interrrupt(void)   
{
    __disable_interrupt();   
     
  dc_motor(1,0x3ff);
      m_delay(1000);
      OCR1A=0x000;
      
      instruction_out(0x01);                // LCD를 지운다.
      string_out(0x80, "Password");        // LCD의 첫번째 라인
      
      arr_cnt=1;
      right=1;
      g=0; 
      s=0;
      i=0;
            
    __enable_interrupt();
}
