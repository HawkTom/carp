//
// Created by hao on 15/06/2020.
//

#include "LMA.h"

#define MNT 50

void LMA(const Task *inst_tasks)
{
    int i, j, k;
    int nc = 30;
    Individual pop[40];

    // path-scanning
    Individual tmp_indi1;
    int ServeMark[2*req_edge_num + req_arc_num + 1];
    memset(ServeMark, 1, sizeof(ServeMark));
    path_scanning(&tmp_indi1, inst_tasks, ServeMark);
    evaluate_route(&tmp_indi1, inst_tasks);
    pop[0] = tmp_indi1;

    // augment-merge
    Individual tmp_indi2;
    augment_merge(&tmp_indi2, inst_tasks);
    evaluate_route(&tmp_indi2, inst_tasks);
    pop[1] = tmp_indi2;

    // Ulusoy's heuristic: FH + Ulusoy's split
    Individual tmp_indi3;
    int FHRoute[250], Route[250];
    memset(Route, 0, sizeof(Route));
    Route[0] = req_edge_num + req_arc_num + 2;
    for (i = 2; i <= req_edge_num+req_arc_num+1; i++)
    {
        Route[i] = i-1;
    }
    FredericksonHeuristic(FHRoute, Route, inst_tasks);
    tmp_indi3.Assignment[0] = 0;
    for (i = 2; i < FHRoute[0]; i++)
    {
        tmp_indi3.Assignment[0] ++;
        tmp_indi3.Assignment[tmp_indi3.Assignment[0]] = FHRoute[i];
    }
    tmp_indi3.TotalCost = split(tmp_indi3.Sequence, tmp_indi3.Assignment, inst_tasks);
    tmp_indi3.TotalVioLoad = 0;
    tmp_indi3.Fitness = 0;
    pop[2] = tmp_indi2;



    int tmp_popsize = 3;
    int used;
    while (tmp_popsize < nc)
    {
        int trial = 0;
        Individual init_indi;

        while (trial < MNT)
        {
            trial ++;
            int *chromosome = rand_perm(req_edge_num+req_arc_num);
            memcpy(init_indi.Assignment, chromosome, sizeof(int)*(chromosome[0]+1));
            init_indi.TotalCost = split(init_indi.Sequence, init_indi.Assignment, inst_tasks);
            printf("%d %d \n", init_indi.TotalCost, get_task_seq_total_cost(init_indi.Sequence, inst_tasks));
            used = 0;
            for (i = 0; i < tmp_popsize; i++)
            {
                if (init_indi.TotalCost == pop[i].TotalCost)
                {
                    used = 1;
                    break;
                }
            }
            if (!used)
                break;
        }
        if (trial == MNT && used)
            break;

        pop[tmp_popsize] = init_indi;
        tmp_popsize++;
    }


}











