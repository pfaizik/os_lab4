#ifndef SETTINGS_H
#define SETTINGS_H
// maximum bytes from stdin:
#define STDIN_DATA_SIZE 128
// size of buffer (bytes) for data exchange between main and server:
#define SHAREDBUF_SIZE 1024
// server settings:
#define SRV_INFO 1 // print info msgs to stderr (0 - dis, 1 - en)
#define IP "127.0.1.1"
#define PORT 80
#define WEBBUF_SIZE 1024*32 // size of buffer for data of client
// macro for error events:
#define SERVER_ERROR(fmt, ...) do { fprintf(stderr, "server error: " fmt "\n",##__VA_ARGS__); exit(-1); } while (0)
// macro for info messages:
#define SERVER_INFO(fmt, ...) do { if (SRV_INFO) fprintf(stderr, "server info: " fmt"\n", ##__VA_ARGS__); } while (0)
#define INDEXHTML_PATH "index.html"


#endif /* SETTINGS_H */

