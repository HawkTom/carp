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
void OX(Individual *xed_child, Individual *p1, Individual *p2, const Task *inst_tasks);
void sort_pop(Individual *pop, int popsize);
void partial_replacement(Individual *pop, int nc, int nrep, const Task *inst_tasks);

#endif //CARP_LMA_H
