#include <xc.h> // include standard header file

long int PID_Control(long int Err,int Kp,int Ki,int Kd,long int LIMIT,int Position,unsigned int AnalogValue,unsigned char ScaleRotateBit)
{
	static long int Ki_Sum=0,Kd_Diff=0;
	static long int tempErr=0;
	long int Kp_OutValue,Ki_OutValue,Kd_OutValue; //暫存PID三種計算的結果
	long int K_OutValue=0;//最後加總在這裡
	
  Err=Err<<ScaleRotateBit;	//尺度放大
    
  //*************比例開始***********//
	Kp_OutValue = Kp*Err;
	//*************比例結束***********//
	
	//*************積分開始***********//
	Ki_Sum += Err;
	if	   (Ki_Sum> LIMIT)Ki_Sum= LIMIT;
	else if(Ki_Sum<-LIMIT)Ki_Sum=-LIMIT;
	
	if((Position-5)<=AnalogValue && (Position+5)>=AnalogValue)
		Ki_Sum=0;//積分重置,積分項為0,就不再輸出到Ki_OutValue
	else
		Ki_OutValue = (Ki*Ki_Sum)>>ScaleRotateBit;
	//*************積分結束***********//
	
	//*************微分開始***********//
	Kd_Diff = (Err-tempErr)<<ScaleRotateBit;
	
	tempErr=Err;
	
	if	   (Kd_Diff> LIMIT)Kd_Diff= LIMIT;
	else if(Kd_Diff<-LIMIT)Kd_Diff=-LIMIT;
	
	Kd_OutValue = Kd*Kd_Diff;
	//*************微分結束***********//
	
	K_OutValue = Kp_OutValue + Ki_OutValue + Kd_OutValue;
	K_OutValue = K_OutValue>>ScaleRotateBit;	//尺度還原(注意signed bit遺失問題!)
 
  return K_OutValue;
} 