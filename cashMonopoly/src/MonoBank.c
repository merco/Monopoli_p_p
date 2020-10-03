
#include "built_in.h"
#define tastoOK 0x2A
#define tastoCANCEL 0x23
#define BACKLIGHT_ON PORTB.F1=1;
#define BACKLIGHT_OFF PORTB.F1=0;
#define tastoZERO 0x30
#define teletype PORTA.F7=1;  Delay_ms(55); PORTA.F7=0;
#define PIN_LOWBATTERY PORTB.F0
#define delayMaster Delay_ms(1000)
#define mostraSaldo  Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota)); \
        Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[4])); \
        IntToStr(tmpCardImporto, msg); \
        msgAddZeros(); \
        for (  i = 7; i >0; i-- ) { \
         Lcd_Chr(2,9+i,msg[i]);     \
         Delay_ms(55);             \
         teletype \
            }  \
         delayMaster ;\
         delayMaster ;\
         delayMaster ; \
         Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
         
#define buzzer PORTA.F7=1;  Delay_ms(100); PORTA.F7=0;

unsigned int   tmpCardImporto=0;
unsigned short i;
char msg[15];
          char txtDebug[7];
unsigned short rec1;
unsigned int inputNumeroValue=0;
const char *spazio = " ";
const char *rigavuota = "                ";

const char *msgs [8] = {"MONO-BANK","Azzero Importi?","Importi azzerati","Muovo:","Saldo:","Inizio:","Tolgo:","Metto:"};
unsigned int keyPadConst [16][2]  = {{139,0x30},  //0
                                     {556,0x31},
                                     {542,0x32},
                                     {525,0x33},
                                     {476,0x34},
                                     {457,0x35},
                                     {433,0x36},
                                     {365,0x37},
                                     {336,0x38},
                                     {299,0x39},   //9
                                     {190,0x2A},   //*
                                     {78,0x23},    //#
                                     {508,0x41},   //A
                                     {408,0x42},   //B
                                     {263,0x43},   //C
                                     {15,0x44}};    //D



unsigned int   NOcardConst=670;
unsigned int   cardConst [6]  = {550,450,330,200,150,80};
unsigned short addrcard  [7]  = {0,4,8,12,16,20,24} ;


unsigned short cardPiu=0;
unsigned short cardMeno=0;


// copy const to ram string
char * CopyConst2Ram(char * dest, const char * src){
  char * d ;
  d = dest;
  for(;*dest++ = *src++;)
    ;
  return d;
}
unsigned short decodifica_tessera(unsigned int adcValue) {

unsigned short i;
unsigned short vo=0;

     if (adcValue>=NOcardConst) return 0;
     

    for (  i = 0; i < 6; i++ ) {
    
        if (adcValue>=cardConst[i]) {
           vo=i+1;
           break;
        }
    
    }

return vo;
}
unsigned short decodifica_tasto(unsigned int adcValue) {
unsigned short i;
unsigned int min,max;
unsigned short vo=0;

 for (  i = 0; i < 16; i++ ) {
     min= keyPadConst[i][0] - 8;
     max= keyPadConst[i][0] + 8;
     
     if (( adcValue>=min) && (adcValue<=max)) {
        vo=keyPadConst[i][1];
       // buzzer
        break;
     }
 
 }
 return vo;
}
void abilitaTMR0() {

 // Interrupt Registers
  INTCON = 0;           // clear the interrpt control register
  INTCON.TMR0IE = 1;        // bit5 TMR0 Overflow Interrupt Enable bit...1 = Enables the TMR0 interrupt
  INTCON.TMR0IF = 0;        // bit2 clear timer 0 interrupt flag
  INTCON.GIE = 1;           // bit7 global interrupt enable
}

void disabilitaTMR0() {
  INTCON.TMR0IE = 0;        // bit5 TMR0 Overflow Interrupt Enable bit...1 = Enables the TMR0 interrupt
  INTCON.TMR0IF = 0;        // bit2 clear timer 0 interrupt flag
  INTCON.GIE = 0;           // bit7 global interrupt enable
    INTCON.PEIE = 0;           // bit7 global interrupt enable
}

