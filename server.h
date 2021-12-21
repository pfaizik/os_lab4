#ifndef SERVER_H
#define SERVER_H
#include <pthread.h>
#include <sys/types.h>
// shared buffer for data exchange between main and server:
struct st_shbuf {
 char *buf;
 ssize_t bufsize;
 pthread_mutex_t *mutex;
};
extern char Button;
extern void *server(void *shbuf);
#endif /* SERVER_H */