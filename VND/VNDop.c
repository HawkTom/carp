//
// Created by hao on 24/06/2020.
//

#include "VND.h"

int vnd_merge_switch(int *new_seq, int (*Route)[250], const int *choice, const Task *inst_tasks)
{

    int origin_cost = 0;
    int task_flag[5000];
    memset(task_flag, 0, sizeof(task_flag));

    // merge phase
    int i, j;

    int prev_node;
    int tour[5000];
    memset(tour, 0, sizeof(tour));
    tour[0] = 1;
    tour[1] = DEPOT;
    for (i = 1; i <= choice[0]; i++)
    {
        tour[0] --;
        prev_node = DEPOT;
        origin_cost += get_task_seq_total_cost(Route[choice[i]], inst_tasks);
        for (j = 2; j < Route[choice[i]][0]; j++)
        {
            JoinArray(tour, shortest_path[prev_node][inst_tasks[Route[choice[i]][j]].head_node]);
            prev_node = inst_tasks[Route[choice[i]][j]].tail_node;
            task_flag[tour[0]] = Route[choice[i]][j];
        }
        JoinArray(tour, shortest_path[prev_node][DEPOT]);
    }

    int NodeIdx[MAX_NODE_TAG_LENGTH][tour[0]];
    memset(NodeIdx, 0, sizeof(NodeIdx));
    NodeIdx[0][0] = 0;

    int max_times = 1, switch_node = 0;
    for (i = 1; i <= tour[0]; i++)
    {
        if (tour[i] != 0)
        {
            NodeIdx[tour[i]][0] ++;
            NodeIdx[tour[i]][NodeIdx[tour[i]][0]] = i;

            if(NodeIdx[tour[i]][0] > max_times)
            {
                max_times = NodeIdx[tour[i]][0];
                switch_node = tour[i];
            }
        }
    }

    int switch_cand[5000];
    memset(switch_cand, 0, sizeof(switch_cand));
    for (i = 1; i <= MAX_NODE_TAG_LENGTH; i++)
    {
        if (NodeIdx[i][0] > 2)
        {
            switch_cand[0]++;
            switch_cand[switch_cand[0]] = i;
        }
    }


    static int select = 0;

    select ++;
//    printf("%d \n", select);
//    int select = rand_choose(switch_cand[0]);
    switch_node = switch_cand[select];


    int switch_tour[MAX_NODE_TAG_LENGTH];
    int switch_flag[MAX_NODE_TAG_LENGTH];
    memset(switch_tour, 0, sizeof(switch_tour));
    memset(switch_flag, 0, sizeof(switch_flag));
    for (i = 1; i < NodeIdx[switch_node][0]; i++)
    {
        for (j = NodeIdx[switch_node][i+1]; j > NodeIdx[switch_node][i]; j--)
        {
            switch_tour[0] ++;
            switch_tour[switch_tour[0]] = tour[j];

            switch_flag[0] ++;
            switch_flag[switch_flag[0]] = inst_tasks[task_flag[j-1]].inverse;
        }
    }
    for (j = NodeIdx[switch_node][1]; j > 0; j--)
    {
        switch_tour[0]++;
        switch_tour[switch_tour[0]] = tour[j];
        switch_flag[0] ++;
        if ( j==1)
        {
            switch_flag[switch_flag[0]] = inst_tasks[task_flag[task_flag[0]-1]].inverse;
        } else {
            switch_flag[switch_flag[0]] = inst_tasks[task_flag[j-1]].inverse;
        }
    }

    for (j = tour[0]-1; j > NodeIdx[switch_node][NodeIdx[switch_node][0]]; j --)
    {
        switch_tour[0] ++;
        switch_tour[switch_tour[0]] = tour[j];
        switch_flag[0] ++;
        switch_flag[switch_flag[0]] = inst_tasks[task_flag[j-1]].inverse;
    }

    int start = 0;
    for (i = 1; i <= switch_tour[0]; i++)
    {
        if (switch_tour[i] == 1)
        {
            start = i;
            break;
        }
    }

    for (i = start; i <= switch_flag[0]; i++)
    {
        if (switch_flag[i]>0)
        {
            new_seq[0]++;
            new_seq[new_seq[0]] = switch_flag[i];
        }
    }
    for (i = 1; i < start; i++)
    {
        if (switch_flag[i]>0)
        {
            new_seq[0]++;
            new_seq[new_seq[0]] = switch_flag[i];
        }
    }
    if (select == 1)
    {
        return origin_cost;
    } else if (select == switch_cand[0])
    {
        select = 0;
        return -1;
    } else {
        return 0;
    }

}


