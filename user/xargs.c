#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LINE MAXPATH

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    char *xargv[MAXARG];
    int i, n;
    char c;

    // eliminate old program name
    for (i = 1; i < argc; i++) {
        xargv[i-1] = argv[i];
    }
    xargv[argc-1] = 0;

    // I am too lazy now to split the line...
    while (1) {
        int pos = 0;
        while (pos < sizeof(line) - 1) {
            n = read(0, &c, 1);
            if (n <= 0 || c == '\n') {
                break;
            }
            line[pos++] = c;
        }
        
        if (n <= 0 && pos == 0) {
            break;
        }

        line[pos] = 0;

        xargv[argc-1] = line;
        xargv[argc] = 0;

        if (fork() == 0) {
            exec(xargv[0], xargv);
            fprintf(2, "exec %s failed\n", xargv[0]);
            exit(1);
        } else {
            wait(0);
        }
    }

    exit(0);
}