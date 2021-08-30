#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>

#include <cstdio>
#pragma once

void fork_off() {
    const auto pid = fork();
    if (-1 == pid) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
}

void daemonize(const char* dir) {
    fork_off();

    if (-1 == setsid()) { // does it require sudo? probably not
        perror("setsid error");
    }

    // not sure if really necessary right now:
    {
        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);

        fork_off();
    }

    // umask(...), but don't want to use it yet

    if (-1 == chdir(dir)) {
        perror("chdir error");
    }

    if (EOF == fclose(stdin)) {
        perror("fclose(stdin) error");
    }
    if (EOF == fclose(stdout)) {
        perror("fclose(stdout) error");
    }
    if (EOF == fclose(stderr)) {
        perror("fclose(stderr) error");
    }

    // seems unnecessary right now
    // for (int fd = sysconf(_SC_OPEN_MAX); 0 <= fd; --fd) {
    //     if (-1 == close(fd)) {
    //         perror("close error");
    //     }
    // }
}