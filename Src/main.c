/*センサやLEDの番号(0~15)は、基板をPICのついてる面から見て、
  左側から、0番目,1番目,2番目... とする
 */

#include "main.h"
#include <xc.h>
#include <stdbool.h>
#include <math.h>

#define WHITE_LINE_UP   500.0
#define WHITE_LINE_DOWN 500.0

void init(void);
void adcon_set(int num);
void led_off(void);
void led_blink(int num, bool state);

const double ADJUST_MAG[16] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
			       1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
const double ADJUST_DIF[16] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			       0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const uint8_t adcon[8] = {(0<<2)|1,(1<<2)|1,(2<<2)|1,(3<<2)|1,
			  (8<<2)|1,(9<<2)|1,(11<<2)|1,(13<<2)|1};


void main(void) {
  
  double value[16] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  uint8_t condition[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  bool enable1_flag = false;
  bool enable2_flag = false;
  uint16_t send_value = 0;
  int i=0,j=0;

  int test_count=0;
    
  init();
    
  while(true){

    /* if(test_count%350 == 0) i++; */
    /* if(i>=16) i=0; */
    /* for(j=0;j<16;j++){ */
    /*   if(j==i){ */
    /* 	led_blink(j,true); */
    /*   }else{ */
    /* 	led_blink(j,false); */
    /*   } */
    /* } */
    /* __delay_us(20); */
    /* test_count++; */
    
    for(i=0;i<16;i++){
      if(i==0 || i==12){
    	enable2_flag = true;
      }
      if(i==4){
    	enable1_flag = true;
      }
      if(enable1_flag){
    	RA5 = 0;
    	__delay_us(50);
    	RA4 = 1;
    	__delay_us(50);
    	enable1_flag = false;
      }
      if(enable2_flag){
    	RA4 = 0;
    	__delay_us(50);
    	RA5 = 1;
    	__delay_us(50);
    	enable2_flag = false;
      }
      adcon_set(i);
      //---- __delay_us(20); より処理時間の長いタスク(start) ----
      if(condition[i] == 1){
    	led_blink(i,true);
      }else{
    	led_blink(i,false);
      }
      
      send_value &= condition[0];
      send_value &= condition[1] << 1;
      send_value &= condition[2] << 2;
      send_value &= condition[3] << 3;
      send_value &= condition[4] << 4;
      send_value &= condition[5] << 5;
      send_value &= condition[6] << 6;
      send_value &= condition[7] << 7;
      send_value &= condition[8] << 8;
      send_value &= condition[9] << 9;
      send_value &= condition[10] << 10;
      send_value &= condition[11] << 11;
      send_value &= condition[12] << 12;
      send_value &= condition[13] << 13;
      send_value &= condition[14] << 14;
      send_value &= condition[15] << 15;

      send_value |= condition[0];
      send_value |= condition[1] << 1;
      send_value |= condition[2] << 2;
      send_value |= condition[3] << 3;
      send_value |= condition[4] << 4;
      send_value |= condition[5] << 5;
      send_value |= condition[6] << 6;
      send_value |= condition[7] << 7;
      send_value |= condition[8] << 8;
      send_value |= condition[9] << 9;
      send_value |= condition[10] << 10;
      send_value |= condition[11] << 11;
      send_value |= condition[12] << 12;
      send_value |= condition[13] << 13;
      send_value |= condition[14] << 14;
      send_value |= condition[15] << 15;
      
      //---- __delay_us(20); より処理時間の長いタスク(end) ----
      ADCON0bits.GO = 1;
      while(ADCON0bits.GO);
      value[i] = (double)(ADRESL + ADRESH*256);
      value[i] = value[i]*ADJUST_MAG[i] + ADJUST_DIF[i];

      if(value[i] < 0){
    	value[i] = 0;
      }
      if(value[i] > 1023){
    	value[i] = 1023;
      }
      
      if(value[i]>=WHITE_LINE_UP){
    	condition[i] = 1;
      }else if(value[i]<WHITE_LINE_DOWN){
    	condition[i] = 0;
      }

      //__delay_us(10);
      CLRWDT();
    }

    
    /*******デバッグ用********/
    snd_data[i*2] = (uint8_t)(value[i]);
    snd_data[i*2+1] = (uint8_t)((uint16_t)(value[i]) >> 8);
 
    /*******競技用************/
    /* snd_data[1] = (uint8_t)send_value; */
    /* snd_data[0] = (uint16_t)send_value >> 8; */

    
    send_value = 0;
          
  }
}

void init(void){
  uint8_t addr = 0x38;
  // Set oscilation 
  OSCCON = 0xF0; //PLL　Enable

  //=============================================
  ANSELA = 0b00001111;
  ANSELB = 0b00111100;
  TRISA  = 0b00001111;
  TRISB  = 0b00111111;
  TRISC  = 0b11100000;
  ADCON0 = 0x00;
  ADCON1 = 0xE0;
  //=============================================
  
  // Set watch dog
  WDTCON = 0x13;

  /* addr |= (PORTBbits.RB1 << 0); */
  /* addr |= (PORTBbits.RB0 << 1); */
  /* addr |= (PORTCbits.RC7 << 2); */
  /* addr |= (PORTCbits.RC6 << 3); */

  I2C_init(addr);//アドレス
}

void adcon_set(int num){
  switch(num){
  case 0:
  case 7:
    ADCON0 = adcon[4];
    break;
  case 1:
  case 6:
    ADCON0 = adcon[5];
    break;
  case 2:
  case 4:
    ADCON0 = adcon[7];
    break;
  case 3:
  case 5:
    ADCON0 = adcon[6];
    break;
  case 8:
  case 12:
    ADCON0 = adcon[0];
    break;
  case 9:
  case 13:
    ADCON0 = adcon[1];
    break;
  case 10:
  case 14:
    ADCON0 = adcon[3];
    break;
  case 11:
  case 15:
    ADCON0 = adcon[2];
    break;
  }
}

void led_off(void){
  RA7 = 0;
  RA6 = 0;
  RC0 = 0;
  RC1 = 0;
  RC2 = 0;
  TRISAbits.TRISA7 = 1;
  TRISAbits.TRISA6 = 1;
  TRISCbits.TRISC0 = 1;
  TRISCbits.TRISC1 = 1;
  TRISCbits.TRISC2 = 1;
}

void led_blink(int num, bool state){
  switch(num){
  case 11://0
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 0;
      RC1 = 0;
      TRISCbits.TRISC2 = 0;
      RC2 = 1;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      RC1 = 0;
      TRISCbits.TRISC2 = 1;
      RC2 = 0;
    }
    break;
    
  case 3://1
    if(state){
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC2 = 1;
      TRISAbits.TRISA7 = 0;
      RA7 = 1;
      TRISCbits.TRISC1 = 0;
      RC1 = 0;
    }else{
      TRISAbits.TRISA7 = 1;
      RA7 = 0;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      RC1 = 0;
      TRISCbits.TRISC2 = 1;
    }
    break;

  case 2://2
    if(state){
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC2 = 1;
      TRISAbits.TRISA7 = 0;
      RA7 = 0;
      TRISCbits.TRISC1 = 0;
      RC1 = 1;
    }else{
      TRISAbits.TRISA7 = 1;
      RA7 = 0;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      RC1 = 0;
      TRISCbits.TRISC2 = 1;
    }
    break;
    
  case 8://3
    if(state){
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      TRISAbits.TRISA7 = 0;
      RA7 = 0;
      TRISAbits.TRISA6 = 0;
      RA6 = 1;
    }else{
      TRISAbits.TRISA7 = 1;
      RA7 = 0;
      TRISAbits.TRISA6 = 1;
      RA6 = 0;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
    }
    break;

  case 4://4
    if(state){
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      TRISAbits.TRISA7 = 0;
      RA7 = 0;
      TRISCbits.TRISC0 = 0;
      RC0 = 1;
    }else{
      TRISAbits.TRISA7 = 1;
      RA7 = 0;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      RC0 = 0;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
    }
    break;
    
  case 7://5
    if(state){
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      TRISAbits.TRISA7 = 0;
      RA7 = 1;
      TRISCbits.TRISC0 = 0;
      RC0 = 0;
    }else{
      TRISAbits.TRISA7 = 1;
      RA7 = 0;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      RC0 = 0;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
    }
    break;

  case 0://6
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      TRISAbits.TRISA6 = 0;
      RA6 = 0;
      TRISCbits.TRISC2 = 0;
      RC2 = 1;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      RA6 = 0;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      RC2 = 0;
    }
    break;
    
  case 1://7
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      TRISAbits.TRISA6 = 0;
      RA6 = 1;
      TRISCbits.TRISC2 = 0;
      RC2 = 0;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      RA6 = 0;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      RC2 = 0;
    }
    break;

  case 15://8
    if(state){
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      TRISAbits.TRISA7 = 0;
      RA7 = 1;
      TRISAbits.TRISA6 = 0;
      RA6 = 0;
    }else{
      TRISAbits.TRISA7 = 1;
      RA7 = 0;
      TRISAbits.TRISA6 = 1;
      RA6 = 0;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
    }
    break;
    
  case 14://9
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      TRISAbits.TRISA6 = 0;
      RA6 = 1;
      TRISCbits.TRISC0 = 0;
      RC0 = 0;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      RA6 = 0;
      TRISCbits.TRISC0 = 1;
      RC0 = 0;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
    }
    break;

  case 13://10
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC2 = 1;
      TRISCbits.TRISC0 = 0;
      RC0 = 1;
      TRISCbits.TRISC1 = 0;
      RC1 = 0;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      RC0 = 0;
      TRISCbits.TRISC1 = 1;
      RC1 = 0;
      TRISCbits.TRISC2 = 1;
    }
    break;
    
  case 5://11
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC0 = 0;
      RC0 = 0;
      TRISCbits.TRISC2 = 0;
      RC2 = 1;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      RC0 = 0;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      RC2 = 0;
    }
    break;

  case 12://12
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 0;
      RC1 = 1;
      TRISCbits.TRISC2 = 0;
      RC2 = 0;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      TRISCbits.TRISC1 = 1;
      RC1 = 0;
      TRISCbits.TRISC2 = 1;
      RC2 = 0;
    }
    break;
    
  case 6://13
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC0 = 0;
      RC0 = 1;
      TRISCbits.TRISC2 = 0;
      RC2 = 0;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      RC0 = 0;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      RC2 = 0;
    }
    break;

  case 9://14
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
      TRISAbits.TRISA6 = 0;
      RA6 = 0;
      TRISCbits.TRISC0 = 0;
      RC0 = 1;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      RA6 = 0;
      TRISCbits.TRISC0 = 1;
      RC0 = 0;
      TRISCbits.TRISC1 = 1;
      TRISCbits.TRISC2 = 1;
    }
    break;
    
  case 10://15
    if(state){
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC2 = 1;
      TRISCbits.TRISC0 = 0;
      RC0 = 0;
      TRISCbits.TRISC1 = 0;
      RC1 = 1;
    }else{
      TRISAbits.TRISA7 = 1;
      TRISAbits.TRISA6 = 1;
      TRISCbits.TRISC0 = 1;
      RC0 = 0;
      TRISCbits.TRISC1 = 1;
      RC1 = 0;
      TRISCbits.TRISC2 = 1;
    }
    break;
  }
  
}

void interrupt  HAND(void){
  Slave_Interrupt();
}
