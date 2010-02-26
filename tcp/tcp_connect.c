/*
 * tcp_connect.c
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

#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>

#include <sys/version.h>
#include <sys/timer.h>
#include <sys/socket.h>
#include <string.h>
//#include "png/png.h"


void tcp_connect(char *MYIP,char *HOST_IP){

	int counter = 0;

 //   u_long baud = MYBAUD;
 //   u_char mac[6] = { MYMAC };

//    TCPSOCKET *serverSock;
    TCPSOCKET *clientSock;

    FILE *stream1;
    //FILE *stream2;
    char buff[128] = "";

    //char OK[] = "OK";
    char teksti[300] = "";

    int index = 0;
    int length = 0;
    int i;
    int a;



	for(;;){
    		puts("");
    		printf("Yritys numero: %d\n", counter);
    		puts("Koitetaan saada sukka aikaan...");
    		if ((clientSock = NutTcpCreateSocket()) != 0) {
    			puts("Sukka muodostettu");
    			//puts("Yritetaan yhdistaa osoitteeseen: " HOST_IP);
    			if (NutTcpConnect(clientSock, inet_addr(HOST_IP), 1025) == 0) {
    				puts("Yhteys");
    				NutSleep(1000);
    				//while(1) {printf("%d bittien maara\n",NutTcpSend(clientSock, OK, strlen(OK)));}
    				//NutTcpReceive(clientSock, buffi, strlen(buffi));
					//puts("Viesti lahetetty");
    				if ((stream1 = _fdopen((int) clientSock, "r+b")) != 0) {

    					puts("Eka");
    					//NutSleep(1000);
        				fputs("get", stream1);


        				fflush(stream1);
        				NutSleep(1000);


        				puts("Toka");
        				//printf("%d",fclose(stream1));
        				//fclose(stream1);



							//puts("tassa on eka teksti: \n");
							while(1) {

								fgets(buff, sizeof(buff), stream1);
								length = strlen(buff);
								for (i=0, a=index; i<length; i++, a++)
								{
									teksti[a]=buff[i];
									index++;

								}
								printf("%s",buff);


								//fprintf(txtfile,"%s",buff);
								//fputs(buff, txtfile);
								printf("%d\n", length);
								if (buff[0] == 'e' && buff[1] == 'n' && buff[2] == 'd') {

									//fclose(txtfile);
									break;
								}

							}
							/*puts("\ntassa on toka teksti: \n");
							while(1) {

														fgets(buff, sizeof(buff), stream2);
														printf("%s",buff);
														//fprintf(txtfile,"%s",buff);
														if (strlen(buff) == 0) {
															//fclose(txtfile);
															break;
														}

													}*/

						}

    				//puts("Taulukon sisalto");
    				//bputs(teksti);
    				//fputs(teksti, txtfile);
    				//for(i=0; i<length; i++){
    				//printf("%s\n",teksti);
    				//}

    				NutTcpCloseSocket(clientSock);
    				printf("\nloppu");
    				return ;
    			}else
    				puts("Ei yhteyttä");
					NutTcpCloseSocket(clientSock);
    		}else
    			puts("Ei saatu sukkaa aikaan");
    	NutSleep(1000);
    	counter++;
    	}

	return;
	}
