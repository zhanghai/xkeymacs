/**
 * @file main.c
 * @author Zhang Hai
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log.h"
#include "keymacs.h"
#include "xkey.h"

static void print_help();
static void trap_finalize();
static void finalize_handler(int sig);
static void init_daemon();

int main(int argc, char **argv) {

    if (argc == 2) {
        if (strcmp(argv[1], "-d") == 0
                || strcmp(argv[1], "--daemon") == 0) {
            init_daemon();
        } else if (strcmp(argv[1], "-h") == 0
                || strcmp(argv[1], "--help") == 0) {
            print_help();
            return EXIT_SUCCESS;
        } else {
            log_error("main: Unknown argument");
            printf("\n");
            print_help();
            return EXIT_FAILURE;
        }
    } else if (argc > 2) {
        log_error("main: Too many arguments");
        printf("\n");
        print_help();
        return EXIT_FAILURE;
    }

    xkey_initialize();

    keymacs_on_bind_key();

    trap_finalize();

    xkey_loop();

    return EXIT_FAILURE;
}

static void print_help() {
    printf("xkeymacs - X11 KEYboard MACroS\n"
           "Usage:\n"
           "\txkeymacs [OPTION]\n"
           "Options:\n"
           "\t-d, --daemon\n"
           "\t\tstart xkeymacs as daemon\n"
           "\t-h, --help\n"
           "\t\tdisplay this help and exit\n");
}

static void trap_finalize() {
    signal(SIGHUP, finalize_handler);
    signal(SIGINT, finalize_handler);
    signal(SIGQUIT, finalize_handler);
    signal(SIGTERM, finalize_handler);
}

static void finalize_handler(int sig) {

    log_info("finalize_handler: Finalizing, signal=%d", sig);
    xkey_finalize();

    log_info("finalize_handler: Exiting");
    exit(EXIT_SUCCESS);
}

/**
 * Implemented according to man page daemon(7).
 */
static void init_daemon() {

    struct rlimit fd_limit;
    int i;
    sigset_t sigset;
    pid_t pid;

    getrlimit(RLIMIT_NOFILE, &fd_limit);
    for (i = 0; i < fd_limit.rlim_cur; ++i) {
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
