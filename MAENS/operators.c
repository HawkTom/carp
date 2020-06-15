//
// Created by hao on 15/06/2020.
//

#include "MAENS.h"

void rand_scanning(Individual *rs_indi, const Task *inst_tasks, const int *serve_mark)
{
    int i, k;
    int serve_task_num = 0; // the number of tasks required to be served.
    for (i = req_edge_num+1; i <= task_num; i++)
    {
        if(serve_mark[i])
            serve_task_num++;
    }

    int load, trial, mindist;

    int unserved_task[MAX_TASK_TAG_LENGTH], candi_task[MAX_TASK_TAG_LENGTH], nearest_task[MAX_TASK_TAG_LENGTH];
    int current_task, next_task;

    int positions[MAX_TASK_SEG_LENGTH];

    rs_indi->Sequence[0] = 1;
    rs_indi->Sequence[1] = 0;
    rs_indi->Loads[0] = 0;

    unserved_task[0] = 0;

    for (i = 1; i <= task_num; i++)
    {
        if ( !serve_mark[i] )
            continue;

        unserved_task[0] ++;
        unserved_task[unserved_task[0]] = i;
    }

    load = 0;
    trial = 0;
    while (trial < serve_task_num)
    {
        current_task = rs_indi->Sequence[rs_indi->Sequence[0]];

        candi_task[0] = 0;
        for( i = 1; i <= unserved_task[0]; i++)
        {
            if( inst_tasks[unserved_task[i]].demand <= capacity - load )
            {
                candi_task[0] ++;
                candi_task[candi_task[0]] = unserved_task[i];
            }
        }

        if (candi_task[0] == 0)
        {
            rs_indi->Sequence[0] ++;
            rs_indi->Sequence[rs_indi->Sequence[0]] = 0;
            rs_indi->Loads[0] ++;
            rs_indi->Loads[rs_indi->Loads[0]] = load;
            load = 0;
            continue;
        }

        mindist = INF;
        nearest_task[0] = 0;

        for (i = 1; i <= candi_task[0]; i++)
        {
            if (min_cost[inst_tasks[current_task].tail_node][inst_tasks[candi_task[i]].head_node] < mindist)
            {
                mindist = min_cost[inst_tasks[current_task].tail_node][inst_tasks[candi_task[i]].head_node];
                nearest_task[0] = 1;
                nearest_task[nearest_task[0]] = candi_task[i];
            }else if(min_cost[inst_tasks[current_task].tail_node][inst_tasks[candi_task[i]].head_node] == mindist)
            {
                nearest_task[0] ++;
                nearest_task[nearest_task[0]] = candi_task[i];
            }
        }

        k = rand_choose(nearest_task[0]);
        next_task = nearest_task[k];

        trial ++;
        rs_indi->Sequence[0] ++;
        rs_indi->Sequence[rs_indi->Sequence[0]] = next_task;
        load += inst_tasks[next_task].demand;
        find_ele_positions(positions, unserved_task, next_task);
        delete_element(unserved_task, positions[1]);

        if (inst_tasks[next_task].inverse > 0)
        {
            find_ele_positions(positions, unserved_task, inst_tasks[next_task].inverse);
            delete_element(unserved_task, positions[1]);
        }
    }

    rs_indi->Sequence[0] ++;
    rs_indi->Sequence[rs_indi->Sequence[0]] = 0;
    rs_indi->Loads[0] ++;
    rs_indi->Loads[rs_indi->Loads[0]] = load;

    rs_indi->TotalCost = get_task_seq_total_cost(rs_indi->Sequence, inst_tasks);
    rs_indi->TotalVioLoad = get_total_vio_load(rs_indi->Loads);

}


void indi_copy(Individual *target, Individual *source)
{
    memcpy(target->Sequence, source->Sequence, (source->Sequence[0]+1)*sizeof(int));
    memcpy(target->Loads, source->Loads, (source->Loads[0]+1)*sizeof(int));
    target->TotalCost = source->TotalCost;
    target->TotalVioLoad = source->TotalVioLoad;
    target->Fitness = source->Fitness;
//    memcpy(target->pred, source->pred, sizeof(source->pred));
//    memcpy(target->succ, source->succ, sizeof(source->succ));
}
