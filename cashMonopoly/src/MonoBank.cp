#line 1 "C:/DAVIDE/monopolo_board/cashMonopoly/src/MonoBank.c"
#line 1 "c:/users/public/mikroelektronika/mikroc/include/built_in.h"
#line 27 "C:/DAVIDE/monopolo_board/cashMonopoly/src/MonoBank.c"
unsigned int tmpCardImporto=0;
unsigned short i;
char msg[15];
 char txtDebug[7];
unsigned short rec1;
unsigned int inputNumeroValue=0;
const char *spazio = " ";
const char *rigavuota = "                ";

const char *msgs [8] = {"MONO-BANK","Azzero Importi?","Importi azzerati","Muovo:","Saldo:","Inizio:","Tolgo:","Metto:"};
unsigned int keyPadConst [16][2] = {{149,0x30},
 {559,0x31},
 {542,0x32},
 {525,0x33},
 {480,0x34},
 {457,0x35},
 {433,0x36},
 {370,0x37},
 {336,0x38},
 {300,0x39},
 {200,0x2A},
 {80,0x23},
 {508,0x41},
 {410,0x42},
 {264,0x43},
 {24,0x44}};



unsigned int NOcardConst=670;
unsigned int cardConst [6] = {550,450,330,200,150,80};
unsigned short addrcard [7] = {0,4,8,12,16,20,24} ;


unsigned short cardPiu=0;
unsigned short cardMeno=0;



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


 for ( i = 0; i < 6; i++ ) {

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

 for ( i = 0; i < 16; i++ ) {
 min= keyPadConst[i][0] - 8;
 max= keyPadConst[i][0] + 8;

 if (( adcValue>=min) && (adcValue<=max)) {
 vo=keyPadConst[i][1];

 break;
 }

 }
 return vo;
}
void abilitaTMR0() {


 INTCON = 0;
 INTCON.TMR0IE = 1;
 INTCON.TMR0IF = 0;
 INTCON.GIE = 1;
}

void disabilitaTMR0() {
 INTCON.TMR0IE = 0;
 INTCON.TMR0IF = 0;
 INTCON.GIE = 0;
 INTCON.PEIE = 0;
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
#line 165 "C:/DAVIDE/monopolo_board/cashMonopoly/src/MonoBank.c"
OSCCON=0x40;
Lcd_Init(&PORTB);
 Lcd_Cmd(Lcd_CLEAR);
 Lcd_Cmd(Lcd_CURSOR_OFF);
 TRISA.F0=1;
 TRISA.F1=1;
 TRISA.F2=1;
 TRISA.F3=1;
 TRISB.F0=1;

 ANSEL=0x0F;
 ADCON0 = 0x80;
 ADCON1 = 0x80;
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
 if (temp_resB<990) {
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






 } while (adcT<=1000);
 if (i>0) {
  PORTA.F7=1; Delay_ms(100); PORTA.F7=0; 
 }
 return i;
}
void inputNumero(unsigned short col,unsigned short row) {
unsigned short iicc;
unsigned short xcol=0;
unsigned short current=0;
unsigned short digits [10];
unsigned short ciniziale=cardPiu+ cardMeno;
 for ( iicc = 0; iicc < 10; iicc++ ) digits[iicc]=0;
 inputNumeroValue=0;

 Lcd_Out(row, 1, CopyConst2Ram(msg,rigavuota));

 Lcd_Cmd( LCD_UNDERLINE_ON);
 Lcd_Chr(row,col-1,'$');




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
 } ;

 if ((cardPiu==0) && (cardMeno==0)) {
 inputNumeroValue=0;
 current== 0x2A  ;
 Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
 Lcd_Cmd( LCD_CURSOR_OFF);
 return;
 }
 else current=tastiera();


 if (current== 0x23 ) {

 if (xcol>0) {
 xcol=xcol-1;
 Lcd_Chr(row,col+xcol,' ');
 digits[xcol]=' ';
 Lcd_Cmd( LCD_MOVE_CURSOR_LEFT);
 current=0;
 }



 }


 if ((current>0) && (current<=57) && (xcol<9) && (current!= 0x23 ) && (current!= 0x2A ) ){
 digits[xcol]=current;
 Lcd_Chr(row,col+xcol,current);
 xcol=xcol+1;
 current=0;
 }



 } while ((current!= 0x2A  ) && (current!= 0x23  ) && (current<=57));


 if (current>57) {
 if (current==65) inputNumeroValue=200;
 if (current==66) inputNumeroValue=150;
 if (current==67) inputNumeroValue=100;
 if (current==68) inputNumeroValue=50;

 }
 else {

 if (current==  0x2A ){
 if (xcol>=1) {



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
eeprom_write(adres, ((char *)&sayi)[0] );
eeprom_write(adres+1, ((char *)&sayi)[1] );

}
void store_long(unsigned int adres, unsigned long sayi){
eeprom_write(adres, ((char *)&sayi)[0] );
eeprom_write(adres+1, ((char *)&sayi)[1] );
eeprom_write(adres+2, ((char *)&sayi)[2] );
eeprom_write(adres+3, ((char *)&sayi)[3] );
}
unsigned long read_long(unsigned int adres){
unsigned long th;
 ((char *)&th)[0] =eeprom_read(adres);
 ((char *)&th)[1] =eeprom_read(adres+1);
 ((char *)&th)[2] =eeprom_read(adres+2);
 ((char *)&th)[3] =eeprom_read(adres+3);
return th;
}
unsigned int read_int(unsigned int adres){
unsigned int th;
 ((char *)&th)[0] =eeprom_read(adres);
 ((char *)&th)[1] =eeprom_read(adres+1);

return th;
}

void salvaImporto(unsigned short id, unsigned int value) {
unsigned short a1=addrcard[id];
 store_int(a1,value);
}
void resetImporti() {
unsigned short i;
 for ( i = 0; i <= 6; i++ ) {

 salvaImporto(i,0);
 }

}
unsigned int caricaImporto(unsigned short id) {

unsigned int adr=addrcard[id];

 adr=read_int(adr);


return adr;
}
void msgAddZeros() {
 msg[6]=' ';
 msg[7]=' ';




}
unsigned char msgbox( unsigned short row, unsigned short msgIdx) {
unsigned short current=0;
 Lcd_Out(row, 1, CopyConst2Ram(msg,rigavuota));
 Lcd_Out(row, 1, CopyConst2Ram(msg,msgs[msgIdx]));
#line 414 "C:/DAVIDE/monopolo_board/cashMonopoly/src/MonoBank.c"
 do {
 current=tastiera();
 } while ((current!= 0x23 ) && (current!= 0x2A )) ;
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




 Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[5]));
 IntToStr(tmpCardImporto, msg);
 msgAddZeros();
 Lcd_Out(2, 9, msg);
  Delay_ms(1000)  ;
  Delay_ms(1000)  ;
  Delay_ms(1000)  ;

 Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
 Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[idm]));
 IntToStr(inputNumeroValue, msg);
 msgAddZeros();
 Lcd_Out(2, 9, msg);
  Delay_ms(1000)  ;
  Delay_ms(1000)  ;
  Delay_ms(1000)  ;