void vnd_cut(int *split_task_seq, const int *one_task_seq, const Task *inst_tasks)
{
    int i, j;
    int total_loss = 0;

    int total_load = 0;
    for (i = 1; i <= one_task_seq[0]; i++)
    {
        total_load += inst_tasks[one_task_seq[i]].demand;
    }

    split_task_seq[0] = 0;

    int load, loss, actual_load;
    int cut_idx = 2, new_cut_idx = 1;
    int min_loss;

    while (1)
    {
        if (total_load < capacity)
            break;

        load = 0;
        for (i = cut_idx; i < one_task_seq[0]; i++)
        {
            load += inst_tasks[one_task_seq[i]].demand;
            if (load > capacity || total_load - load > capacity * ((double)ceil(1.0*total_load/capacity) - 1))
            {
                break;
            }
        }

        min_loss = INF;
        for(j = cut_idx; j < i; j++)
        {
            loss = min_cost[inst_tasks[one_task_seq[j]].tail_node][DEPOT] + min_cost[DEPOT][inst_tasks[one_task_seq[j+1]].head_node]
            - min_cost[inst_tasks[one_task_seq[j]].tail_node][inst_tasks[one_task_seq[j+1]].head_node];
            if (loss < min_loss)
            {
                min_loss = loss;
                new_cut_idx = j;
            }
        }

        total_loss += min_loss;
        split_task_seq[0] ++;
        split_task_seq[split_task_seq[0]] = 0;
        actual_load = 0;
        for (i = cut_idx; i <= new_cut_idx; i++)
        {
            split_task_seq[0] ++;
            split_task_seq[split_task_seq[0]] = one_task_seq[i];
            actual_load += inst_tasks[one_task_seq[i]].demand;
        }

        total_load = total_load - actual_load;
        cut_idx = new_cut_idx+1;
    }

    split_task_seq[0] ++;
    split_task_seq[split_task_seq[0]] = 0;
    for (i = cut_idx; i <= one_task_seq[0]; i++)
    {
        split_task_seq[0] ++;
        split_task_seq[split_task_seq[0]] = one_task_seq[i];
    }

    split_task_seq[0] ++;
    split_task_seq[split_task_seq[0]] = 0;
    if (get_task_seq_total_cost(split_task_seq, inst_tasks) != get_task_seq_total_cost(one_task_seq, inst_tasks)+total_loss)
    {
        printf("loss error!");
    }
}


int vnd_shorten( int *task_seq,  const Task *inst_tasks)
{
    // shorten operator is same as the OPT-2 (for single route) operator in LMA

    // task_seq [6 0 t1 t2 t3 t4 0]
//    int cost3 = get_task_seq_total_cost(task_seq, inst_tasks);
    int i, j, u, v;

    int Positions[50], Route[50][250];
    memset(Route, 0, sizeof(Route));
    find_ele_positions(Positions, task_seq, 0);
    Route[0][0] = Positions[0] - 1; // PA*
    for(i = 1; i < Positions[0]; i++)
    {
        AssignSubArray(task_seq, Positions[i], Positions[i+1], Route[i]);
    }

    int total_change_cost = 0;
    int change_cost;
    for (j = 1; j <= Route[0][0]; j++)
    {
        for (u = 1; u <= Route[j][0]-2; u ++)
        {
            for (v = u + 2; v <= Route[j][0]; v ++)
            {
                change_cost = min_cost[inst_tasks[Route[j][u]].tail_node][inst_tasks[Route[j][u+1]].head_node]
                              + min_cost[inst_tasks[Route[j][v-1]].tail_node][inst_tasks[Route[j][v]].head_node]
                              - min_cost[inst_tasks[Route[j][u]].tail_node][inst_tasks[Route[j][v-1]].tail_node]
                              - min_cost[inst_tasks[Route[j][u+1]].head_node][inst_tasks[Route[j][v]].head_node];


                if (change_cost > 0)
                {
                    for (i = u+1; i < v; i++)
                    {
                        Route[j][i] = inst_tasks[Route[j][i]].inverse;
                    }

                    int tmp, end;
                    end = v-1;
                    for (i = u+1; i <= (u+v)/2; i++)
                    {
                        tmp = Route[j][i];
                        Route[j][i] = Route[j][end];
                        Route[j][end] = tmp;
                        end --;
                    }
                    total_change_cost += change_cost;
                }
            }
        }
        int FHRoute[250],cost1, cost2;;
        FredericksonHeuristic(FHRoute, Route[j], inst_tasks);
        cost1 = get_task_seq_total_cost(FHRoute, inst_tasks);
        cost2 = get_task_seq_total_cost(Route[j], inst_tasks);
        if (cost1 < cost2)
        {
            AssignArray(FHRoute, Route[j]);
            total_change_cost += (cost2 - cost1);
        }

    }

    task_seq[0] = 1;
    task_seq[1] = 0;
    for (i = 1; i <= Route[0][0]; i++)
    {
        task_seq[0] --;
        JoinArray(task_seq, Route[i]);
    }


//    int cost4 = get_task_seq_total_cost(task_seq, inst_tasks);


    return total_change_cost;
}
















