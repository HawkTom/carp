//
// Created by hao on 15/06/2020.
//

#include "LMA.h"

#define MNT 50
#define MNRS 20
#define MNREP 8
#define UB 50000

void LMA(const Task *inst_tasks)
{
    int i, j, k;
    int popsize = 30;
    Individual pop[40];

    short used[UB];
    // initial used vector;
    memset(used, 0, sizeof(used));


    // path-scanning
    Individual tmp_indi1;
    int ServeMark[2*req_edge_num + req_arc_num + 1];
    memset(ServeMark, 1, sizeof(ServeMark));
    path_scanning(&tmp_indi1, inst_tasks, ServeMark);
    evaluate_route(&tmp_indi1, inst_tasks);
    pop[0] = tmp_indi1;
    used[tmp_indi1.TotalCost] = 1;

    // augment-merge
    Individual tmp_indi2;
    augment_merge(&tmp_indi2, inst_tasks);
    evaluate_route(&tmp_indi2, inst_tasks);
    pop[1] = tmp_indi2;
    used[tmp_indi2.TotalCost] = 1;

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
    pop[2] = tmp_indi3;
    used[tmp_indi3.TotalCost] = 1;



    int tmp_popsize = 3;
    while (tmp_popsize < popsize)
    {
        int trial = 0;
        Individual init_indi;

        while (trial < MNT)
        {
            trial ++;
            int *chromosome = rand_perm(req_edge_num+req_arc_num);
            memcpy(init_indi.Assignment, chromosome, sizeof(int)*(chromosome[0]+1));
            init_indi.TotalCost = split(init_indi.Sequence, init_indi.Assignment, inst_tasks);
            if (used[init_indi.TotalCost])
                break;
        }
        if (trial == MNT && used[init_indi.TotalCost])
            break;

        pop[tmp_popsize] = init_indi;
        used[init_indi.TotalCost] = 1;
        tmp_popsize++;
    }
    popsize = tmp_popsize;
    sort_pop(pop, popsize);

//    printf("init: %d \n", pop[0].TotalCost);
    Individual parent1, parent2, xed_child, mted_child, child;

    int restarts = 0;
    int mnpi = 20000, mnwi = 6000;
    double Pls = 0.1;

    while (restarts < MNRS)
    {
        // main search
        int npi = 0, nwi = 0;
        while (npi != mnpi && nwi != mnwi)
        {

            // randomly select two parents
            int par_id1, par_id2;
            rand_selection(&par_id1, &par_id2, popsize);
            if (pop[par_id1].TotalCost < pop[par_id2].TotalCost)
            {
                parent1 = pop[par_id1];
                parent2 = pop[par_id2];
            } else {
                parent2 = pop[par_id1];
                parent1 = pop[par_id2];
            }

            OX(&xed_child, &parent1, &parent2, inst_tasks);
            xed_child.TotalCost = split(xed_child.Sequence, xed_child.Assignment, inst_tasks);

            child = xed_child;

            k = rand_choose(popsize - popsize/2 + 1) + popsize/2  - 1;
            if (k > popsize)
            {
                printf("select k error %d, %d \n", k, popsize);
                exit(0);
            }

            double random = 1.0 * rand() / RAND_MAX;
            if (random < Pls)
            {
                // Do the local search
                lma_lns(&child, &mted_child, inst_tasks);
                evaluate_route(&mted_child, inst_tasks);
                if (!used[mted_child.TotalCost] || mted_child.TotalCost == pop[k-1].TotalCost)
                {
                    child = mted_child;
                }
            }


            if ( !used[child.TotalCost] || child.TotalCost == pop[k - 1].TotalCost)
            {
                npi ++;
                if (child.TotalCost < pop[0].TotalCost)
                {
                    nwi = 0;
                } else {
                    nwi ++;
                }
                used[pop[k-1].TotalCost] = 0;
                used[child.TotalCost] = 1;
                pop[k-1] = child;
                //resort pop;
                sort_pop(pop, popsize);
            }
//            break;
        }


        mnpi = 2000;
        mnwi = 2000;
        Pls = 0.2;
        restarts ++;

        // partial replacement
        partial_replacement(pop, popsize, MNREP, inst_tasks);

        printf("one iteration: %d\n", pop[0].TotalCost);
    }

}











