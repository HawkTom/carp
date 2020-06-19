//
// Created by hao on 15/06/2020.
//

#ifndef CARP_LMA_H
#define CARP_LMA_H

#include "../functions.h"
#include "../heutistic.h"


void LMA(const Task *inst_tasks);
void evaluate_route(Individual *indi, const Task *inst_tasks);
int* rand_perm(int num);

#endif //CARP_LMA_H
