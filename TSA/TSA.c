//
// Created by hao on 13/06/2020.
//


#include "TSA.h"

void TSA(Task *inst_tasks)
{
    int i, j,k, m;

    int NVer, NRE, NRA, NNR, NVeh, Cap, LLB;
    NVer = vertex_num;
    NRE = req_edge_num;
    NRA = req_arc_num;
    NNR = 2*nonreq_edge_num+nonreq_arc_num;
    NVeh = vehicle_num;
    Cap = capacity;
    LLB = 0; // Lower bound can be obtained from some paper.


    Individual InitSolution;
    int ServMark[2*NRE+NRA+1]; // the mark for served tasks
    memset(ServMark, 1, sizeof(ServMark));
    path_scanning(&InitSolution, inst_tasks, ServMark);

    InitSolution.Fitness = InitSolution.TotalCost;

    printf("TSA: initial is ok \n");

    // following five variables are defined for Frederick Heuristic
    int Route[250], FHRoute[250], Position[50], TmpSeq[250], Cost1, Cost2;

    TmpSeq[0] = 1; // tmp solution to link all processed sub-routes
    InitSolution.TotalCost = 0;

    // improve initial solution by Frederick Heuristic, 这一段原文没有

    find_ele_positions(Position, InitSolution.Sequence, 0);
    for(i = 1; i < Position[0]; i++)
    {
        AssignSubArray(InitSolution.Sequence, Position[i], Position[i+1], Route);
        FredericksonHeuristic(FHRoute, Route, inst_tasks);
        Cost1 =get_task_seq_total_cost(Route, inst_tasks);
        Cost2 = get_task_seq_total_cost(FHRoute, inst_tasks);

        TmpSeq[0] --; // ??
        if (Cost1 < Cost2)
        {
            JoinArray(TmpSeq, Route); //link two routes
            InitSolution.TotalCost += Cost1;
        } else {
            JoinArray(TmpSeq, FHRoute);
            InitSolution.TotalCost += Cost2;
        }
    }

    InitSolution.Fitness = InitSolution.TotalCost;
    AssignArray(TmpSeq, InitSolution.Sequence);

    // For each required edge, which routes they located in.
    InitSolution.Assignment[0] = NRE + NRA; // record as the tabu list
    j = 1;
    for (i = 2; i <= InitSolution.Sequence[0]; i++)
    {
        if (InitSolution.Sequence[i] == 0)
        {
            j ++;
            continue;
        }
        int tmp = InitSolution.Sequence[i];
        if (tmp > NRE)
            tmp -= NRE;

        InitSolution.Assignment[tmp] = j; // record which routes each edge locates in.
    }

    Individual CurrSolution, BestSolution, BestFsbSolution, NextSolution, TmpSolution;
    int TabuList[2*NRE+NRA+1][50];
    memset(TabuList, 0, sizeof(TabuList));
    int TabuTenure = (NRE + NRA) / 2;

    CurrSolution = InitSolution;
    BestSolution = InitSolution;

    BestFsbSolution.TotalCost = 0;
    if (InitSolution.TotalVioLoad == 0)
    {
        BestFsbSolution = InitSolution;
    }

    double PenPrmt = 1; //penalty parameter
    int ConstFsb, ConstInFsb; // the number of iterations for feasible/unfeasible solution

    int FSI, FDI, FSWAP; //frequency of the different types of move

    ConstFsb = 0;
    ConstInFsb = 0;

    FSI = 1;
    FDI = 1; // 5 in original paper
    FSWAP = 1; // 5 in original paper

    int npi, mnpi, nti, mnti;

    mnpi = 900 * (int)sqrt(NRE + NRA); //

    npi = 0; // iteration
    nti = 0; //

    Individual BestSINeighbor, BestDINeighbor, BestSWAPNeighbor;
    int ChangedRoutesID[2], MovedTasks[3], BestMovedTasks[3], RID1, RID2;
    int TotalFitEval = 0, FitEval = 0;
    while (BestFsbSolution.TotalCost > LLB)
    {
        NextSolution.Fitness = INF;
        int improved = 0;

        if (npi % FSI == 0)
        {
            // single insertion
            FitEval = SingleInsertion(&BestSINeighbor, &CurrSolution, MovedTasks, ChangedRoutesID, inst_tasks, TabuList, PenPrmt, BestSolution.Fitness, BestFsbSolution.Fitness);
            printf("Single Insertion, %d\n", FitEval);
            TotalFitEval += FitEval;
        }

        if (npi % FDI == 0)
        {
            // double insertion
            FitEval = DoubleInsertion(&BestDINeighbor, &CurrSolution, MovedTasks, ChangedRoutesID, inst_tasks, TabuList, PenPrmt, BestSolution.Fitness, BestFsbSolution.Fitness);
            printf("Double Insertion, %d\n", FitEval);
            TotalFitEval += FitEval;
        }

        if (npi % FSWAP == 0)
        {
            // swap
            FitEval = SWAP(&BestSWAPNeighbor, &CurrSolution, MovedTasks, ChangedRoutesID, inst_tasks, TabuList, PenPrmt, BestSolution.Fitness, BestFsbSolution.Fitness);
            printf("SWAP, %d\n", FitEval);
            TotalFitEval += FitEval;
        }
    }

}




