void Vdelay_us(unsigned time_us)
{
  unsigned ncyc;

   ncyc = Clock_MHz()>>2;
  ncyc *= time_us>>4;
  ncyc=ncyc-3;
  while (ncyc--)
  {
    asm nop;

  }

}



void setupMCU () {

TRISA=0;
INTCON=0;
TRISB=0;
CMCON=7;
ANSEL=0;
PORTB=0;
PORTA=0;

/*
IRCF<2:0>: Internal RC Oscillator Frequency Select bits
000 = 31.25 kHz
001 = 125 kHz
010 = 250 kHz
011 = 500 kHz
100 = 1 MHz
101 = 2 MHz
110 = 4 MHz
111 = 8 MHz
*/

OSCCON=0x40;
Lcd_Init(&PORTB);
 Lcd_Cmd(Lcd_CLEAR);       // Clear display
  Lcd_Cmd(Lcd_CURSOR_OFF);  // Turn cursor off
 TRISA.F0=1;
  TRISA.F1=1;
   TRISA.F2=1;
      TRISA.F3=1;
   TRISB.F0=1;
 //ANSEL=7;
 ANSEL=0x0F;
    ADCON0 = 0x80;  // Configure analog inputs and Vref
   ADCON1 = 0x80;  // Configure analog inputs and Vref
   INTCON=0;
   PIE1=0;
   PIE2=0;
   CCP1CON=0;

}

void Write_String(char *texte)
 {
 short int i1,j1,k;
 j1=strlen(texte);
 i1=0;
 do
 {
 k=*(texte+i1);
 //Soft_UART_Write(k);
// send_byte_1200(k);
 i1++;
 }while (i1<j1);
}

void leggiTessere() {

   cardPiu=decodifica_tessera(Adc_Read(1));
   cardMeno=decodifica_tessera(Adc_Read(2));

}
void displayTessere() {
unsigned int temp_resB;
    Lcd_Chr(1,1,' ');
    Lcd_Chr(1,2,'.');
   Lcd_Chr(1,15,'.');
      Lcd_Chr(1,16,' ');
     if (cardPiu>0) Lcd_Chr(1,2,cardPiu+48) ;
   if (cardMeno>0) Lcd_Chr(1,15,cardMeno+48);
   
   
   temp_resB=Adc_Read(3) ;
   if (temp_resB<990)   {
        Lcd_Chr(1,1,'B');
         Lcd_Chr(1,16,'B');
   }
}


