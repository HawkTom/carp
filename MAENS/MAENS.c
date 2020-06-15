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




}