if (PoM==1) {
 tmpCardImporto=tmpCardImporto+inputNumeroValue;

}
else {
 if (inputNumeroValue> tmpCardImporto) tmpCardImporto=0;
 else tmpCardImporto=tmpCardImporto-inputNumeroValue;
}

 salvaImporto(idCarta, tmpCardImporto);

 for ( i = 7; i >0; i-- ) {
 Lcd_Chr(2,9+i,'-');
  PORTA.F7=1; Delay_ms(55); PORTA.F7=0; 
 Delay_ms(55);
 }

  Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota)); Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[4])); IntToStr(tmpCardImporto, msg); msgAddZeros(); for ( i = 7; i >0; i-- ) { Lcd_Chr(2,9+i,msg[i]); Delay_ms(55); PORTA.F7=1; Delay_ms(55); PORTA.F7=0;  } Delay_ms(1000)  ; Delay_ms(1000)  ; Delay_ms(1000)  ; Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota)); 
#line 491 "C:/DAVIDE/monopolo_board/cashMonopoly/src/MonoBank.c"
}
void main () {


 setupMCU();
 disabilitaTMR0();
  PORTA.F7=1; Delay_ms(100); PORTA.F7=0; 
 Lcd_Cmd(Lcd_CLEAR);
 Lcd_Cmd(Lcd_CURSOR_OFF);



 Lcd_Out(1, 4, CopyConst2Ram(msg,msgs[0]));

 Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));

 inputNumeroValue=0;
 rec1=0;
#line 530 "C:/DAVIDE/monopolo_board/cashMonopoly/src/MonoBank.c"
 do {
 rec1=tastiera();
 if (rec1== 0x30 )

 {
  PORTB.F1=1; 
 rec1=msgbox(2,1);

 if (rec1== 0x2A ) {
 resetImporti();
 Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
 Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[2]));
  Delay_ms(1000) ;
 Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
 }
 Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota));
  PORTB.F1=0; 
 }



 leggiTessere();
 displayTessere();

 if ((cardPiu!=0) || (cardMeno!=0)) {
  PORTB.F1=1; 
 if (cardPiu!=0) tmpCardImporto=caricaImporto(cardPiu) ;
 else tmpCardImporto=caricaImporto(cardMeno);
  Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota)); Lcd_Out(2, 1, CopyConst2Ram(msg,msgs[4])); IntToStr(tmpCardImporto, msg); msgAddZeros(); for ( i = 7; i >0; i-- ) { Lcd_Chr(2,9+i,msg[i]); Delay_ms(55); PORTA.F7=1; Delay_ms(55); PORTA.F7=0;  } Delay_ms(1000)  ; Delay_ms(1000)  ; Delay_ms(1000)  ; Lcd_Out(2, 1, CopyConst2Ram(msg,rigavuota)); 

 inputNumero(3,2);
 leggiTessere();
 displayTessere();
 if ((cardPiu!=0) || (cardMeno!=0)) {
 if (inputNumeroValue>0) {
 IntToStr(inputNumeroValue, msg);
 msgAddZeros();
 Lcd_Out(2, 9, msg);
  Delay_ms(1000) ;
  Delay_ms(1000) ;
 if (cardPiu!=0) movimentaCarta(cardPiu,1);
 displayTessere();
 if (cardMeno!=0) movimentaCarta(cardMeno,0);

 }
 }
  PORTB.F1=0; 
 }


 } while (1);


}
