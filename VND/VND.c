//
// Created by hao on 23/06/2020.
//

#include "VND.h"

/*
 * Representation: Sequence: 0 task1 task2 task3 0 task4 task5 task6 0 task7 task8 task9 0
 */

void vnd(const Task *inst_tasks)
{
    int i;

    // generate tour by Frederickson heuristic
    int FHRoute[250], Route[250];
    memset(Route, 0, sizeof(Route));
    Route[0] = req_edge_num + req_arc_num + 2;
    for (i = 2; i <= req_edge_num+req_arc_num+1; i++)
    {
        Route[i] = i-1;
    }
    FredericksonHeuristic(FHRoute, Route, inst_tasks);

    // Sequence

}