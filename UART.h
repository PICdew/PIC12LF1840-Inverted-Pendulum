#include <xc.h> // include standard header file

void UART_Tx(unsigned char mydata_byte)
{
    while(!TXSTAbits.TRMT);    // PollingÀË¬d¬O§_ªÅ
    TXREG = mydata_byte;       // transmit data
}