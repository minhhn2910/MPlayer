#include "radio.h"
#include "mp3driver.h"
#include "Global.h"
#include "httpclient.h"
struct ip_addr remote_ip;
unsigned long local_port =2222;



//*****************************************************************//
//lwIPHostTimerHandler 
//Prototype: void lwIPHostTimerHandler(void)
//Description: handle Lwip host interrupt
//Returns: None.
//*****************************************************************//

void lwIPHostTimerHandler(void)
{

}

//*****************************************************************//
//init_Ethernet 
//Prototype: void init_Ethernet(void)
//Description: initial ethernet and lwip module
//Returns: None.
//*****************************************************************//

void init_Ethernet()
{
	unsigned long ulUser0, ulUser1;
	unsigned char pucMACArray[8];
	// Enable and Reset the Ethernet Controller.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
	SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

	// Enable Port F for Ethernet LEDs.
	//  LED0        Bit 3   Output
	//  LED1        Bit 2   Output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	

	FlashUserGet(&ulUser0, &ulUser1);
	if((ulUser0 == 0xffffffff) || (ulUser1 == 0xffffffff))
	{
		UARTprintf("FATAL ERROR: NO MAC FOUND\n");
		while(1)
		{
		}
	}

	// Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
	// address needed to program the hardware registers, then program the MAC
	// address into the Ethernet Controller registers.
	pucMACArray[0] = ((ulUser0 >>  0) & 0xff);
	pucMACArray[1] = ((ulUser0 >>  8) & 0xff);
	pucMACArray[2] = ((ulUser0 >> 16) & 0xff);
	pucMACArray[3] = ((ulUser1 >>  0) & 0xff);
	pucMACArray[4] = ((ulUser1 >>  8) & 0xff);
	pucMACArray[5] = ((ulUser1 >> 16) & 0xff);

	// Initialze the lwIP library, using DHCP.
	// lwIPInit(pucMACArray, 0, 0, 0, IPADDR_USE_DHCP);
	lwIPInit(pucMACArray, 0xc0a8015a, 0xffffff00,  0xc0a80101, IPADDR_USE_STATIC);

}

//**************************************************//
//start_internet_radio 
//Prototype: void start_internet_radio(void)
//Description: start radio application
//Returns: None.
//**************************************************//
void start_internet_radio()
{

	MP3_stream_mode();
	UARTprintf("start internet radio\n");
	IP4_ADDR(&remote_ip, 192,168,1,99);
	local_port++;
	if(local_port>60000)
	local_port = 10000;
	hc_open(remote_ip,local_port);	 

}


//*************************************************//
//stop_internet_radio 
//Prototype: void stop_internet_radio(void)
//Description: stop radio application
//Returns: None.
//************************************************//
void stop_internet_radio()
{
	UARTprintf("stop internet radio\n");
	stop_connection();

}

