#include <xc.h> // include standard header file

void UART_Tx(unsigned char mydata_byte)
{
    while(!TXSTAbits.TRMT);    // Polling�ˬd�O�_��
    TXREG = mydata_byte;       // transmit data
}