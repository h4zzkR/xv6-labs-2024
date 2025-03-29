#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    char buf = 'x';
    int parent_to_child[2];
    int child_to_parent[2];

    pipe(parent_to_child);
    pipe(child_to_parent);

    if (fork() == 0) {
        close(child_to_parent[0]);
        close(parent_to_child[1]);

        read(parent_to_child[0], &buf, 1);
        close(parent_to_child[0]);

        int pid = getpid();
        fprintf(1, "%d: received ping\n", pid);

        write(child_to_parent[1], &buf, 1);
        close(child_to_parent[1]);
    } else {
        close(child_to_parent[1]);
        close(parent_to_child[0]);

        write(parent_to_child[1], &buf, 1);
        close(parent_to_child[1]);

        read(child_to_parent[0], &buf, 1);
        close(child_to_parent[0]);

        int pid = getpid();
        fprintf(1, "%d: received pong\n", pid);
    }

    exit(0);
}