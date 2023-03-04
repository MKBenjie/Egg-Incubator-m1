#include<string.h>
#include<stdio.h>


byte readFlag;
double analogVal = 0;
double Temp;
char temp[6];
int sys_state = 0;
int menu=0;
unsigned long previousTime =0;
unsigned long currentTime;



int main(void)
{
  
  DDRD |= 0b11111000; // setting PIN 3,4,5,6,7 as output
   
  PORTD |= 0b10000000; // Turning on the motor 
  
  //DDRC |= 0b00000000; // setting the UP_Button as input
  PORTC |= (1<<PORTC1); // enable internal pullup resistor on UP
  //DDRC |= (~(1<<DDC0)); // setting the Down_Button as input
  PORTC |= (1<<PORTC2); // enabling internal pull up resistor
  //DDRD |= (~(1<<DDD2)); // setting the select button as input
  PORTD |= 0b00000100;
  
  DDRB |= 0b00111111;
  //_delay_ms(150);
  
  //lcd_initialise();
  //lcdcmdWrite(0x80);
  DDRC |= (1<<DDC3);
  //PORTC |= (1<<PORTC3);
  
  Serial.begin(9600);
  
  ADMUX &= 0b11011111; // clearing ADLAR
  
  ADMUX |= 0b01000000; // setting reference voltage
  
  ADMUX &= 0b11110000; // Clearing MUX
  
  ADMUX |= 0b00000000; // setting A0
  
  ADCSRA |= 0b10000000; // set ADEN to enable ADC
  
  ADCSRA |= 0b00100000; // set ADATE to enable auto_trigerring
  
  // Clear ADTS to set trigger mode to free running
  // This implies as soon as ADC has finished, the next will immediately start
  
  ADCSRB &= 0b11111000;
  
  //setting prescalar to 128 (16000KHz/126 = 125KHz)
  ADCSRA |= 0b00000111;
  
  ADCSRA |= 0b00001000; // set ADIE to enable the ADC interrupt
  
  PCICR |= 0b00000100; // enable pin change interrupt 2
  PCMSK2 |= 0b00000100; // enable pin change interrupt pin 2
  
  sei();
  //PCICR |= 0b00000100; // enable pin change interrupt 1
  //PCMSK2 |= 0b00000100; // enable pin change interrupt pin A0
  //sei();
  
  //readFlag = 0;
  
  ADCSRA |= 0b01000000;
  _delay_ms(10);
  lcd_initialise();
  
  
  for(;;){
    
    //currentTime = millis();

    Temp = map(analogVal, 20, 358, -40.0, 125.0);
  
    //sprintf(temp, "%s", Temp);

      //Temp = Temp * 5;
      //Temp = Temp - 0.5;
      //Temp = Temp * 100;

	String SerialData;

	SerialData = String(Temp,2);
    SerialData.toCharArray(temp, 6);

	//Serial.println(SerialData);

    lcdcmdWrite(0x80); // cursor to begining of line
    lcdWrite("Temp(C): ");
    //lcdcmdWrite(0x88);
    lcdWrite(temp);
    //lcdWrite(temp);
    //lcdWrite(" C ");
    Serial.println(temp);
    
    FVM_1(); 
  
    if(readFlag  == 1){
      if((PIND & 0b00000100)){
        menuUpdate();
        menu++;
        readFlag = 0;
      }
    }
  }
  
}



ISR(ADC_vect) {
 
  //readFlag = 1;
  analogVal = ADCL | (ADCH << 8);
  ADCSRA |= 0b01000000;
}


ISR(PCINT2_vect) {
  readFlag = 1;
}

// finite state machine 
//contolls heater(buld), led lighting and alarm
int FVM_1(){
  switch(sys_state){
      case 0:
      if (Temp > 39){
        sys_state = 1;
      }
      if (Temp > 35 && Temp < 40){
        sys_state = 2;
      }
      if (Temp < 36){
        PORTD |= 0b01000100; //Turn buld/heater on
        PORTD &= 0b11000111; //Turn leds off
        PORTD |= 0b00100000; //Turn blue light on
        PORTC &= 0b11110111;
        sys_state = 0;
      }
      break;
      case 1:
      if (Temp < 36){
        sys_state = 0;
      }
      if (Temp > 35 && Temp < 40){
        sys_state = 2;
      }
      	
      if (Temp > 39){
        
        PORTD &=0b10000111; //Turn bulb/heater off
        PORTD |= 0b00001000; //Turn on red light
        PORTC |= 0b00001000; // Alarm setting
        //tone(600,7500);
        
        sys_state = 1;
      }
      break;
      case 2:
      
      if (Temp < 36){
        sys_state = 0;
      }
      if (Temp > 39){
        sys_state = 1;
      }
      if (Temp > 35 && Temp < 40){
        //PORTD |= 0b01000000; //Leave heater on
        PORTD |= (1<<PORTD6);
        PORTD &= 0b11000111; //Turn leds off
        PORTD |= 0b00010000; //Turn on red led
        PORTC &= 0b11110111;
        sys_state = 2;
      }
      break;
      default:
      //none
      	PORTD &= 0b11000111;
    }
}

