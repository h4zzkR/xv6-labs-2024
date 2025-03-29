#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(2, "usage: grep pattern [file ...]\n");
        exit(1);
    }

    int seconds = atoi((const char*)(argv[1]));

    while ((seconds = sleep(seconds)) != 0) {
    }
    
    exit(0);
}
