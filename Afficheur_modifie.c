
#include <Afficheur_modifie.h>
#include <msp430.h>

/********************************/
/*        Pin connection        */
/********************************/
/* Connection for MSP430G2452   */
/*    MSP430    |    Display	*/
/*     P1.4     |      SSN      */
/*     P1.6     |      SDI       */
/*     P1.5     |      SCK      */
/*..............................*/
/* Connection for MSP430G2553   */
/*    MSP430    |    Display    */
/*     P1.4     |      SSN      */
/*     P1.7     |      SDI       */
/*     P1.5     |      SCK      */
/*..............................*/


/*...................................................................................*/
/* void Init_USI_SPI()                                                               */
/*...................................................................................*/
/* Description : Initialisation of the SPI bus                                       */
/* Inputs :                                                                          */
/*      - unsigned int bit : the PIN to use                                                                       */
/* Output :                                                                          */
/*      - void                                                                       */
/*...................................................................................*/
void Init_USI_SPI(unsigned int bit)
{
    UCB0CTL1 = UCSWRST;

    P2DIR |= bit; // pin bit CS output
    P2OUT |= bit; // pin bit CS high
    P1SEL |= BIT7 | BIT5;  // Enable SIMO + SCLK
    P1SEL2 |= BIT7 | BIT5; // Enable SIMO + SCLK
    UCB0CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC; // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2; // SMCLK
    UCB0CTL1 &= ~UCSWRST;
}

/*...................................................................................*/
/* void Send_char_SPI(unsigned char carac)                                           */
/*...................................................................................*/
/* Description : Send a char via SPI                                                 */
/* Inputs :                                                                          */
/*      - unsigned char carac : the char to send
 *      - unsigned int bit : the PIN to send                                         */
/* Output :                                                                          */
/*      - void                                                                       */
/*...................................................................................*/
void Send_char_SPI(unsigned char carac,  unsigned int bit)
{
    P2OUT &= ~bit;  // CS low
    UCB0TXBUF = carac; // Put caracter in transmit buffer
    while (!(IFG2 & UCB0TXIFG)); // USCI_A0 TX buffer ready?
    P2OUT |= bit; // CS high
}




/*...................................................................................*/
/* void Aff_4carac(unsigned char buffer[4])                                       */
/*...................................................................................*/
/* Description : Display four characters                                             */
/* Inputs :                                                                          */
/*      - unsigned char buffer[4] : characters to display
 *      - unsigned int bit : the PIN to send                       */
/* Output :                                                                          */
/*      - void                                                                       */
/*...................................................................................*/
void Aff_4carac(unsigned char buffer[4],unsigned char bit)
{
  Send_char_SPI(buffer[0],bit);
  Send_char_SPI(buffer[1],bit);
  Send_char_SPI(buffer[2],bit);
  Send_char_SPI(buffer[3],bit);
}


/*...................................................................................*/
/* void Aff_Points(unsigned char code)                                           */
/*...................................................................................*/
/* Description : Swicth on points on the display                                     */
/* Inputs :                                                                          */
/*      - unsigned char code :  points to switch on
 *      - unsigned int bit : the PIN to send                                  */
/* Output : void                                                                     */
/*...................................................................................*/
void Aff_Points(unsigned char code, unsigned int bit)
{
  Send_char_SPI(0x77,bit);
  Send_char_SPI(code,bit);
}



/*...................................................................................*/
/* void Aff_Efface()                                     */
/*...................................................................................*/
/* Description : Clear the display                                                   */
/* Inputs :                                                                          */
/*      - unsigned int bit : the PIN to send                                                                       */
/* Output :                                                                          */
/*      - void                                                                       */
/*...................................................................................*/
void Aff_Efface(unsigned int bit)
{
  Send_char_SPI(0x76, bit);
  Aff_Points(0x00, bit);
}

/*...................................................................................*/
/* void Aff_Luminosite(unsigned char lum)                                     */
/*...................................................................................*/
/* Description : Change the luminosity of the display                                */
/* Inputs :                                                                          */
/*      - unsigned char lum : Value of the luminosity between 0 (min) and 255 (max)  */
/* Output : void                                                                     */
/*...................................................................................*/
void Aff_Luminosite(unsigned char lum, unsigned int bit)
{
  Send_char_SPI(0x7A, bit);
  Send_char_SPI(lum, bit);
}


