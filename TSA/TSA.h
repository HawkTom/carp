//
// Created by hao on 13/06/2020.
//

#ifndef CARP_TSA_H
#define CARP_TSA_H

# include "../functions.h"
# include "../heutistic.h"

int SingleInsertion(Individual *BestSINeighbor, Individual *CurrSolution, int *MovedTasks, int *ChangedRoutesID, Task *inst_tasks, int (*TabuList)[50],
                    double PenPrmt, double BestFitness, double BestFsbFitness );
int DoubleInsertion(Individual *BestDINeighbor, Individual *CurrSolution, int *MovedTasks, int *ChangedRoutesID, Task *inst_tasks, int (*TabuList)[50],
                    double PenPrmt, double BestFitness, double BestFsbFitness);
int SWAP(Individual *BestSWAPNeighbor, Individual *CurrSolution, int *MovedTasks, int *ChangedRoutesID, Task *inst_tasks, int (*TabuList)[50],
                    double PenPrmt, double BestFitness, double BestFsbFitness);
void RepairInfeasibility (Individual *Indi, Task *inst_tasks);

void TSA(Task *inst_tasks);

#endif //CARP_TSA_H
