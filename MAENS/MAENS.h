//
// Created by hao on 15/06/2020.
//

#ifndef CARP_MAENS_H
#define CARP_MAENS_H


#define MAX_POPSIZE 30
#define MAX_TOTALSIZE 210
#define MAX_NONDOMINATED_NUM 1000

#define M_trial 10
#define M_PROB 0.2
#define M_ite 100
#define M_wite 100

#define MAX_NSIZE 10 // upper bound of nsize
#define MAX_ENSSIZE 100 // maximal ENS neighborhood size


# include "../functions.h"

void MAENS(Task *inst_tasks);

void rand_scanning(Individual *rs_indi, const Task *inst_tasks, const int *serve_mark);
void indi_copy(Individual *target, Individual *source);
void rand_selection(int *id1, int *id2, int popsize);

#endif //CARP_MAENS_H