unsigned short tastiera() {
unsigned int adcT;
unsigned short i=0;
    do {
    adcT = Adc_Read(0);
    i=decodifica_tasto(adcT);
    Delay_ms(5);
    adcT = Adc_Read(0);
    

    //   IntToStr(adcT, txtDebug);  // txt is " -4220" (one blank here)
     //Lcd_Out(2, 1, txtDebug);
          
          
    } while (adcT<=1000);
    if (i>0) {
       buzzer
     }
    return i;
}
void inputNumero(unsigned short col,unsigned short row) {
unsigned short iicc;
unsigned short xcol=0;
unsigned short current=0;
unsigned short digits [10];
unsigned short ciniziale=cardPiu+ cardMeno;
        for (  iicc = 0; iicc < 10; iicc++ ) digits[iicc]=0;
    inputNumeroValue=0;

    Lcd_Out(row, 1, CopyConst2Ram(msg,rigavuota));
  
    Lcd_Cmd( LCD_UNDERLINE_ON);
    Lcd_Chr(row,col-1,'$');
   // Lcd_Cmd( LCD_MOVE_CURSOR_RIGHT);



 do {
 
  cardPiu=decodifica_tessera(Adc_Read(1));
   cardMeno=decodifica_tessera(Adc_Read(2));
   
   if ( ciniziale!=(cardPiu+cardMeno)) {
         Lcd_Chr(1,1,' ');
         Lcd_Chr(1,2,'.');
         Lcd_Chr(1,15,'.');
         Lcd_Chr(1,16,' ');
         if (cardPiu>0) Lcd_Chr(1,2,cardPiu+48) ;
         if (cardMeno>0) Lcd_Chr(1,15,cardMeno+48);
         ciniziale=cardPiu+ cardMeno;
         Lcd_Cmd( LCD_CURSOR_OFF);
   }  ;

         if ((cardPiu==0) &&   (cardMeno==0)) {
            inputNumeroValue=0;
            current==tastoOK  ;
            Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
            Lcd_Cmd( LCD_CURSOR_OFF);
            return;
         }
        else current=tastiera();
        

            if (current==tastoCANCEL) {

                     if (xcol>0) {
                        xcol=xcol-1;
                        Lcd_Chr(row,col+xcol,' ');
                        digits[xcol]=' ';
                         Lcd_Cmd( LCD_MOVE_CURSOR_LEFT);
                        current=0;
                        }

                     //if (xcol!=0) current=0;

             }
             

           if ((current>0) && (current<=57) && (xcol<9) && (current!=tastoCANCEL) && (current!=tastoOK) ){
            digits[xcol]=current;
            Lcd_Chr(row,col+xcol,current);
            xcol=xcol+1;
            current=0;
            }



    } while ((current!=tastoOK ) && (current!=tastoCANCEL ) && (current<=57));


        if (current>57) {
            if (current==65) inputNumeroValue=200;
            if (current==66) inputNumeroValue=150;
            if (current==67) inputNumeroValue=100;
            if (current==68) inputNumeroValue=50;

        }
        else  {
        
             if (current== tastoOK){
                 if (xcol>=1) {
                      //digits[xcol]=0;
                      //digits[xcol-1]=0;
                      //digits[xcol-2]=0;
                       inputNumeroValue=atoi(digits);

                 }
                 else {
                      inputNumeroValue=0;
                 }

                 
                 }
            else inputNumeroValue=0;

        
        }



      Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
    Lcd_Cmd( LCD_CURSOR_OFF);

}
void store_int(unsigned int adres, unsigned int sayi){
eeprom_write(adres,lo(sayi));
eeprom_write(adres+1,hi(sayi));

}
void store_long(unsigned int adres, unsigned long sayi){
eeprom_write(adres,lo(sayi));
eeprom_write(adres+1,hi(sayi));
eeprom_write(adres+2,higher(sayi));
eeprom_write(adres+3,highest(sayi));
}
unsigned long read_long(unsigned int adres){
unsigned long th;
lo(th)=eeprom_read(adres);
hi(th)=eeprom_read(adres+1);
higher(th)=eeprom_read(adres+2);
highest(th)=eeprom_read(adres+3);
return th;
}
unsigned int read_int(unsigned int adres){
unsigned int th;
lo(th)=eeprom_read(adres);
hi(th)=eeprom_read(adres+1);

return th;
}