/*...................................................................................*/
/* void Aff_Init()                                                               */
/*...................................................................................*/
/* Description :Initializing of the display                                       */
/* Inputs :                                                                          */
/*      - unsigned int bit : the PIN to send                                                                       */
/* Output :                                                                          */
/*      - void                                                                       */
/*...................................................................................*/
void Aff_Init(unsigned int bit)
{
  Init_USI_SPI(bit);
  Aff_Efface(bit);
  Aff_4carac("8888",bit);
  Aff_Points(0x3F,bit);
  Aff_Luminosite(255,bit);
  __delay_cycles(1000000);
  Aff_Efface(bit);
}


/*...................................................................................*/
/* void Aff_1carac( unsigned char num, unsigned char code)                        */
/*...................................................................................*/
/* Description : Display only one character                                          */
/* Inputs :                                                                          */
/*      - unsigned char num : number of the character (between 1 and 4 )             */
/*      - unsigned char code : segments to swicth on
 *      - unsigned int bit : the PIN to send                             */
/* Output : void                                                                     */
/*...................................................................................*/
void Aff_1carac(unsigned char num, unsigned char code,unsigned int bit)
{
  num %=5;
  num +=0x7A;
  Send_char_SPI(num,bit);
  Send_char_SPI(code,bit);
}

/*...................................................................................*/
/* void convert_digit(int value)                                                      */
/*...................................................................................*/
/* Description : Use for an other fonction                                           */
/* Inputs :                                                                          */
/*      - int value: value of the digit you want to convert in ASCII                       */
/*                                                                                   */
/* Output : int  : value of the ASCII code of the digit                                                                    */
/*...................................................................................*/

int convert_digit(int value)                       // fonction de conversion en hexadecimal
{
	int valTens=0;

	if(value<=9) valTens = value +0x30;          // pour les unités

    else valTens = value +0x37;                  // pour les nombres > 9

	return valTens;
}


/*...................................................................................*/
/* void convert_ASCII(int c, buffer)                                                         */
/*...................................................................................*/
/* Description : Convert the value of an int in ASCII                                          */
/* Inputs :                                                                          */
/*      - int c: value of the number you want to convert  (unsigned)                           */
/*      - buffer : adress of the ASCII buffer to put the results                                                                             */
/* Output : void                							*/
/*...................................................................................*/

void convert_ASCII(int c, unsigned char hexa[4])
{

	int digit1,digit2,digit3,digit4;

	digit4= c & ( 0x000F);                     //  décalage de bit selon le digit
	digit3=(c & ( 0x00F0))>>0x04;
	digit2=(c & ( 0x0F00))>>0x08;
	digit1=(c & ( 0xF000))>>0x0C;
	hexa[3]=convert_digit(digit4);		  // placement dans un tableau de 4 caractères + conversion
	hexa[2]=convert_digit(digit3);
	hexa[1]=convert_digit(digit2);
	hexa[0]=convert_digit(digit1);

}


/*...................................................................................*/
/* void convert_Hex_Dec(int valeur_hex)                                            */
/*...................................................................................*/
/* Description : Convert an hexadecimal to decimal                                   */
/* Inputs :                                                                          */
/*      - char toExtract[]: value of the number in hexadecimal you want to convert   */
/*                                                                                   */
/* Output : char                                                                     */
/*...................................................................................*/
int convert_Hex_Dec(int valeur_hex)
{
	unsigned char dec [4];
        int val_dec;
	dec[0]= valeur_hex / 0x03e8;
 	dec[1]= (valeur_hex % 0x03e8) / 0x64;
	dec[2]= (valeur_hex % 0x64) / 0x0A;
	dec[3]= (valeur_hex % 0x0A);
	val_dec= (dec[0]<<12) + (dec[1]<<8)+ (dec[2]<<4) + dec[3];
	return val_dec;


}


/*...................................................................................*/
/* void affiche_valeur(int av)                                                       */
/*...................................................................................*/
/* Description : Display a value without ASCII conversion                            */
/* Inputs :                                                                          */
/*      - int av: value of the number you want to display
 *      - unsigned int bit : the PIN to send                            */
/*                                                                                   */
/* Output : void                                                                     */
/*...................................................................................*/
void Aff_valeur(int c,unsigned int bit)
{

	int digit1,digit2,digit3,digit4;
	unsigned char v[4];


	digit4= c & ( 0x000F);                     //  décalage de bit selon le digit
	digit3=(c & ( 0x00F0))>>0x04;
	digit2=(c & ( 0x0F00))>>0x08;
	digit1=(c & ( 0xF000))>>0x0C;
	v[3]=(digit4);			           // placement dans un tableau de 4 caractères + conversion
	v[2]=(digit3);
	v[1]=(digit2);
	v[0]=(digit1);
	Aff_4carac(v, bit);                          // affichage du tableau

}
