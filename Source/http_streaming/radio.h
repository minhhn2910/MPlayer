#ifndef __RADIO_H__
#define __RADIO_H__
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/ethernet.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/lwiplib.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "httpclient.h"
#include "lwip/dns.h"

#include <stdlib.h>
#include "Global.h"


extern void lwIPHostTimerHandler(void);
extern void init_Ethernet(void);
extern void start_internet_radio(void);
extern void stop_internet_radio(void);
#endif  // Global.h
