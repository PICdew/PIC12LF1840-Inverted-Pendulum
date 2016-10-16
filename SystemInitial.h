#include <xc.h> // include standard header file
 
void OSC_Initial(void)
{
	if(1)//16MHz
	{
		// set up oscillator control register
	    OSCCONbits.SPLLEN=0;    // PLL is disabled
	    OSCCONbits.IRCF=0x0F;   //set OSCCON IRCF bits to select OSC frequency=16Mhz
	    OSCCONbits.SCS=0x02;    //set the SCS bits to select internal oscillator block
	}
	if(0)//32MHz
	{
	    OSCCONbits.SPLLEN=1;    // PLL is Enabled
	    OSCCONbits.IRCF=0x0E;   //set OSCCON IRCF bits to select OSC frequency=8Mhz
	    OSCCONbits.SCS=0x00;    //set the SCS bits to select internal oscillator block
	}    
}

void IO_Initial(void)
{	
	//�]�w�Upin�}��
	//����\�Ʀ�			I\O
	ANSELAbits.ANSA0=0,	TRISAbits.TRISA0=0;	//LED(Motor +)
	ANSELAbits.ANSA1=1,	TRISAbits.TRISA1=1;	//ADC in
	ANSELAbits.ANSA2=0,	TRISAbits.TRISA2=0;	//LED(Motor -)
  /*RA3 �L����\��*/	TRISAbits.TRISA3=0; //Output Only
	ANSELAbits.ANSA4=0,	TRISAbits.TRISA4=0;	//Tx
	/*RA5 �L����\��*/	TRISAbits.TRISA5=1;	//Rx
}

void UART_Initial(void)
{
	TXCKSEL=1;				//�]�w TX pin from RA0 to RA4
	RXDTSEL=1;				//�]�w RX pin from RA1 to RA5
	
	//�]�wBaud Rate
	//��((_XTAL_FREQ/(64*BAUDRATE))-1)��...
	SPBRGL=12;	//���
  SPBRGH=0;	//�p��
	
	TXSTAbits.BRGH=0;		//��ܧC�tBR
	TXSTAbits.TXEN = 1; //�϶ǰe�i�H�ǰe
	TXSTAbits.SYNC = 0;	//�]�w���D�P�B
	
	PIR1bits.RCIF=0;		//�T�O�X�вM��
	PIE1bits.RCIE=1;		//Rx���_�ҥ�
	RCSTAbits.SPEN=1;		//�ҥ�UART
	RCSTAbits.CREN=1;		//Rx�i����
}

void PWM_Initial(void)
{
	//�T��PWM��X
	TRISAbits.TRISA0=1;
	TRISAbits.TRISA2=1;
	
	//�]�wPWM�g��,PWM Period = (PR2+1)*4*TOSC �h
	PR2=0x40;
	
	//�]�wCCP1CON,�NCCP�Ҳճ]��PWM����
	CCP1CON=0b00001100;
	
	unsigned char DummyNumber=0x00;
	//�]�wDuty Cycle, ��gCCPR1L(��8��)�PCCP1CONbits.CC1P1(�C2��)
	CCPR1L = DummyNumber>>2;
	CCP1CONbits.DC1B = DummyNumber&0x03;
	
	//�]�wTimer2
	//�M�����_�X��
	PIR1bits.TMR2IF=0;
	
	//��J�ߪi�w���W,4^n,n={0,1,2,3}
	T2CONbits.T2CKPS=0x0;
	
	//�Ұ�Timer2
	TMR2ON=1;
	
	//�}��PWM��X
	TRISAbits.TRISA0=0;
	TRISAbits.TRISA2=0;
}