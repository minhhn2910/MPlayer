#include "Global.h"
#include "peripheral.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/uartstdio.h"
#include "mp3driver.h"
#include "drivers/rit128x96x4.h"
#include "ebook.h"

// **********************************
// select_CARD
// Prototype: void select_CARD(void)
// Description: enable SDCARD module.
// Returns: None.
// **********************************
void select_CARD()
{
	initSSI();

	GPIOPinWrite(CARD_CS,0);
}


// *************************************
// deselect_CARD
// Prototype: void deselect_CARD(void)
// Description: disable SDCARD module.
// Returns: None.
// *************************************
void deselect_CARD()
{
	

	GPIOPinWrite(CARD_CS,GPIO_PIN_0);
}

// ***************************************
// select_LCD
// Prototype: void select_LCD(void)
// Description: enable LCD module.
// Returns: None.
// ***************************************
void select_LCD()
{
	RIT128x96x4Enable(2000000);
	//	GPIOPinWrite(LCD_CS,0);
}


// ******************************************
// deselect_LCD
// Prototype: void deselect_LCD(void)
// Description: disable LCD module.
// Returns: None.
// ******************************************
void deselect_LCD()
{
	RIT128x96x4Disable();
	//GPIOPinWrite(LCD_CS,GPIO_PIN_3);
}

void deselect_MP3()
{
	
	GPIOPinWrite(MP3_CS,MP3_CS_PIN);
}


// **********************************************
// select_MP3
// Prototype: void select_MP3(void)
// Description: enable MP3 module.
// Returns: None
// **********************************************
void select_MP3()
{
	initSSI();
	GPIOPinWrite(MP3_CS,0);
}



