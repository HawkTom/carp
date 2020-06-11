//
// Created by hao on 11/06/2020.
//

#include "functions.h"

/*
 * stop[0]: number of outside vehicles
 * require edge number (req_edge_num): number of tasks.
 * Array[0] == req_edge_num
 */
// require to be tested
int greedy_split(int *split_task_seq, const int *Array, int *stop, int *remain_capacity, const Task *inst_tasks)
{

    stop[0] ++;
    stop[stop[0]] = DEPOT;
    remain_capacity[0] ++;
    remain_capacity[remain_capacity[0]] = capacity;

    int i, j;
    int used[stop[0]];
    int next_node, tmp_reach, tmp_veh, tmp_cost;
    int cost, load, acd;
    int Length = Array[0];
    int Position[50], assign_veh[50];
    int route_cost[50], route_num;

    route_num = 0;
    next_node = 1;


    while (1)
    {
        if (next_node > Length)
            break;

        route_num ++;
        Position[route_num] = next_node;

        int min_acd = INF;
        for (i = 1; i <= stop[0]; i++)
        {
            if (used[i])
                continue;

            cost = 0;
            load = 0;
            j = next_node;
            while ( j <= Length )
            {
              if ( load + inst_tasks[Array[j]].demand > remain_capacity[i])
                  break;

              load += inst_tasks[Array[j]].demand;
              if ( j == next_node )
              {
                  cost = min_cost[stop[i]][inst_tasks[Array[j]].head_node] + inst_tasks[Array[j]].serv_cost + min_cost[inst_tasks[Array[j]].tail_node][DEPOT];
              } else {
                  cost += min_cost[inst_tasks[Array[j-1]].tail_node][inst_tasks[Array[j]].head_node] \
                        + inst_tasks[Array[j]].serv_cost + min_cost[inst_tasks[Array[j]].tail_node][DEPOT] \
                        - min_cost[inst_tasks[Array[j-1]].tail_node][DEPOT];
              }
              acd =  cost / (float) load;
              if (acd < min_acd)
              {
                  min_acd = acd;
                  tmp_reach = j;
                  tmp_veh = i;
                  tmp_cost = cost;
              }
              j ++;
            }
        }
        next_node = tmp_reach + 1;
        if(tmp_veh != stop[0])
            used[tmp_veh] = 1;

        assign_veh[route_num] = tmp_veh;
        route_cost[route_num] = tmp_cost;

    }
    Position[0] = route_num;
    Position[0] ++;
    Position[Position[0]] = next_node;

    assign_veh[0] = route_num;
    route_cost[0] = route_num;


    int total_cost = 0;
    int v_capacity[route_num + 1];
    int v_stop[route_num + 1];
    split_task_seq[0] = 1;
    split_task_seq[1] = 0;
    v_stop[0] = route_num;
    v_capacity[0] = route_num;

    // split and calculate total cost
    for (i = 1; i <= route_num; i++)
    {

        for (j = Position[i]; j < Position[i+1]; j++)
        {
            split_task_seq[0] ++;
            split_task_seq[split_task_seq[0]] = Array[j];
        }
        split_task_seq[0] ++;
        split_task_seq[split_task_seq[0]] = 0;

        v_stop[i] = assign_veh[i];
        v_capacity[i] = remain_capacity[v_stop[i]];
        total_cost += route_cost[i];
    }

    // repair for unused outside vehicle
    for (i = 1; i < stop[0]; i++)
    {
        if ( !used[i])
        {
            total_cost += min_cost[stop[i]][DEPOT];
            split_task_seq[0] += 2;
            v_stop[0] ++;
            v_stop[v_stop[0]] = stop[i];
            v_capacity[0] ++;
            v_capacity[v_capacity[0]] = remain_capacity[stop[i]];
        }
    }

    // new stop: v_stop, new_remain_capacity: v_capacity
    return total_cost;

}
