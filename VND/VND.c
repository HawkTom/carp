//
// Created by hao on 23/06/2020.
//

#include "VND.h"

/*
 * Representation: Sequence: 0 task1 task2 task3 0 task4 task5 task6 0 task7 task8 task9 0
 */
typedef struct lns_comb{
    int ids[MAX_ROUTE_TAG_LENGTH + 1];
} lns_comb;

void VND(const Task *inst_tasks)
{
    int i, j;

    // generate tour by Frederickson heuristic
    int FHRoute[250], iRoute[250];
    memset(iRoute, 0, sizeof(iRoute));
    iRoute[0] = req_edge_num + req_arc_num + 2;
    for (i = 2; i <= req_edge_num+req_arc_num+1; i++)
    {
        iRoute[i] = i-1;
    }
    FredericksonHeuristic(FHRoute, iRoute, inst_tasks);
    vnd_shorten(FHRoute, inst_tasks);

    Individual init_indi;
    AssignSubArray(FHRoute, 2, FHRoute[0]-1, init_indi.Assignment);
    init_indi.TotalCost = split(init_indi.Sequence, init_indi.Assignment, inst_tasks);

    int total_loads = 0;
    for (i = 1; i <= init_indi.Assignment[0]; i++)
    {
        total_loads += inst_tasks[init_indi.Assignment[i]].demand;
    }

    Individual s_best, curr_indi;
    curr_indi = init_indi;

    int c, f_best, max_route, nsize;
    while (1)
    {
        nsize = ceil(total_loads/capacity);
        s_best = curr_indi;
        while (1)
        {


            int Positions[50], Route[50][250];
            memset(Route, 0, sizeof(Route));
            find_ele_positions(Positions, curr_indi.Sequence, 0);

            Route[0][0] = Positions[0] - 1; // PA*
            for(i = 1; i < Positions[0]; i++)
            {
                AssignSubArray(curr_indi.Sequence, Positions[i], Positions[i+1], Route[i]);
            }


            int multi = Route[0][0];
            long int ub_trial = Route[0][0];
            for (i = 1; i < nsize; i++)
            {
                multi --;
                ub_trial *= multi;
            }

            multi = nsize;
            for (i = 1; i < nsize; i++)
            {
                ub_trial /= multi;
                multi --;
            }

            int maxcount = ub_trial;
            if (maxcount > M_NUM)
                maxcount = M_NUM;


            lns_comb cand_combs[M_NUM+1];

            int pointers[MAX_ROUTE_TAG_LENGTH + 1];
            for (i = 1; i <= nsize; i++)
            {
                pointers[i] = i;
            }
            int curr_ptr;
            for (i = 1; i <= maxcount; i++)
            {
                cand_combs[i].ids[0] = nsize;
                for (j = 1; j <= nsize; j++)
                {
                    cand_combs[i].ids[j] = pointers[j];
                }
                curr_ptr = nsize;

                while (pointers[curr_ptr] == Route[0][0] - nsize + curr_ptr)
                {
                    curr_ptr --;
                }
                if(curr_ptr == 0)
                    break;

                pointers[curr_ptr] ++;
                for (j = curr_ptr + 1; j <= nsize;  j++)
                {
                    pointers[j] = pointers[j-1] + 1;
                }
            }



            f_best = curr_indi.TotalCost;
            int new_seq1[MAX_TASK_TAG_LENGTH];
            int new_seq2[MAX_TASK_TAG_LENGTH];
            int best_seq[MAX_TASK_TAG_LENGTH];
            int old_cost, new_cost, flag;
            int best_choice = 0;
            for (c = 1; c <= maxcount; c++)
            {
//                int tmp_route[MAX_TASK_TAG_LENGTH];
//                tmp_route[0] = 1;
//                tmp_route[1] = 0;
//                int task_num = 0;
//                for (i = 1; i <= cand_combs[c].ids[0]; i++)
//                {
//                    tmp_route[0] --;
//                    JoinArray(tmp_route, Route[cand_combs[c].ids[i]]);
//                    for (j = 2; j < Route[cand_combs[c].ids[i]][0]; j++)
//                    {
//                        printf("%d \t", Route[cand_combs[c].ids[i]][j]);
//                        task_num ++;
//                    }
//                    task_num += Route[cand_combs[c].ids[i]][0] - 2;
//                }
//                printf("\n");

                while (1)
                {
                    memset(new_seq1, 0, sizeof(new_seq1));
                    memset(new_seq2, 0, sizeof(new_seq2));
                    flag = vnd_merge_switch(new_seq1, Route, cand_combs[c].ids, inst_tasks);
                    if (flag > 0)
                    {
                        old_cost = flag;
                    }
//                    if (new_seq1[0] != task_num)
//                    {
//                        printf("task error !\n");
//                        memset(new_seq1, 0, sizeof(new_seq1));
//                        old_cost = vnd_merge_switch(new_seq1, Route, cand_combs[c].ids, inst_tasks);
//                    }
                    new_cost = split(new_seq2, new_seq1, inst_tasks);
                    new_cost -= vnd_shorten(new_seq2, inst_tasks);

                    if (curr_indi.TotalCost - old_cost + new_cost < f_best)
                    {
                        f_best = curr_indi.TotalCost - old_cost + new_cost;
                        best_choice = c;
                        memcpy(best_seq, new_seq2, sizeof(new_seq2));
                    }

                    if (flag < 0)
                    {
                        break;
                    }
                }

            }

            if (f_best < curr_indi.TotalCost)
            {
                // set s = best_s;
                for (i = 1; i <= cand_combs[best_choice].ids[0]; i++)
                {
                    Route[cand_combs[best_choice].ids[i]][0] = 0;
                }
                curr_indi.Sequence[0] = 1;
                curr_indi.Sequence[1] = 0;
                for (i = 1; i <= Route[0][0]; i++)
                {
                    if (Route[i][0] != 0)
                    {
                        curr_indi.Sequence[0] --;
                        JoinArray(curr_indi.Sequence, Route[i]);
                    }
                }

                curr_indi.Sequence[0] --;
                JoinArray(curr_indi.Sequence, best_seq);
                curr_indi.TotalCost = f_best;

                if (get_task_seq_total_cost(curr_indi.Sequence, inst_tasks) != f_best)
                {
                    printf("error here \n");
                }

            } else {
                nsize --;
            }

            if (nsize == 0)
            {
                break;
            }
            break;
        }

        if(curr_indi.TotalCost >= s_best.TotalCost)
        {
            break;
        } else {
            s_best = curr_indi;
        }
        printf("best cost: %d \n", s_best.TotalCost);

    }
}