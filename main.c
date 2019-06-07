/*
 * [smass]_CostumeProject2.c
 *
 * Created: 6/6/2019 1:29:35 AM
 * Author : shirinmassoumi
 */ 
/*
 * [smass]_CustomProject.c
 *
 * Created: 5/10/2019 11:16:08 AM
 * Author : shirinmassoumi
 */ 

#include <avr/io.h>
#include <bit.h>
#include <avr/interrupt.h>
//#include <timer.h>
#include "timer.h"
#include <stdio.h>
#include "io.c"
#include "oo.c"
#include "io.h"
#include "keypad.h"
#include "scheduler.h"
#include <stdlib.h>
//#include "lcd_8bit_task.h"

//Task 1
//LED function and shared variables
int Temperature_LitLED(int a);
enum Temperature_LitLEDState {init_TLED, LED_one, LED_two, LED_three, LED_four, LED_five, LED_six, OverFlow};
int TLL_nextState;
unsigned short ADC_value; //ADC value stored in a char
//unsigned char Temp_state; //The flag to communicate between Temperature_LitLED and Motor_controller_fct
//unsigned short tempC;  
unsigned char tempC = 0;


//Task2
//Displaying temp and shared variables
int Display_temp(int b);
enum Display_tempState {init_DT, Display_Temp, Delay_Display};
int DT_nextState;
unsigned char tempA1;
char TemperatureValuesChar [2]; //null terminated string
int TemperatureValue; //The value of temperature from ADC to integer
unsigned short ADC_temp_value;

//Task3
int ADC_mux(int c);
enum ADC_MuxState {ADC_temp,  ADC_Joystick};
int ADCM_nextState;
unsigned short JoyStick_value;
char tempstring [4];

//Task 4
int DisplayLastTen(int z);
enum DisplayLastTenSM {wait_DLT, display_DLT, release_DLT};
int DLT_next;
unsigned char FLAG;
unsigned char Celsuis = 0x43; 

int DisplayArray [10];
int lastTen [10];
int DisplayCount =0;
char Dstring[2];

//Task 5
int  JoystickControl (int d);
enum  JoystickControl_State {wait_JC, up, down, center};
int JC_nextState;
unsigned char JoyStick_Position = 5;


//Task 5
/*
int Display_LastTenTemp  (int e);
enum Display_LastTenTemp_State {};
unsigned char DLTT_nextState;
*/

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

void LCD_Custom_Char (unsigned char loc, unsigned char *msg)
{
    unsigned char i;
    if(loc<8)
    {
     LCD_WriteCommand(0x40 + (loc*8));  /* Command 0x40 and onwards forces 
                                       the device to point CGRAM address */
       for(i=0;i<8;i++)  /* Write 8 byte for generation of 1 character */
           LCD_WriteData(msg[i]);      
    }   
}
void LCD_Custom_Char1 (unsigned char loc, unsigned char *msg)
{
    unsigned char i;
    if(loc<8)
    {
     LCD_WriteCommand1(0x40 + (loc*8));  /* Command 0x40 and onwards forces 
                                       the device to point CGRAM address */
       for(i=0;i<8;i++)  /* Write 8 byte for generation of 1 character */
           LCD_WriteData1(msg[i]);      
    }   
}

