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
	//³]©w¦Upin¸}¬°
	//Ãþ¤ñ\¼Æ¦ì			I\O
	ANSELAbits.ANSA0=0,	TRISAbits.TRISA0=0;	//LED(Motor +)
	ANSELAbits.ANSA1=1,	TRISAbits.TRISA1=1;	//ADC in
	ANSELAbits.ANSA2=0,	TRISAbits.TRISA2=0;	//LED(Motor -)
  /*RA3 µLÃþ¤ñ¥\¯à*/	TRISAbits.TRISA3=0; //Output Only
	ANSELAbits.ANSA4=0,	TRISAbits.TRISA4=0;	//Tx
	/*RA5 µLÃþ¤ñ¥\¯à*/	TRISAbits.TRISA5=1;	//Rx
}

void UART_Initial(void)
{
	TXCKSEL=1;				//³]©w TX pin from RA0 to RA4
	RXDTSEL=1;				//³]©w RX pin from RA1 to RA5
	
	//³]©wBaud Rate
	//¨ú((_XTAL_FREQ/(64*BAUDRATE))-1)ªº...
	SPBRGL=12;	//¾ã¼Æ
  SPBRGH=0;	//¤p¼Æ
	
	TXSTAbits.BRGH=0;		//¿ï¾Ü§C³tBR
	TXSTAbits.TXEN = 1; //¨Ï¶Ç°e¥i¥H¶Ç°e
	TXSTAbits.SYNC = 0;	//³]©w¬°«D¦P¨B
	
	PIR1bits.RCIF=0;		//½T«OºX¼Ð²M°£
	PIE1bits.RCIE=1;		//Rx¤¤Â_±Ò¥Î
	RCSTAbits.SPEN=1;		//±Ò¥ÎUART
	RCSTAbits.CREN=1;		//Rx¥i±µ¦¬
}

void PWM_Initial(void)
{
	//¸T¤îPWM¿é¥X
	TRISAbits.TRISA0=1;
	TRISAbits.TRISA2=1;
	
	//³]©wPWM¶g´Á,PWM Period = (PR2+1)*4*TOSC „h
	PR2=0x40;
	
	//³]©wCCP1CON,±NCCP¼Ò²Õ³]¬°PWM¤§¥Î
	CCP1CON=0b00001100;
	
	unsigned char DummyNumber=0x00;
	//³]©wDuty Cycle, ¶ñ¼gCCPR1L(°ª8¦ì)»PCCP1CONbits.CC1P1(§C2¦ì)
	CCPR1L = DummyNumber>>2;
	CCP1CONbits.DC1B = DummyNumber&0x03;
	
	//³]©wTimer2
	//²M°£¤¤Â_ºX¼Ð
	PIR1bits.TMR2IF=0;
	
	//¿é¤J¯ßªi¹w°£ÀW,4^n,n={0,1,2,3}
	T2CONbits.T2CKPS=0x0;
	
	//±Ò°ÊTimer2
	TMR2ON=1;
	
	//¶}©ñPWM¿é¥X
	TRISAbits.TRISA0=0;
	TRISAbits.TRISA2=0;
}