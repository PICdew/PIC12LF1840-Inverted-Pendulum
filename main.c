#define _XTAL_FREQ 16000000 //�]���Q __delay_us() �ϥ�,�ҥH�����n�b�Ҧ���#include <xc.h>���e
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

  GIE=1;		//���줤�_(1=Enable,0=Disable)
  PEIE=1;		//�P�䤤�_(Timer1��P��)

  //***Timer 1***//
  T1CON=0x00;	//	Fosc/4, No Perscale,
  TMR1H=TH1_NUM;
  TMR1L=TL1_NUM;
  TMR1IE=1;	//Timer1���_�ҥ�
  TMR1ON=1;	//Timer1�Ұ�
  //***Timer 1***//

  while(1)
  {
    if(TxFlag)
    {
	      //�ΰϰ��ܼƥ��s�UGlobal_AnalogValue
	      //	1. �קKTx�L�{���o�ͤ��_��Global_AnalogValue�P�ɳQR\W
	      //	2. �ϳo�@�q���|����critical section
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
	{//Timer1�����_�禡�A���j�������
		
		PIR1bits.TMR1IF=0;//�M��Over Follow Flag
		TMR1ON=0;			//����Timer1
		TMR1H=TH1_NUM;//Load ��8��
		TMR1L=TL1_NUM;//Load �C8��
		TMR1ON=1;			//�Ұ�Timer1
		
		Global_AnalogValue = Read_ADC_Value();	//���˻P�ഫ
    	long int Err=((signed int)Global_AnalogValue-Position);//�Y���j���૬�A�L�k�o�X�t�ȡA�o��bug��F10hrs
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
	{//Rx�����_�禡
    unsigned char RxBuf=0;
    PIR1bits.RCIF=0;//�M���������_�X��
    RxBuf=RCREG;	//�N�㴧�o�ʪ�Rx�����Ȧs���O�d�U��
    if(RxBuf>96 && RxBuf<123)RxBuf-=32; //�p�g�ܤj�g

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