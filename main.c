#include <msp430.h>
#include "UART.h"
#include "Afficheur_modifie.h"

char read[16];
int j=0;
int i=0;
double cal_result;
int pm25;

unsigned char commande[];
unsigned char commande1[];
unsigned char commande2[];

unsigned char size =0;
char *adresse;
unsigned char a[];
unsigned int length = 0;

unsigned char message[];
unsigned int len = 24;

int main(void)
{
    commande[0]=0x81;
    commande1[0]=0x11;
    commande2[0]=0x6e;


    Init_uart();
    Aff_Init(BIT1);
    Aff_Luminosite(200,BIT1);
      while(1){

          //to_hex(commande,sizeof(commande),message);
          send_string(commande);
          send_string(commande1);
          send_string(commande2);

          for (j=0; j<5000; j++);
          __enable_interrupt();       // 打开全局中断

          pm25 = getpm25();
          Aff_valeur(convert_Hex_Dec(pm25),BIT1);

      }
}

int getpm25(void){
    return (int)read[11]*16*16+(int) read[12];
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    if (IFG2 & UCA0RXIFG){

       if(i<16){
            read[i]=UCA0RXBUF;
            i++;
            if(read[0]!=0x81)
                i=0;
        }
       else{
               IE2&=~UCA0RXIE;

               int calculatedChecksum = 0;
               for (j = 0; j < 16;j++) {
                   calculatedChecksum += read[j];
               }
               if((calculatedChecksum % 0x100) != 0){
                   P1OUT &=~BIT0;
               }
               else
                   P1OUT |=BIT0;

               IE2|=UCA0RXIE;
               i=0;
       }
    }
}


//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCI0RX_ISR(void)
//{
//  _disable_interrupts();
//  IFG2&=~UCA0RXIFG;
//
//  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
//  for(j=0 ; j<16 ; j++)
//  {
//      data[i] = UCA0RXBUF;
//  }
//
////  read(data);
//
//  UCA0TXBUF = data[0];                    // TX -> RXed character
//
////  for (i=0; i<50000; i++);
//
//  __enable_interrupt();       // 打开全局中断
//
//}

//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCI0RX_ISR(void)
//{
//    data1[size++] = UCA0RXBUF;;                        // Enable USCI_A0 TX interrupt
//    if(size > sizeof data1 - 1)
//    {
//        size = 0;
//        send_string(data1[4]);
//        send_string(data1[5]);
//        send_string(data1[6]);
//        send_string(data1[6]);
//    }
//
//
//
//
//}



//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCI0RX_ISR(void){
//
//    IFG2 &=~UCA0RXIFG; //clear RX interrupt flag
//    a[length]=UCA0RXBUF;
//    length++;
//    if(length>15){
//        UCA0TXBUF = a[4];
//
//        length = 0;
//    }
//}










/*
// Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    //send0_buf(string1);
    //UCA0TXBUF = *string1;
    while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
    unsigned int j = 0;
    unsigned int i = 0;
    while(UCA0RXBUF != '\0'){
        data1[j] = UCA0RXBUF;
        j++;
    }
    data2[0] = to_dec(data1[3])*10+to_dec(data1[4]);
//    send_char(data1[0]);
//        data2[1] = to_dec(dat)



    UCA0TXBUF = data2[0];                    // TX -> RXed character

}
*/



