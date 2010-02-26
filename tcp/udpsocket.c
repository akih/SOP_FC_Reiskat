/*
 * udpsocket.c
 *
 *  Created on: 4.2.2010
 *      Author: Administrator
 */
#include <dev/board.h>

#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>

#include <sys/version.h>
#include <sys/timer.h>
#include <pro/snmp_config.h>
#include <pro/snmp_mib.h>
#include <pro/snmp_api.h>
#include <pro/snmp_agent.h>
#include "mib2sys.h"
#include "mib2if.h"
#include "snmp.h"

#define HOST_IP		"169.254.16.243"
#define HOST_PORT 	161

UDPSOCKET *udpsock;
int view_idx;
OID view_all[] = { SNMP_OID_INTERNET, 3 };
char buffer[500] = "";
char *buff;

void udpsocket(void){

	u_short port = 161;
	u_long address = inet_addr(HOST_IP);
	int i;
	buff = buffer;

	printf("Tassa ollaan 1\n");

	udpsock = NutUdpCreateSocket(SNMP_PORT);
	/* Nut/Net doesn't provide UDP datagram buffering by default. */
	{
		u_short max_ms = SNMP_MAX_MSG_SIZE * 2;
		NutUdpSetSockOpt(udpsock, SO_RCVBUF, &max_ms, sizeof(max_ms));
	}

	printf("Tassa ollaan 2\n");

	/* Register system variables. */
	if (MibRegisterSysVars()) {
		printf("Failed to register MibSys\n");
		for (;;)
		NutSleep(1000);
	}

	/* Register interface variables. */
	if (MibRegisterIfVars()) {
		printf("Failed to register MibIf\n");
		for (;;)
		NutSleep(1000);
	}

	/* Register interface variables. */
	if (MibRegisterOwnVars()) {
		printf("Failed to register MibOwn\n");
		for (;;)
		NutSleep(1000);
	}

	/* Create views. */
	if ((view_idx = SnmpViewCreate("all", view_all, sizeof(view_all), SNMP_VIEW_INCLUDED)) <= 0) {
		printf("Failed to create view\n");
		for (;;)
			NutSleep(1000);
	}

	/* Create communities. */
	if (SnmpCommunityCreate("SOP2010", view_idx, view_idx)) { //|| SnmpCommunityCreate("private", view_idx, view_idx)) {
		printf("Failed to create communities\n");
		for (;;)
			NutSleep(1000);
	}

	SnmpAgent(udpsock);

	/*
	printf("%s\n",buffer);

	for(;;){
	if (NutUdpReceiveFrom(udpsock, &address, &port, buff, 500, 10000) < 1)
	{
		printf("Ei saatu dataa\n");
	}
	else
	{
		printf("%s\n",buffer);
		for (i=0; i<500; i++)
		{
			buffer[i]=0;
		}
	}
	NutSleep(1000);
	}
	*/

	NutUdpDestroySocket(udpsock);
	for (;;) {
		NutSleep(100);
		printf("Hello ");
	}
}

