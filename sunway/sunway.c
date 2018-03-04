//
// Created by genshen on 2018-3-4.
//
#include <stdio.h>

extern "C" {
#include <athread.h>
}

void sunwayAThreadInit() {
    athread_init();
    int i;
    i = athread_get_max_threads();
    printf("max thread:%d", i);
}

void sunwayAThreadClean() {
    athread_halt();
}