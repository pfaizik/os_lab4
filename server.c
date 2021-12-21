#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server.h"
#include "settings.h"
#include <fcntl.h>
#include "http.h"

int SET_Delay(char Delay){
    switch(Delay){
        case('1'):
            return 1;
        case('2'):
            return 2;
        case('5'):
            return 5;
    }
}

void SET_LEDS(char* indata, char* LEDS, char BUT){
    switch(BUT){
        case('0'):
            if(*(LEDS + 3) == '1') {
                strcpy(indata, "led_1010: \n");
                *LEDS = '1';
                *(LEDS + 1) = '0';
                *(LEDS + 2) = '1';
                *(LEDS + 3) = '0';
            }
            else{
                strcpy(indata, "led_0101: \n");
                *LEDS = '0';
                *(LEDS + 1) = '1';
                *(LEDS + 2) = '0';
                *(LEDS + 3) = '1';
            }
            break;
        case('1'):
            if(*(LEDS + 3) == '1') {
                strcpy(indata, "led_1010: \n");
                *LEDS = '1';
                *(LEDS + 1) = '0';
                *(LEDS + 2) = '1';
                *(LEDS + 3) = '0';
            }
            else{
                strcpy(indata, "led_0101: \n");
                *LEDS = '0';
                *(LEDS + 1) = '1';
                *(LEDS + 2) = '0';
                *(LEDS + 3) = '1';
            }
            break;
        case('2'):
            strcpy(indata, "led_1010: \n");
            *LEDS = '1';
            *(LEDS + 1) = '0';
            *(LEDS + 2) = '1';
            *(LEDS + 3) = '0';
            break;
    }
}

char Button = '4';

int server_init (char *ip, int port, int numofclients);
 
void *server(void *shbuf) {
    
    // print info:
    fprintf(stderr, "server was started\r\n");
    
    // get arg:
    struct st_shbuf *sharedbuf = (struct st_shbuf *) shbuf;       // преобразуем тип аргумента
    
    // server initialization:
    int lfd = server_init(IP, PORT, 1); 
    
    // forever loop for client accept:
    while (1) {
 
        // wait for client:
        int cfd = accept(lfd, NULL, NULL);
        if (cfd == -1) {
            fprintf(stderr, "error: cannot accept client. Sorry\n");
            exit (-1);
        } else {
            SERVER_INFO("client was connected");
        }
        
        // loop for data exchange (echo mode):
        char *request   = malloc(WEBBUF_SIZE);
        char *response  = malloc(WEBBUF_SIZE);
        char *response_buff  = malloc(WEBBUF_SIZE);
        char *html      = malloc(WEBBUF_SIZE);
        char *data;
        int f, datalen;
        char Button_buff;
        char Delay = '1';
        char LEDS[4] = {"1010"};
        int i = 0;
        
        while (1) {
 
            // recieve request from server:
            memset(request, 0, WEBBUF_SIZE);
            if (recv(cfd, request, WEBBUF_SIZE, 0) == -1) {
                SERVER_INFO("connection was closed");
                break;
            } 
            else {
                SERVER_INFO("client request:\n%s", request);
            }
//            printf("+++++++++++++++++++++++++++++++++++++++++++++++==\n%s\n++++++++++++++++++++++++++++++++++++++++++++++++++\n", request);
            // parse request:
            memset(response, 0, WEBBUF_SIZE);
            memset(response_buff, 0, WEBBUF_SIZE);
            switch (http_parse_request(request)) {
                
                // 3.2.2.1. send index.html to web:
                case HTTP_REQ_INDEX_HTML:
 
                    // read index.html:
                    f = open(INDEXHTML_PATH, O_RDONLY);
                    memset(html, 0, WEBBUF_SIZE);
                    
                    if (f == -1) {
                        getcwd(html, WEBBUF_SIZE);
                        SERVER_INFO("%s not found in %s dir", INDEXHTML_PATH, html);
                    } else {
                        datalen = read(f, html, WEBBUF_SIZE);
                    }
                    close(f);

                    // create http response:
                    http_200OK(response, html);
 
                break;
                
                // 3.2.2.2. send data from shared buffer to web:
                case HTTP_REQ_READDATA:
                    pthread_mutex_lock(sharedbuf->mutex);
                    Button_buff = Button;                    
                    
                    SET_LEDS(response_buff, &LEDS, Button_buff);
                    sleep(SET_Delay(Delay));
                    
                    http_200OK(response, response_buff);
                    memset(sharedbuf->buf, 0, sharedbuf->bufsize);
                    pthread_mutex_unlock(sharedbuf->mutex);
                    
 
                break;
                
                // 3.2.2.3. get data from web and send to stdout:
                case HTTP_REQ_WRITEDATA:
                    
                    // get data from web and send to stdout:
                    datalen = http_find_data (request,  &data);
                    if (datalen != -1) {
                        data[datalen] = '\n';
                        data[datalen + 1] = '\0';
                        write(STDOUT_FILENO, data, datalen+2);
                        if((strcmp(data, "Delay: 5\n") == 0)||(strcmp(data, "Delay: 2\n") == 0)||(strcmp(data, "Delay: 1\n") == 0)){
                            Delay = data[7];
                        }
                        else if((strcmp(data, "BUTTON0: clicked\n") == 0)||(strcmp(data, "BUTTON1: clicked\n") == 0)||(strcmp(data, "BUTTON2: clicked\n") == 0)){
                            pthread_mutex_lock(sharedbuf->mutex);
                            Button = data[6];
                            pthread_mutex_unlock(sharedbuf->mutex);
                        }
                    }
                    //printf("XXXXXXXXXXXXXXXXXXXXXX\n%s\nXXXXXXXXXXXXXXXXXXXXXXX",data);
                    // response to web:
                    http_200OK(response, "");
                    
                break;
                
                // 3.2.2.4. If receive unknown HTTP-packet:
                case HTTP_REQ_OTHER:
                    http_200OK(response, "");
                    SERVER_INFO("BBBBBBBBBBBBBBBBBBBBBBBBBBB");
                break;
 
            }

            
            // send answer:
           // printf("-----------------------------------------------\n%s\n--------------------------------------------------\n", response);
            
            if (send(cfd, response, strlen(response), 0) == -1) {
                SERVER_INFO("connection was closed");
                break;
            } else {
                SERVER_INFO("server response:\n%s", response);
            }
 
            
            
        }
 
        // close client connection:
        free (request);
        free (response);
        free (response_buff);
        close (cfd);
 
    } 
 
    // print info:
    fprintf(stderr, "server was finished\r\n");
    
}


int server_init (char *ip, int port, int numofclients) {
    
    // create a socket:
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        SERVER_ERROR("cannot create a socket");
    }
 
    // bind socket to interface:
    int anyip = (in_addr_t *) ip == (in_addr_t *)INADDR_ANY;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family 	= PF_INET;
    servaddr.sin_addr.s_addr 	= anyip? INADDR_ANY : inet_addr(ip);
    servaddr.sin_port 		= htons(port);
 
    if (bind(lfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        SERVER_ERROR ("cannot bind socket to interface with IP: %s", anyip? "any" : ip);
        exit(-1);
    }
 
    // create a queue for clients:
    if (listen(lfd, numofclients) == -1) {
        SERVER_ERROR("cannot create a queue (system call listen)");
    }
 
    fprintf(stderr, "server was started IP: %s, PORT: %d\n", ip, port);
    
    return lfd;
    
}

