#include <dev/board.h>

#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>

#include <sys/version.h>
#include <sys/timer.h>

#include <pro/snmp_config.h>
#include <pro/snmp_mib.h>
#include <pro/snmp_api.h>
#include <pro/snmp_agent.h>

#include <string.h>
#include <sys/socket.h>


#include "udpsocket.h"
#include "tcp_connect.h"


/*!
 * \example snmpd/snmpd.c
 *
 * Basic SNMP Agent.
 */
static char *version = "0.2.0";

/*! \brief Default MAC.
 *  Used if EEPROM is empty. */
#define MYMAC   0x00, 0x06, 0x98, 0x33, 0x44, 0x00

/*! \brief Default local IP address.
 *  Used if EEPROM configuration and DHCP is unavailable. */
#define MYIP    "169.254.16.244"
#define HOST_IP    "169.254.16.243"

/*! \brief Default IP mask.
 *  Used if EEPROM configuration and DHCP is unavailable. */
#define MYMASK  "255.255.255.0"

/*! \brief Default gateway address.
 *  Used if EEPROM configuration and DHCP is unavailable.
 *  Only required if syslog or time server are located in
 *  a different network. */
#define MYGATE  "192.168.192.1"

/*! \brief Device name used for UART output. */
#define MYUART  DEV_UART_NAME

/*! \brief Output device. */
#define MYDEV   DEV_UART

/*! \brief UART baudrate. */
#define MYBAUD  115200

/* Determine the compiler. */
#if defined(__IMAGECRAFT__)
#if defined(__AVR__)
#define COMPILERNAME   "ICCAVR"
#else
#define COMPILERNAME   "ICC"
#endif
#elif defined(__GNUC__)
#if defined(__AVR__)
#define COMPILERNAME   "AVRGCC"
#elif defined(__arm__)
#define COMPILERNAME   "ARMGCC"
#else
#define COMPILERNAME   "GCC"
#endif
#else
#define COMPILERNAME   "Compiler unknown"
#endif



int main(int argc, char** argv) {

	int counter = 0;

    u_long baud = MYBAUD;
    u_char mac[6] = { MYMAC };

    TCPSOCKET *serverSock;
    TCPSOCKET *clientSock;

    FILE *stream1;
    //FILE *stream2;
    char buff[128] = "";

    //char OK[] = "OK";
    char teksti[100] = "";

    int index = 0;
    int lenght = 0;
    int i;
    int a;



    /*
    FILE *txtfile = fopen("client","w");

    fclose(txtfile);

    FILE *test;
    if((test = fopen("client", "r")) != NULL)
    {puts("taitaa se olla"); }
	*/






    while(NutRegisterDevice(&MYDEV, 0, 0)) {}

    freopen(MYUART, "w", stdout);
    freopen(MYUART, "r", stderr);

    while(_ioctl(_fileno(stdout), UART_SETSPEED, &baud)) {}
    printf("\n");

    puts("Registering LAN device...");

    while (NutRegisterDevice(&DEV_ETHER, 0x8300, 5)) {
    }
    puts("LAN device registered");

    puts("Configuring network interface...");

    while (NutNetIfConfig("eth0", mac, inet_addr(MYIP), inet_addr(MYMASK))) {
    }
    puts("Network interface configured");
    puts("\n My ip is: " MYIP);

   // if ((udpsock = NutUdpCreateSocket(161)) != 0){
   // 	printf("UDP socket created\n");
   // }






    printf("SnmpAgent start running\n");
    /* Run agent. */
    udpsocket();


    //tcp_connect(MYIP, HOST_IP);



    return 0;
}
