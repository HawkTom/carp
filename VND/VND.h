//
// Created by hao on 23/06/2020.
//

#ifndef CARP_VND_H
#define CARP_VND_H


#include "../functions.h"
#include "../heutistic.h"


void VND(const Task *inst_tasks);
void vnd_shorten( int *task_seq,  const Task *inst_tasks);
void vnd_merge_switch(int *new_task_seq, int *Sequence, const int *choice, const Task *inst_tasks);
void vnd_switch1(const Task *inst_tasks);

#endif //CARP_VND_H
