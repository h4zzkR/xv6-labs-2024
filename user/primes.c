#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define LIMIT 280

typedef int bool;
#define TRUE 1
#define FALSE 0

#define NULL 0

#define DEBUG TRUE

#define debug_print(fmt, ...)             \
    do                                    \
    {                                     \
        if (DEBUG)                        \
            fprintf(2, fmt, __VA_ARGS__); \
    } while (0)

#define ERR_CHECK(call, msg)                                                  \
    do                                                                        \
    {                                                                         \
        int __ret = (call);                                                   \
        if (__ret < 0)                                                        \
        {                                                                     \
            fprintf(2, "ERROR (pid=%d): %s. ret=%d\n", getpid(), msg, __ret); \
            exit(1);                                                          \
        }                                                                     \
    } while (0)

int send_to_routine(int number, int channel_to_routine)
{
    return write(channel_to_routine, &number, sizeof(int));
}

int get_from_routine(int channel_to_routine, int *number)
{
    return read(channel_to_routine, number, sizeof(int));
}

int spawn_routine(int channel_to_spawned[2], int old_pipe);

void filter_routine(int channel_in)
{
    int n, init_prime;
    int ret = get_from_routine(channel_in, &init_prime);
    if (ret == 0)
    {
        close(channel_in);
        return;
    }
    ERR_CHECK(ret, "Failed to read from pipe");

    fprintf(1, "prime %d\n", init_prime);

    int channel_to_spawned[2];
    ERR_CHECK(pipe(channel_to_spawned), "Failed to pipe");

    spawn_routine(channel_to_spawned, channel_in);

    while (1)
    {
        ret = get_from_routine(channel_in, &n);
        if (ret == 0)
        {
            break;
        }
        ERR_CHECK(ret, "Failed to read number from pipe");

        if (n % init_prime != 0)
        {
            ERR_CHECK(
                send_to_routine(n, channel_to_spawned[1]),
                "Failed to send number to next stage");
        }
    }

    close(channel_in);
    close(channel_to_spawned[1]);
}

int spawn_routine(int channel_to_spawned[2], int old_pipe)
{
    int pid = fork();
    ERR_CHECK(pid, "Failed to fork");

    if (pid == 0)
    {
        if (old_pipe != -1)
        {
            close(old_pipe);
        }
        close(channel_to_spawned[1]);
        filter_routine(channel_to_spawned[0]);
        wait(0);
        exit(0);
    }
    else
    {
        close(channel_to_spawned[0]);
        return pid;
    }
}

int main(int argc, char *argv[])
{
    int channel_to_spawned[2];
    ERR_CHECK(pipe(channel_to_spawned), "Failed to pipe");
    spawn_routine(channel_to_spawned, -1);

    for (int i = 2; i <= LIMIT; i++)
    {
        ERR_CHECK(
            send_to_routine(i, channel_to_spawned[1]),
            "Failed to send number in main");
    }

    close(channel_to_spawned[1]);
    wait(NULL);

    exit(0);
}