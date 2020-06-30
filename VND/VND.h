//
// Created by hao on 23/06/2020.
//

#ifndef CARP_VND_H
#define CARP_VND_H


#include "../functions.h"
#include "../heutistic.h"

#define M_NUM 2500

void VND(const Task *inst_tasks);
int vnd_shorten( int *task_seq,  const Task *inst_tasks);
int vnd_merge_switch(int *new_seq, int (*Route)[250], const int *choice, const Task *inst_tasks);
void vnd_switch1(const Task *inst_tasks);
void vnd_cut(int *split_task_seq, const int *one_task_seq, const Task *inst_tasks);

#endif //CARP_VND_H
