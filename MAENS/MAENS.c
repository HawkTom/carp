//
// Created by hao on 15/06/2020.
//

#include "MAENS.h"


void MAENS(Task *inst_tasks)
{
    int i, j, k;



    int popsize = 30;
    Individual pop[MAX_TOTALSIZE];
    Individual best_fsb_solution;

    best_fsb_solution.TotalCost = INF;

    // initilization
    int tmp_popsize = 0;
    int used;
    while (tmp_popsize < popsize)
    {
        int trial = 0;
        Individual init_indi;
        while (trial < M_trial)
        {
            trial ++;
            int serve_mark[MAX_TASK_TAG_LENGTH];
            memset(serve_mark, 0, sizeof(serve_mark));
            for (i = 1; i <= task_num; i++)
            {
                serve_mark[i] = 1;
            }

            rand_scanning(&init_indi, inst_tasks, serve_mark);
            used = 0;
            for (i = 0; i < tmp_popsize; i++)
            {
                if (init_indi.TotalCost == pop[i].TotalCost && init_indi.TotalVioLoad == pop[i].TotalVioLoad)
                {
                    used = 1;
                    break;
                }
            }
            if ( !used )
                break;
        }

        if (trial == M_trial && used == 1)
            break;

//        indi_copy(&pop[tmp_popsize], &init_indi);

        pop[tmp_popsize] = init_indi;
        tmp_popsize ++;
        if(init_indi.TotalVioLoad == 0 && init_indi.TotalCost < best_fsb_solution.TotalCost)
        {
            best_fsb_solution = init_indi;
        }
    }
    popsize = tmp_popsize;

    // main loop

    int ite, wite;
    Individual parent1, parent2, xed_child, mted_child, child;

    int offsize = 6*popsize;
    int totalsize = popsize + offsize;

    ite = 0;
    wite = 0;
    while (ite < M_ite)
    {
        ite ++;
        wite ++;

        int ptr = popsize;
        while (ptr < totalsize)
        {
            child.TotalCost = 0;

            // randomly select two parents
            int par_id1, par_id2;
            rand_selection(&par_id1, &par_id2, popsize);
            parent1 = pop[par_id1];
            parent2 = pop[par_id2];

            // crossover




        }



    }




}
