unsigned char degree[8] = {
	0b01110,
	0b01010,
	0b01110,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

int main(void)
{
	DDRA = 0x18; PORTA = 0xE7; //OUTPUT FOR LED's ALL PINS
	//DDRA = 0xFC; PORTA = 0x03; //OUTPUT FOR LED's ALL PINS
	DDRB = 0xFF; PORTB = 0x00; //Input on pin PINB0 and PINA, PINB0 -> TEMP SENSOR, PINB1 -> INPUT BUTTON FOR LCD
	DDRC = 0xFF; PORTC = 0x00; //output 
	DDRD = 0xFF; PORTD = 0x00; //ALL OUTPUT FOR LCD bus

	//double PWM_value;
	
	TimerSet(10);
	TimerOn();
	ADC_init();
	LCD_init();
	LCD_init1();

	LCD_Custom_Char(0, degree);
	LCD_WriteCommand(0x80);
	LCD_Custom_Char1(0, degree);
	LCD_WriteCommand1(0x80);
	
	 // Value of ADC register now stored in variable x
	/* Replace with your application code */
	
	//Declare an array of tasks
	static task task1, task2, task3, task4, task5;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	//Task 1
	//There was only one task to do so the size = 1;

	task1.state = 0;
	task1.period = 10; //Task Period.
	task1.elapsedTime =task1.period;
	task1.TickFct = &Temperature_LitLED;

	//Task 2
	task2.state = 0;
	task2.period = 10; //Task Period.
	task2.elapsedTime = task2.period;
	task2.TickFct = &Display_temp;

	//Task 3
	task3.state = 0;
	task3.period = 10; //Task Period.
	task3.elapsedTime = task3.period;
	task3.TickFct = &ADC_mux;

	//Task 4
	task4.state = 0;
	task4.period = 10; //Task Period.
	task4.elapsedTime = task4.period;
	task4.TickFct = &DisplayLastTen;

	task5.state = 0;
	task5.period = 10; //Task Period.
	task5.elapsedTime = task5.period;
	task5.TickFct = &JoystickControl;
	//
	
	LCD_DisplayString(1, "Temperature:");
	LCD_DisplayString1(1, "Prev Temp:");
	unsigned short i; // Scheduler for-loop iterator
	
	while(1) {
		
		tempA1 = (~PINA & 0x02) >>1; //Button input to turn on the LED with the temperature value display
		for ( i = 0; i < numTasks; i++ ) {
			if ( tasks[i]->elapsedTime >= tasks[i]->period ) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
	
}

int Temperature_LitLED (int TLL_state){
	const unsigned short MAX = 896;
	const unsigned short MIN = 500;
	//unsigned short ADC_value;
	//unsigned short Threshhold = MAX/10;
	//unsigned char tempC;
	//ADC_value = ADC; 
	ADC_value = ADC_temp_value - MIN;
	//89
	 	
	switch(TLL_state){
	case init_TLED:
		TLL_nextState = LED_one;
	break;

	 case LED_one:
	 if (ADC_value<= 50 ){
		  TLL_nextState = LED_one;
	  }
	  else {
		  TLL_nextState = LED_two;
	  }
	 break;
	 
	case LED_two:
	if(ADC_value <50 ){
		TLL_nextState = LED_one;
	}
	else if(ADC_value <100){
		TLL_nextState = LED_two;
	}
	else {
		TLL_nextState = LED_three;
	}
	break;
	
	case LED_three:
	if(ADC_value <100 ){
		TLL_nextState = LED_two;
	}
	else if(ADC_value <150){
		TLL_nextState = LED_three;
	}
	else {
		TLL_nextState = LED_four;
	}
	break;
	
	case LED_four:
	if(ADC_value < 150 ){
		TLL_nextState = LED_three;
	}
	else if(ADC_value <200){
		TLL_nextState = LED_four;
	}
	else {
		TLL_nextState = LED_five;
	}
	break;
	
	case LED_five:
	if(ADC_value <200 ){
		TLL_nextState = LED_four;
	}
	else if(ADC_value < 250){
		TLL_nextState = LED_five;
	}
	else {
		TLL_nextState = LED_six;
	}
	break;
	
	case LED_six:
	if(ADC_value < 250 ){
		TLL_nextState = LED_five;
	}
	else if(ADC_value <300) {
		TLL_nextState = LED_six;
	}
	else{
		TLL_nextState = OverFlow;
	}
	break;
	
	case OverFlow:
	if(ADC_value <300){
		TLL_nextState = LED_six;
	}
	else{
		TLL_nextState = OverFlow;
	}
	break;
	
	default:
	TLL_nextState = init_TLED;
	break; 
	}
	
	//ACTIONS of this state machine
	switch(TLL_state){
		
		case init_TLED:
		tempC = 0x00;
		break;
		
		case LED_one:
		tempC = 0x01;
		break;
		
		case LED_two:
		tempC = 0x02;
		break;
		
		case LED_three:
		tempC = 0x04;
		break;
		
		case LED_four:
		tempC = 0x08;
		break;
		
		case LED_five:
		tempC = 0x10;
		break;
		
		case LED_six:
		tempC = 0x20;
		break;
		
		case OverFlow:
		tempC = 0x3F;
		break;
	}
	
	PORTD = tempC;
	TLL_state = TLL_nextState;
	return TLL_state;
}

//This SM will display the temperature which is controlled by a button A1
//It will update & display the temperature every 100ms

int Display_temp (int DT_state){
	//unsigned char DisplayTempString;
	//This function stores the value of ADC value in a char string and the base is 10

	itoa(ADC_value, TemperatureValuesChar, 10); 
	TemperatureValue = atoi(TemperatureValuesChar);
	TemperatureValue = (TemperatureValue*0.09) + 22.01;
	itoa(TemperatureValue, TemperatureValuesChar, 10);
	TemperatureArray();
	//unsigned char temptemptemp = TemperatureValuesChar;
	
	switch(DT_state){
		case init_DT:
		if(tempA1==1){
			DT_nextState = Display_Temp;
		}
		else{
			DT_nextState = init_DT;
		}
		break;
		
		case Delay_Display:
			DT_nextState = Display_Temp;
		break;
		
		case Display_Temp:
			DT_nextState = Delay_Display;
		break;
		
		default:
			DT_nextState = init_DT;
		break;
	}
	
	switch(DT_state){
		case init_DT:
		break;
		
		case Delay_Display:
		delay_ms(100);
		break;
		
		case Display_Temp:
		LCD_Cursor(13);
		LCD_WriteData(TemperatureValuesChar[0]);
		LCD_Cursor(14);
		LCD_WriteData(TemperatureValuesChar[1]);
		LCD_Cursor(15);
		LCD_WriteData(0);
		LCD_Cursor(16);
		LCD_WriteData(Celsuis);
		
		break;
	}
	
	DT_state = DT_nextState;
	return DT_state;
}

int ADC_mux (int c){
	
	switch(c){
		case ADC_temp:
		ADCM_nextState = ADC_Joystick;
		break;
		
		case ADC_Joystick:
		ADCM_nextState = ADC_temp;
		break;
		
		default:
		ADCM_nextState = ADC_temp;
		break;
		
	}
	//actions
	switch(c){
		case ADC_temp:
		ADMUX = 0;
		delay_ms1(10);
		ADC_temp_value = ADC;
		//itoa(ADC_temp_value,tempstring,10);
		//LCD_DisplayString1(1, tempstring);
		break;
		
		case ADC_Joystick:
		ADMUX = 2;
		delay_ms1(10);
		JoyStick_value = ADC;
		//itoa(JoyStick_value,tempstring,10);
		//LCD_DisplayString1(1, tempstring);
		break;
	}
	
	c = ADCM_nextState;
	return ADCM_nextState;
	
}

void TemperatureArray()
{
	DisplayCount++;
	if(DisplayCount < 11)
	{
		lastTen[DisplayCount - 1] = TemperatureValue;
	}
	else
	{
		for(int i = 0; i < 9; i++)
		{
			lastTen[i] = lastTen[i+1];
		}
		lastTen[9]= TemperatureValue;
	}
}



int DisplayLastTen(int z)
{
	unsigned char DisplayPin = (~PINA & 0x20) >> 5;
	//itoa(DisplayArray[9], tempstring, 10);
	//LCD_DisplayString1(1, DisplayPin);
	switch(z)
	{
		case wait_DLT:
			if(DisplayPin == 1)
			{
				DLT_next = display_DLT;
			}
			else
			{
				DLT_next = wait_DLT;
			}
		break;
		
		case display_DLT:
			DLT_next = release_DLT;
		break;
		
		case release_DLT:
			if(DisplayPin == 1)
			{
				DLT_next = release_DLT;
			}
			else
			{
				DLT_next = wait_DLT;
			}
		break;
	}
	
	switch(z)
	{
		case wait_DLT:
		FLAG = 0;
		break;
		
		case display_DLT:
		FLAG = 1;
		for(int i = 0; i < 10; i++)
		{
			DisplayArray[i] = lastTen[i];
		}
		break;
		
		case release_DLT:
		FLAG=0;
		break;
	}
	
	z = DLT_next;
	return DLT_next;
}


int JoystickControl (int d)
{
	switch(d)
	{
		case wait_JC:
		if(JoyStick_value <400)
		{
			JC_nextState = down;
		}
		else if(JoyStick_value > 600)
		{
			JC_nextState = up;
		}
		else
		{
			JC_nextState = wait_JC;
		}
		
		break;

		case up:
			JC_nextState = center;
		break;
		
		case down:
			JC_nextState = center;
		break;

		case center:
		if(JoyStick_value >= 400 && JoyStick_value <= 600)
		{
			JC_nextState = wait_JC;
		}
		else
		{
			JC_nextState = center;
		}
		break;
	}
	//Actions
	switch(d)
	{
		case wait_JC:
		
		if(FLAG ==1){
		itoa(DisplayArray[JoyStick_Position] , Dstring, 10);
		LCD_Cursor1(13);
		LCD_WriteData1(Dstring[0]);
		LCD_Cursor1(14);
		LCD_WriteData1(Dstring[1]);
		LCD_Cursor(15);
		LCD_WriteData1(0);
		LCD_Cursor(16);
		LCD_WriteData1(Celsuis);
		//LCD_DisplayString1(1, Dstring);
		}
		break;

		case up:
		if(JoyStick_Position >= 9){
			JoyStick_Position = 9;
		}
		else {
			JoyStick_Position++; 
		}
		
		itoa(DisplayArray[JoyStick_Position] , Dstring, 10);
		LCD_Cursor1(13);
		LCD_WriteData1(Dstring[0]);
		LCD_Cursor1(14);
		LCD_WriteData1(Dstring[1]);
		LCD_Cursor(15);
		LCD_WriteData1(0);
		LCD_Cursor(16);
		LCD_WriteData1(Celsuis);
		//LCD_DisplayString1(1, Dstring);
		
		break;
		
		case down:
		if(JoyStick_Position <= 0){
			JoyStick_Position = 0;
		}
		else {
			JoyStick_Position--; 
		}

		itoa(DisplayArray[JoyStick_Position] , Dstring, 10);
		LCD_Cursor1(13);
		LCD_WriteData1(Dstring[0]);
		LCD_Cursor1(14);
		LCD_WriteData1(Dstring[1]);
		LCD_Cursor(15);
		LCD_WriteData1(0);
		LCD_Cursor(16);
		LCD_WriteData1(Celsuis);
		//LCD_DisplayString1(1, Dstring);
		
		break;

		case center:
		break;
	}

	d = JC_nextState;
	return JC_nextState;

}
