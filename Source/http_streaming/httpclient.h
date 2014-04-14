
#ifndef __HTTPCLIENT_H
#define __HTTPCLIENT_H

#include "utils/lwiplib.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
//#include "ethernet.h"

// Public function
extern int hc_open(struct ip_addr, long localport);

// Function that lwip calls for handling recv'd data
err_t hc_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

extern void stop_connection(void);
#endif //  __WEBCLIENT_H
