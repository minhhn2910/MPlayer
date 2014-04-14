#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "utils/uartstdio.h"
#include "driverlib/gpio.h"
#include "Global.h"
#include "buttons.h"

unsigned char buttons_state = NON;
unsigned char btnUpHold=0;
unsigned char btnDownHold=0;
unsigned char btnLeftHold=0;
unsigned char btnRightHold=0;
unsigned char btnSelectHold=0;

unsigned char btnUp=0;
unsigned char btnDown=0;
unsigned char btnLeft=0;
unsigned char btnRight=0;
unsigned char btnSelect=0;
int count_button = 0 ;
//this state machine only catch select hold for function

//***********************************************//
//buttons_process
//Prototype: void buttons_process (void)
//Description: perform state machine that processes events when user pressed a button
//Returns: None
//***********************************************//
void buttons_process()
{
	switch(buttons_state)
	{
		case NON:
			count_button = 0;	
			if(UP_PRESSED )
			{
				random_number = random_number/2;
				count_button++;
				buttons_state = UP;
			}					
			else if(DOWN_PRESSED )
			{
				random_number = random_number/3;
				count_button++;
				buttons_state = DOWN;
			}
			else if(RIGHT_PRESSED )
			{
				random_number = random_number/4;
				count_button++;
				buttons_state = RIGHT;	
			}
			else if(LEFT_PRESSED )
			{
				random_number = random_number/5;
				count_button++;
				buttons_state = LEFT;	
			}
			else if(SELECT_PRESSED )
			{
				random_number = random_number/6;
				count_button++;
				buttons_state = SELECT;	
			}			
		break;
		case UP:
			UARTprintf("UP \n");
			if(UP_PRESSED )
			{
				count_button++;
				if(count_button>LONG_BUTTON)
				{
					count_button = 0;
					buttons_state = UP_HOLD;
				}
			}	
			else
			{	
				if(count_button>SHORT_BUTTON)
					btnUp=1;
				buttons_state = NON;
			}
		break;
		case DOWN:
			UARTprintf("DOWN \n");
			if(DOWN_PRESSED )
			{
				count_button++;
				if(count_button>LONG_BUTTON)
				{
					count_button = 0;					
					buttons_state = DOWN_HOLD;
				}
			}	
			else
			{	
				if(count_button>SHORT_BUTTON)
					btnDown=1;
				buttons_state = NON;
			}			
		break;
		case LEFT:
			UARTprintf("LEFT \n");
		if(LEFT_PRESSED )
			{
				count_button++;
				if(count_button>LONG_BUTTON)
				{
					count_button = 0;
					buttons_state = LEFT_HOLD;
				}
			}	
			else
			{	
				if(count_button>SHORT_BUTTON)
					btnLeft=1;
				buttons_state = NON;
			}					
		break;
		case RIGHT:
			UARTprintf("RIGHT \n");
		if(RIGHT_PRESSED )
			{
				count_button++;
				if(count_button>LONG_BUTTON)
				{
					count_button = 0;
					buttons_state = RIGHT_HOLD;
				}
			}	
			else
			{	
				if(count_button>SHORT_BUTTON)
					btnRight=1;
				buttons_state = NON;
			}					
		break;
		case SELECT:
			UARTprintf("SELECT \n");
			if(SELECT_PRESSED )
			{
				count_button++;
				if(count_button>LONG_BUTTON)
				{
					count_button = 0;
					buttons_state = SELECT_HOLD;
				}
			}	
			else
			{	
				if(count_button>SHORT_BUTTON)
					btnSelect=1;
				buttons_state = NON;
			}					
		break;
		case UP_HOLD:
			UARTprintf("up hold \n");
		if(UP_PRESSED )
			{
				count_button++;
				if(count_button>MIDDLE_BUTTON)
					btnUp = 1;
			}		
			else
			{
				btnUp = 1;
				buttons_state = NON;
			}
		break;
		case DOWN_HOLD:
			UARTprintf("down hold \n");
		if(DOWN_PRESSED )
			{
				count_button++;
				if(count_button>MIDDLE_BUTTON)
					btnDown = 1;
			}		
			else
			{
				btnDown = 1;
				buttons_state = NON;
			}			
		break;
		case LEFT_HOLD:
			UARTprintf("left_hold \n");
			if(LEFT_PRESSED )
			{
				count_button++;
				if(count_button>MIDDLE_BUTTON)
					btnLeft = 1;
			}		
			else
			{
				btnLeft = 1;
				buttons_state = NON;
			}			
		break;
		case RIGHT_HOLD:
			UARTprintf("right_hold \n");
		if(RIGHT_PRESSED )
			{
				count_button++;
				if(count_button>MIDDLE_BUTTON)
					btnRight = 1;
			}		
			else
			{
				btnRight = 1;
				buttons_state = NON;
			}			
		break;
		case SELECT_HOLD:
			UARTprintf("select_hold \n");
		if(SELECT_PRESSED )
			{
				count_button++;
				if(count_button>MIDDLE_BUTTON)
					btnSelectHold = 1;
			}		
			else
			{
				btnSelectHold = 1;
				buttons_state = NON;
			}			
		break;
		default:
			
		break;
	}
}

//************************************************//
//Init_buttons
//Prototype: void init_buttons(void)
//Description: initial state of GPIO pins related to buttons  
//Returns: None
//************************************************//
void init_buttons()
{
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);   
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_1, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
}
