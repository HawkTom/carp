//
// Created by hao on 24/06/2020.
//

#include "VND.h"

void vnd_merge(int *tour, int (*Route)[250], const int *choice, const Task *inst_tasks)
{
    int i, j;
    int prev_node;
    tour[0] = 0;
    for (i = 1; i <= choice[0]; i++)
    {
        prev_node = 0;
        for (j = 2; j < Route[choice[i]][0]; j++)
        {
            JoinArray(tour, shortest_path[prev_node][inst_tasks[Route[choice[i]][j]].head_node]);
            prev_node = inst_tasks[Route[choice[i]][j]].tail_node;
        }
    }
}


void vnd_switch(int *tour)
{
    // 考虑一个环 来switch
}


void vnd_cut(int *tour)
{
    // 有点像split
}

void vnd_shorten( int *tour,  const Task *inst_tasks)
{
    int i, j, k;
    // find the path for the given solution, start from the depot

    int mark_task[250][250];

}
