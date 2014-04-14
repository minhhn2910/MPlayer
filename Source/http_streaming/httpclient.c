#include <stdlib.h>
#include <string.h>
#include "httpclient.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "mp3driver.h"
#include "Global.h"
struct tcp_pcb *pcb_temp;
char skip;
// Close a PCB(connection)

//************************************************//
//stop_connection 
//Prototype: void stop_connection(void)
//Description: close current tcp connetion
//Returns: None.
//************************************************//
void stop_connection()
{
	if(pcb_temp != NULL)
	{
		// Close the TCP connection
		tcp_close(pcb_temp);
	}
}

// Function that lwip calls for handling recv'd data
err_t hc_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	struct pbuf * temp_p;
	int i;
	if(skip<2)
	skip++;

	if(volume_change !=0)
	{
		MP3_Set_Volume(volume, volume);
		volume_change =0;
	}
	
	if((err == ERR_OK) && (p != NULL))
	{
		tcp_recved(pcb, p->tot_len);

		// Add payload (p) to state
		temp_p = p;

		
		while(temp_p != NULL)
		{
			if(skip<2) //skip some frames
			break;
			for(i=0;i<temp_p->len;i++)
			MP3_SDI_Write(((char*)temp_p->payload)[i]);
			temp_p = temp_p->next;
			
		}

		while(p != NULL)
		{
			temp_p = p->next;
			pbuf_free(p);
			p = temp_p;
		}

	}
	// 		if(btnSelect ==1)
	// 		{
	// 			btnSelect =0;
	// 			stop_flag = 1;
	// 			UARTprintf("stop flag_set before %d\n",stop_flag);
	// 			stop_connection();
	// 				UARTprintf("stop flag_set after %d\n",stop_flag);
	// 		}
	return(ERR_OK);
}

// Function that lwip calls when there is an error
static void hc_error(void *arg, err_t err)
{

}

// Function that lwip calls when the connection is idle
// Here we can kill connections that have stayed idle for too long
static err_t hc_poll(void *arg, struct tcp_pcb *pcb)
{
	return(ERR_OK);
}

// lwip calls this function when the remote host has successfully received data (ack)
static err_t hc_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	return(ERR_OK);
}

// lwip calls this function when the connection is established
static err_t hc_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
	char  * headers;

	// error?
	if(err != ERR_OK)
	{
		stop_connection();

		return(ERR_OK);
	}

	// GET headers (without page)(+ \0) = 19
	headers = malloc(19);
	usprintf(headers,"GET / HTTP/1.0\r\n\r\n");

	// Setup the TCP receive function
	tcp_recv(pcb, hc_recv);

	// Setup the TCP error function
	tcp_err(pcb, hc_error);

	// Setup the TCP polling function/interval	 //TCP_POLL IS NOT CORRECT DEFINED @ DOC!!!
	tcp_poll(pcb, hc_poll, 10);					 	

	// Setup the TCP sent callback function
	tcp_sent(pcb, hc_sent);

	// Send data
	tcp_write(pcb, headers, strlen(headers), 1);
	tcp_output(pcb);

	// remove headers
	free(headers);
	return(ERR_OK);
}


// Public function for request a webpage (REMOTEIP, ...
//****************************************************//
//hc_open 
//Prototype: int hc_open(struct ip_addr remoteIP , long localport)
//Parameter: struct ip_addr: remote ip, localport 
//Description: Public function for request a http page from remote ip, use localport
//Returns: return integer value tell us sucess or error.
//***************************************************//
int hc_open(struct ip_addr remoteIP, long localport)
{
	struct tcp_pcb *pcb = NULL;
	static u8_t num = 0;
	err_t error1;
	u16_t port=localport; 	
	//skip some frames to make mp3 quality stable
	skip=0;
	
	pcb = tcp_new();
	if(pcb == NULL)
	{
		UARTprintf("hc_open: Not enough memory for pcb or state\n");	
		//Not enough memory
		return 0;
	}
	pcb_temp = pcb;
	// Bind to local IP & local port
	while(tcp_bind(pcb, IP_ADDR_ANY, port) != ERR_OK)
	{
		// Local port in use, use port+1
		port++;
	}

	// Use conn -> argument(s)
	tcp_arg(pcb, NULL);

	// Open connect (SEND SYN) 
	
	error1 = tcp_connect(pcb, &remoteIP, 8080, hc_connected);
	UARTprintf("return %d \n",error1 );

	return num;
}