void salvaImporto(unsigned short id, unsigned int value) {
unsigned short a1=addrcard[id];
     store_int(a1,value);
}
void resetImporti() {
unsigned short i;
     for (  i = 0; i <= 6; i++ ) {

            salvaImporto(i,0);
     }

}
unsigned int caricaImporto(unsigned short id) {

unsigned int adr=addrcard[id];

        adr=read_int(adr);
       // if (adr==65535) adr=0;

return adr;
}
void msgAddZeros() {
  msg[6]=' ';
 msg[7]=' ';
 //msg[6]='0';
 //msg[7]='0';


}
unsigned char msgbox( unsigned short row, unsigned short msgIdx) {
unsigned short current=0;
        Lcd_Out(row, 1, CopyConst2Ram(msg,rigavuota));
        Lcd_Out(row, 1, CopyConst2Ram(msg,msgs[msgIdx]));
        /*
        do  {
            asm {NOP};
            current=tastiera();
        }   while ((current==tastoCANCEL));
         */

        do  {
            current=tastiera();
        }   while ((current!=tastoCANCEL) && (current!=tastoOK)) ;
return current;

}
void movimentaCarta(unsigned short idCarta, unsigned short PoM) {


unsigned short idm;
  Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
  
if (PoM==1) {
   Lcd_Chr(1,1,'+') ;
   idm=7;
   }
  else {
   idm=6;
    Lcd_Chr(1,16,'-');
   };
  tmpCardImporto=caricaImporto(idCarta);
  //valore iniziale
  
  //mostraSaldo

  Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[5]));
  IntToStr(tmpCardImporto, msg);
  msgAddZeros();
  Lcd_Out(2, 9, msg);
  delayMaster ;
  delayMaster ;
  delayMaster ;
  // movimento
  Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
  Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[idm]));
  IntToStr(inputNumeroValue, msg);
  msgAddZeros();
  Lcd_Out(2, 9, msg);
  delayMaster ;
  delayMaster ;
  delayMaster ;

if (PoM==1) {
    tmpCardImporto=tmpCardImporto+inputNumeroValue;

}
else {
      if (inputNumeroValue> tmpCardImporto) tmpCardImporto=0;
      else  tmpCardImporto=tmpCardImporto-inputNumeroValue;
}

    salvaImporto(idCarta, tmpCardImporto);

     for (  i = 7; i >0; i-- ) {
         Lcd_Chr(2,9+i,'-');
         teletype
         Delay_ms(55);
     }

     mostraSaldo
  //mostro saldo
  /*
  Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
  Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[4]));
  IntToStr(tmpCardImporto, msg);
  msgAddZeros();
  for (  i = 7; i >0; i-- ) {
         Lcd_Chr(2,9+i,msg[i]);
         Delay_ms(150);
   }
   delayMaster ;
   delayMaster ;
   delayMaster ;

  Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
*/
//mostraSaldo
}
void main () {


   setupMCU();
   disabilitaTMR0();
   buzzer
   Lcd_Cmd(Lcd_CLEAR);       // Clear display
   Lcd_Cmd(Lcd_CURSOR_OFF);  // Turn cursor off
   


   Lcd_Out(1, 4, CopyConst2Ram(msg,msgs[0]));

    Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));

    inputNumeroValue=0;
    rec1=0;
    
    
    
    
   /* debug per controllo tasti

    do {
       unsigned int temp_res;
       rec1=tastiera();

    
    } while (1);
    
     */
    
    
    
    

    
    
  do {
    rec1=tastiera();
    if (rec1==tastoZERO)
    
      {
        BACKLIGHT_ON
        rec1=msgbox(2,1);

        if (rec1==tastoOK)  {
           resetImporti();
           Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
           Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[2]));
           delayMaster;
           Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
        }
       Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
       BACKLIGHT_OFF
    }

 //
  
   leggiTessere();
   displayTessere();
   
   if ((cardPiu!=0) || (cardMeno!=0))  {
      BACKLIGHT_ON
      if (cardPiu!=0)   tmpCardImporto=caricaImporto(cardPiu) ;
      else   tmpCardImporto=caricaImporto(cardMeno);
       mostraSaldo
   
      inputNumero(3,2);
      leggiTessere();
      displayTessere();
      if ((cardPiu!=0) || (cardMeno!=0))  {
      if (inputNumeroValue>0) {
            IntToStr(inputNumeroValue, msg);
           msgAddZeros();
           Lcd_Out(2, 9, msg);
              delayMaster;
                 delayMaster;
           if (cardPiu!=0) movimentaCarta(cardPiu,1);
           displayTessere();
           if (cardMeno!=0) movimentaCarta(cardMeno,0);

      }
      }
     BACKLIGHT_OFF
   }
   //Delay_ms(50);

    } while (1);                    // endless loop (as this condition is always satisfied)


}