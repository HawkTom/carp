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

void SBX(Individual *xed_child, Individual *p1, Individual *p2, const Task *inst_tasks)
// the SBX crossover operator
{
    /*
     * P1 -> R1, P2 -> R2
     * R1 -> {R11, R12}, R2 -> {R21, R22}
     * NewR = {R11, R22}
     * remove duplicate task in NewR and insert missing task to NewR -> offspring
     */

    int i, j, k;
    int SubPath1[MAX_TASK_SEQ_LENGTH], SubPath2[MAX_TASK_SEQ_LENGTH];
    int Routes1[50][MAX_TASK_SEQ_LENGTH], Routes2[50][MAX_TASK_SEQ_LENGTH];
    int XCLds[50], LeftTasks[MAX_TASK_SEQ_LENGTH], Positions[50];

    struct CandSelection
    {
        int RouteID;
        int Pos;
    };

    // CandSLCTList record which routes and the postion of each task located
    struct CandSelection CandSLCTList1[MAX_TASK_SEQ_LENGTH], CandSLCTList2[MAX_TASK_SEQ_LENGTH];
    int LLength1 = 0, LLength2 = 0;

    find_ele_positions(Positions, p1->Sequence, 0);
    Routes1[0][0] = Positions[0] - 1;
    for (i=1; i < Positions[0]; i++)
    {
        AssignSubArray(p1->Sequence, Positions[i], Positions[i+1], Routes1[i]);
    }

    find_ele_positions(Positions, p2->Sequence, 0);
    Routes2[0][0] = Positions[0] - 1;
    for (i=1; i < Positions[0]; i++)
    {
        AssignSubArray(p2->Sequence, Positions[i], Positions[i+1], Routes2[i]);
    }

    memcpy(XCLds, p1->Loads, sizeof(p1->Loads));

    for (i=1; i <= Routes1[0][0]; i++)
    {
        for (j = 2; j < Routes1[i][0]; j++)
        {
            LLength1 ++;
            CandSLCTList1[LLength1].RouteID = i;
            CandSLCTList1[LLength1].Pos = j;
        }
    }

    for (i=1; i <= Routes2[0][0]; i++)
    {
        for (j = 2; j < Routes2[i][0]; j++)
        {
            LLength2 ++;
            CandSLCTList2[LLength2].RouteID = i;
            CandSLCTList2[LLength2].Pos = j;
        }
    }

    int k1 = rand_choose(LLength1);
    int k2 = rand_choose(LLength2);

    AssignSubArray(Routes1[CandSLCTList1[k1].RouteID], 1, CandSLCTList1[k1].Pos, SubPath1);
    AssignSubArray(Routes2[CandSLCTList2[k2].RouteID], CandSLCTList2[k2].Pos,Routes2[CandSLCTList2[k2].RouteID][0], SubPath2);
    AssignSubArray(Routes1[CandSLCTList1[k1].RouteID], CandSLCTList1[k1].Pos+1,Routes1[CandSLCTList1[k1].RouteID][0]-1, LeftTasks);

    // remove duplicated tasks for Route1
    int Checked[MAX_TASK_SEQ_LENGTH];
    memset(Checked, 0, sizeof(Checked));

    for (i = 1; i < SubPath2[0]; i++)
    {
        if (Checked[i])
            continue;

        for (j = SubPath1[0]; j > 1; j--)
        {
            if (SubPath1[j] == SubPath2[i] || SubPath1[j] == inst_tasks[SubPath2[i]].inverse)
            {
                delete_element(SubPath1, j);
                Checked[i] = 1;
                break;
            }
        }
    }

    for (i = 1; i < SubPath2[0]; i++)
    {
        if (Checked[i])
            continue;

        for (j = LeftTasks[0]; j > 0; j--)
        {
            if (LeftTasks[j] == SubPath2[i] || LeftTasks[j] == inst_tasks[SubPath2[i]].inverse)
            {
                delete_element(LeftTasks, j);
                Checked[i] = 1;
                break;
            }
        }
    }

    for (i = 1; i <= SubPath2[0]; i ++)
    {
        if (Checked[i])
            continue;

        for (j=1; j <= Routes1[0][0]; j++)
        {
            if (j == CandSLCTList1[k1].RouteID)
                continue;

            for (k = Routes1[j][0]; k > 1; k --)
            {
                if (Routes1[j][k] == SubPath2[i] || Routes1[j][k] == inst_tasks[SubPath2[i]].inverse)
                {
                    delete_element(Routes1[j], k);
                    XCLds[j] -= inst_tasks[SubPath2[i]].demand;
                    Checked[i] = 1;
                }
            }
            if (Checked[i])
                break;
        }
    }

    JoinArray(SubPath1, SubPath2);
    memcpy(Routes1[CandSLCTList1[k1].RouteID], SubPath1, sizeof(SubPath1));
    XCLds[CandSLCTList1[k1].RouteID] = 0;
    for (i = 2; i < Routes1[CandSLCTList1[k1].RouteID][0]; i++)
    {
        XCLds[CandSLCTList1[k1].RouteID] += inst_tasks[Routes1[CandSLCTList1[k1].RouteID][i]].demand;
    }

    int NO_LeftTasks = LeftTasks[0];

    // insert missing tasks




}

void rand_selection(int *id1, int *id2, int popsize)
/* pop has been sorted increasingly already */
{
int k1, k2;
int candi[MAX_POPSIZE+1];
candi[0] = popsize;
for (int i = 1; i <= popsize; i++)
{
candi[i] = i-1;
}

k1 = rand_choose(candi[0]);
*id1 = candi[k1];
delete_element(candi, k1);
k2 = rand_choose(candi[0]);
*id2 = candi[k2];
//printf("id1 = %d, id2 = %d, popsize = %d\n", id1, id2, popsize);
}