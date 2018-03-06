//
// Created by genshen on 2018-3-4.
//
#include <stdio.h>

void sunwayAThreadInit() {
    athread_init();
    int i;
    i = athread_get_max_threads();
    printf("max thread:%d", i); // todo only print in master processor.
}

void sunwayAThreadClean() {
    athread_halt();
}