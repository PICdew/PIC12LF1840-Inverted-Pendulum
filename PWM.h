#include <xc.h> // include standard header file

long int PWM_Set(long int _u)
{
	if(_u>1023)_u=1023;			//設定上限, can't use 1024, that is 11 bit!
	else if(_u<-1023)_u=-1023;	//設定下限
   
	if( _u>0)
	{
		//if(STR1A==0 && STR1B==1)__delay_us(1);	//死區延遲
		STR1A=1;STR1B=0;
	}
	else if( _u<0)
	{
	  //if(STR1A==1 && STR1B==0)__delay_us(1);	//死區延遲
		STR1A=0;STR1B=1;_u=-_u;
	}
	else if(_u==0){STR1A=0;STR1B=0;}

	//10bit PWM 的高8bit在CCPR1L, 低2bit在CCP1CONbits.DC1B
	CCPR1L = _u>>2;	//取高8bit
	CCP1CONbits.DC1B = _u&0x03;	//取低2bit
	
	//若u原本是負的, return時還原回去
	//若u原本不是負, return時原封不動
	return (0==STR1A && 1==STR1B)? -_u:_u;
}
