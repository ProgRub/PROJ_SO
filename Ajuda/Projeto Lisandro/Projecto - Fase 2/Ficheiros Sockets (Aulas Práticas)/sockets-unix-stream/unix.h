#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define UNIXSTR_PATH "/tmp/s.unixstr"
#define UNIXDG_PATH  "/tmp/s.unixdgx"
#define UNIXDG_TMP   "/tmp/dgXXXXXXX"

