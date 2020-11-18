#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define UNIXSTR_PATH "server-stream.socket35784"
#define UNIXDG_PATH  "server-dgram.socket35784"
#define UNIXDG_TMP   "client-dgram.socketXXXXXX"
