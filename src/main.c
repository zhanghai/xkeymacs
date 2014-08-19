/**
 * @file main.c
 * @author Zhang Hai
 */

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

#include "xkey.h"
#include "keymacs.h"

void init_deamon();

int main(int argc, char **argv) {

    // init_daemon()；

    xkey_initialize();

    keymacs_on_bind_key();

    xkey_loop();

    xkey_finalize();

    return 0;
}

/**
 * Implemented according to man page daemon(7).
 */
void init_daemon() {

    struct rlimit fd_limit;
    int i;
    sigset_t sigset;
    pid_t pid;

    getrlimit(RLIMIT_NOFILE, &fd_limit);
    for (i = 3; i < fd_limit.rlim_cur; ++i) {
        close(i);
    }

    for (i = 1; i < _NSIG; ++i) {
        signal(i, SIG_DFL);
    }

    sigemptyset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    setsid();

    pid = fork();
    if (pid == -1) {
        perror("fork");
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    i = open("/dev/null", O_RDWR);
    dup(i);
    dup(i);

    umask(0);
    chdir("/");
}
