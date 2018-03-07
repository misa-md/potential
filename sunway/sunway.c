//
// Created by genshen on 2018-3-4.
//

/**
 * in this file,we initial & clean athread environment.
 */

#include <stdio.h>
#include "sunway.h"

void sunwayAThreadInit() {
    athread_init();
    int i;
    i = athread_get_max_threads();
    printf("max thread:%d", i); // todo only print in master processor.
}

void sunwayAThreadClean() {
    athread_halt();
}