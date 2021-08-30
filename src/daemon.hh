#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#include <cstdio>
#pragma once

void daemonize(const char* dir) {
    if (-1 == chdir(dir)) {
        perror("chdir error");
    }
    if (-1 == setsid()) {
        perror("setsid error");
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
}