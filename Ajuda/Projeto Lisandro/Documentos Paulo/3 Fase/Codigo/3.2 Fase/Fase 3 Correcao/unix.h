#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

#define ADDRESS      "champ_socket"

#define UNIXSTR_PATH "/tmp/socket.unix.stream1"
#define UNIXDG_PATH  "/tmp/socket.unix.datagram"
#define UNIXDG_TMP   "/tmp/socket.unix.dgXXXXXXX"

#define MAX_LINE 512