void menuUpdate(){
  switch(menu){
    case 0:
    //if(menu == 0){
    	menu = 1;
   // }
    	break;
    case 1: 
        lcdcmdWrite(0x01); //clears screen
        lcdcmdWrite(0x0C); //display on cursor off
        lcdcmdWrite(0x06);
        //lcd_initialise();
    	lcdcmdWrite(0x80); // sets cursor
    	//lcdWrite("Menu 1"); 
        lcdWrite("Set Temp");
    	menu = 1;
        break;
    case 2: 
        lcdcmdWrite(0x01); //clears screen
        lcdcmdWrite(0x06);
        //lcd_initialise();
    	lcdcmdWrite(0x80); // sets cursor
    	//lcdWrite("Menu 1"); 
        lcdWrite("Set Min Temp:");
    	_delay_ms(600);
    	menu = 2;
        break;
    case 3:
        lcdcmdWrite(0x01); //clears screen
        lcdcmdWrite(0x0C); //display on cursor off
        lcdcmdWrite(0x06);
        //lcd_initialise();
		lcdcmdWrite(0x80);
        lcdWrite("Set Max Temp:");
    	_delay_ms(600);
    	menu = 3;
        break;
    case 5:
    	menu = 1;
    	break;
    	default:
    		menu = 0;
  }
}

// interfacing with the 16x2 lcd in 4 bit mode

void print_data(char dta){
  if(dta & 0x10 == 0x10){ PORTB |= 0b00001000;}
  else                   { PORTB &= 0b11110111;}
  
  if(dta & 0x20 == 0x20){ PORTB |= 0b00000100;}
  else                   { PORTB &= 0b11111011;}
  
  if(dta & 0x40 == 0x40){ PORTB |= 0b00000010;}
  else                   { PORTB &= 0b11111101;}
  
  if(dta & 0x80 == 0x80){ PORTB |= 0b00000001;}
  else                   { PORTB &= 0b11111110;}
}


//function for writing instructions to lcd
void lcdWrite(char *data_1){
  
  char data;
  int len = strlen(data_1);
  
  for(int i=0; i<len; i++){
    if(i == 16){
      lcdcmdWrite(0xC0);
    }
    
    data=data_1[i];
    
    PORTB |= 0b00100000;
    
    if((data&0b00010000)>>4 == 0x01){PORTB |= 0b00001000;}
    else                            {PORTB &= 0b11110111;}
    if((data&0b00100000)>>5 == 0x01){PORTB |= 0b00000100;}
    else                            {PORTB &= 0b11111011;}
    if((data&0b01000000)>>6 == 0x01){PORTB |= 0b00000010;}
    else                            {PORTB &= 0b11111101;}
    if((data&0b10000000)>>7 == 0x01){PORTB |= 0b00000001;}
    else                            {PORTB &= 0b11111110;}
    
    //_delay_ms(15);
    PORTB |= 0b00010000;
    _delay_ms(2);
    PORTB &= 0b11101111;
    //_delay_ms(15);
    
    if((data&0b00000001)>>0 == 0x01){PORTB |= 0b00001000;}
    else                            {PORTB &= 0b11110111;}
    if((data&0b00000010)>>1 == 0x01){PORTB |= 0b00000100;}
    else                            {PORTB &= 0b11111011;}
    if((data&0b00000100)>>2 == 0x01){PORTB |= 0b00000010;}
    else                            {PORTB &= 0b11111101;}
    if((data&0b00001000)>>3 == 0x01){PORTB |= 0b00000001;}
    else                            {PORTB &= 0b11111110;}
    
    //_delay_ms(15);
    PORTB |= 0b00010000;
    _delay_ms(2);
    PORTB &= 0b11101111;
    //_delay_ms(15);
  }
  
}

void lcdcmdWrite(char data){
  
  	PORTB &= 0b11011111;
    
    if((data&0b00010000)>>4 == 0x01){PORTB |= 0b00001000;}
    else                            {PORTB &= 0b11110111;}
    if((data&0b00100000)>>5 == 0x01){PORTB |= 0b00000100;}
    else                            {PORTB &= 0b11111011;}
    if((data&0b01000000)>>6 == 0x01){PORTB |= 0b00000010;}
    else                            {PORTB &= 0b11111101;}
    if((data&0b10000000)>>7 == 0x01){PORTB |= 0b00000001;}
    else                            {PORTB &= 0b11111110;}
    
    //_delay_ms(15);
    PORTB |= 0b00010000;
    //_delay_ms(15);
    PORTB &= 0b11101111;
    //_delay_ms(15);
    
    if((data&0b00000001)>>0 == 0x01){PORTB |= 0b00001000;}
    else                            {PORTB &= 0b11110111;}
    if((data&0b00000010)>>1 == 0x01){PORTB |= 0b00000100;}
    else                            {PORTB &= 0b11111011;}
    if((data&0b00000100)>>2 == 0x01){PORTB |= 0b00000010;}
    else                            {PORTB &= 0b11111101;}
    if((data&0b00001000)>>3 == 0x01){PORTB |= 0b00000001;}
    else                            {PORTB &= 0b11111110;}
    
    //_delay_ms(15);
    PORTB |= 0b00010000;
    //_delay_ms(15);
    PORTB &= 0b11101111;
    //_delay_ms(15);
}


void lcd_initialise(){
  
  lcdcmdWrite(0x32); // initialisation of 4 bit mode
  lcdcmdWrite(0x28); // 2 line, 5*7
  lcdcmdWrite(0x01); // clear display
  lcdcmdWrite(0x0C); //display on cursor off
  lcdcmdWrite(0x06); // increment cursor
}
