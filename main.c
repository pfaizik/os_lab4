#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include "settings.h"
#include "server.h"



int main() {

 // print info:
 fprintf(stderr, "main was started\r\n");

 // create shared buffer for data from stdin between main and server:
 struct st_shbuf *sharedbuf = malloc(sizeof(struct st_shbuf)); // создаем в куче
 sharedbuf->bufsize = SHAREDBUF_SIZE; // запоминаем размер буфера
 sharedbuf->buf = malloc(sharedbuf->bufsize); // память в куче под буфер
 memset(sharedbuf->buf, 0, sharedbuf->bufsize); // заливаем буфер нулями
 sharedbuf->mutex = malloc(sizeof(pthread_mutex_t)); // память под мьютекс
 pthread_mutex_init(sharedbuf->mutex, NULL); // инициализация мьютекса

 // create new thread for server:
 pthread_t thread;
 if (pthread_create(&thread, NULL, server, (void *)sharedbuf) != 0) {
 fprintf(stderr, "error: pthread_create was failed\r\n");
 exit(-1);
 }

 // forever loop:
 char indata[STDIN_DATA_SIZE];
 char LEDS[4] = {"1010"};
 int j;
 while (1) {

 // 3.1. get data from stdin:
 fgets(indata, sizeof(indata), stdin);
 
 if((strcmp(indata, "button0: clicked\n") == 0)||(strcmp(indata, "button1: clicked\n") == 0)||(strcmp(indata, "button2: clicked\n") == 0)){
     pthread_mutex_lock(sharedbuf->mutex);
     Button = indata[6];
     strcpy(sharedbuf->buf, &indata);
     pthread_mutex_unlock(sharedbuf->mutex);
 }
 else{
 // 3.2. push to buffer:
 pthread_mutex_lock(sharedbuf->mutex);
 if ((sharedbuf->bufsize - strlen(sharedbuf->buf) - 1) > strlen(indata)) {
 strcat(sharedbuf->buf, indata);
 }
 pthread_mutex_unlock(sharedbuf->mutex);
 }
 // 3.3. clear input buf:
 memset(indata, 0, sizeof(indata));

 }

 // wait server thread exit:
 pthread_join(thread, NULL);

 // print info:
 fprintf(stderr, "main was finished\r\n");
 free(sharedbuf->buf); // освобождаем память
 free(sharedbuf->mutex); // освобождаем память
 free(sharedbuf); // освобождаем память
 return (0);

}