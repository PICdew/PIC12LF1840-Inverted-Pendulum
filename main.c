#define _XTAL_FREQ 16000000 //因為被 __delay_us() 使用,所以必須要在所有的#include <xc.h>之前
#include <xc.h> // include standard header file
#include <stdio.h>
#include "ADC.h"
#include "PID.h"
#include "PWM.h"
#include "UART.h"
#include "SystemInitial.h"
#define BAUDRATE 19200

// set Config bits
#pragma config FOSC=INTOSC, PLLEN=OFF, MCLRE=ON, WDTE=OFF
#pragma config LVP=OFF, CLKOUTEN=OFF,

#define TAKE_LOG2_T1_ISR_FREQ 10	//log2(1024)=10
#define T1_ISR_FREQ (1<<TAKE_LOG2_T1_ISR_FREQ)
#define TIMER1_INTERVAL ((_XTAL_FREQ/4)/T1_ISR_FREQ)
#define TH1_NUM (65535-TIMER1_INTERVAL)>>8
#define TL1_NUM (65535-TIMER1_INTERVAL)&0x00ff
const long int LIMIT = 2000000000;	//for difference and integral
//#define PWM_RESOLUTION_BIT 10

//==== use at main & ISR ====
volatile unsigned int Global_AnalogValue=0;
volatile long int Global_PWM_Value=0;
bit TxFlag=0;
//=======================

const char DecToHex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void main(void)
{   
  OSC_Initial();
  UART_Initial();
  IO_Initial();
  PWM_Initial();

  GIE=1;		//全域中斷(1=Enable,0=Disable)
  PEIE=1;		//周邊中斷(Timer1算周邊)

  //***Timer 1***//
  T1CON=0x00;	//	Fosc/4, No Perscale,
  TMR1H=TH1_NUM;
  TMR1L=TL1_NUM;
  TMR1IE=1;	//Timer1中斷啟用
  TMR1ON=1;	//Timer1啟動
  //***Timer 1***//

  while(1)
  {
    if(TxFlag)
    {
	      //用區域變數先存下Global_AnalogValue
	      //	1. 避免Tx過程中發生中斷使Global_AnalogValue同時被R\W
	      //	2. 使這一段不會成為critical section
	      volatile unsigned int Ruf_AnalogValue = Global_AnalogValue;
        volatile unsigned int Ruf_PWM_Value = (Global_PWM_Value+1023)/2; //scale to 0~1023
        
        TxFlag=0;

        UART_Tx('$');UART_Tx('A');UART_Tx('D');UART_Tx('C');
        
        //CH1(Analog Value)
        UART_Tx(',');
        UART_Tx(DecToHex[Ruf_AnalogValue>>8]);
        UART_Tx(DecToHex[(Ruf_AnalogValue>>4)&0x0F]);
        UART_Tx(DecToHex[Ruf_AnalogValue&0x0F]);
        
        //CH2(PWM Value)
        UART_Tx(',');
        UART_Tx(DecToHex[Ruf_PWM_Value>>8]);
        UART_Tx(DecToHex[(Ruf_PWM_Value>>4)&0x0F]);
        UART_Tx(DecToHex[Ruf_PWM_Value&0x0F]);

        //CH3
        UART_Tx(',');UART_Tx('0');UART_Tx('0');UART_Tx('0');

        UART_Tx('\r');UART_Tx('\n');
    }
  }
}

void interrupt ISR(void)
{
	static unsigned char Times=0;
  static int Position=511;
  static int KP=90,KI=5,KD=2,K_Num=0;
	
	if(PIR1bits.TMR1IF)
	{//Timer1的中斷函式，閉迴路控制用
		
		PIR1bits.TMR1IF=0;//清除Over Follow Flag
		TMR1ON=0;			//停止Timer1
		TMR1H=TH1_NUM;//Load 高8位
		TMR1L=TL1_NUM;//Load 低8位
		TMR1ON=1;			//啟動Timer1
		
		Global_AnalogValue = Read_ADC_Value();	//取樣與轉換
    	long int Err=((signed int)Global_AnalogValue-Position);//若不強制轉型，無法得出負值，這個bug花了10hrs
		long int Output = PID_Control(Err,KP,KI,KD,LIMIT,Position,Global_AnalogValue,TAKE_LOG2_T1_ISR_FREQ);
		Global_PWM_Value = PWM_Set(Output);
		
		Times++;
	}
	
	if(Times>20)
	{
		Times=0;
		TxFlag=1;
	}
	
	if(PIR1bits.RCIF)
	{//Rx的中斷函式
    unsigned char RxBuf=0;
    PIR1bits.RCIF=0;//清除接收中斷旗標
    RxBuf=RCREG;	//將具揮發性的Rx接收暫存器保留下來
    if(RxBuf>96 && RxBuf<123)RxBuf-=32; //小寫變大寫

    switch(RxBuf)
    {
      case'L':Position=318;break;
      case'M':Position=511;break;
      case'R':Position=690;break;
      case'+':Position++,printf("\nPosition=%d\n",Position);break;
      case'-':Position--,printf("\nPosition=%d\n",Position);break;

      case'P':KP=K_Num;K_Num=0;printf("\n KP=%d \n",KP);break;
      case'I':KI=K_Num;K_Num=0;printf("\n KI=%d \n",KI);break;
      case'D':KD=K_Num;K_Num=0;printf("\n KD=%d \n",KD);break;

      default:
        if(RxBuf>47 && RxBuf<58)//0~9
        {
            RxBuf-=48; //ASCII to Dec
            K_Num = K_Num*10;
            K_Num = K_Num + RxBuf;
        }
        break;
    }
	}
}