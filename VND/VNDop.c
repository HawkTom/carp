//
// Created by hao on 24/06/2020.
//

#include "VND.h"
void vnd_swap_seq(int *Array, int k);

void vnd_swap_seq(int *Array, int k)
{
    // process for circle data
    int tmp[Array[0]+1];
    memcpy(tmp, Array, sizeof(int)*(Array[0]+1));
    for (int i=1; i <= Array[0]; i++)
    {
        if (i + k - 1 <= Array[0])
        {
            Array[i] = tmp[i+k-1];
        } else {
            Array[i] = tmp[Array[0]-i-k+2];
        }
    }
}

void vnd_merge_switch(int *new_task_seq, int *Sequence, const int *choice, const Task *inst_tasks)
{
    int mark_task[250][250];

    // merge phase
    int i, j;
    int Positions[50], Route[50][250];
    find_ele_positions(Positions, Sequence, 0);

    Route[0][0] = Positions[0] - 1; // PA*
    for(i = 1; i < Positions[0]; i++)
    {
        AssignSubArray(Sequence, Positions[i], Positions[i+1], Route[i]);
    }

    int prev_node;
    int tour[5000];
    tour[0] = 1;
    tour[1] = DEPOT;
    for (i = 1; i <= choice[0]; i++)
    {
        tour[0] --;
        prev_node = DEPOT;
        for (j = 2; j < Route[choice[i]][0]; j++)
        {
            JoinArray(tour, shortest_path[prev_node][inst_tasks[Route[choice[i]][j]].head_node]);
            prev_node = inst_tasks[Route[choice[i]][j]].tail_node;
            mark_task[tour[0]][tour[0]+1] = 1;
            mark_task[tour[0]+1][tour[0]] = 1;
        }
        JoinArray(tour, shortest_path[prev_node][DEPOT]);
    }

    // switch phase
    // tour: 1 x x x x x 1 x x x x x 1 x x x x x 1
    int NodeIdx[MAX_NODE_TAG_LENGTH][tour[0]];
    memset(NodeIdx, 0, sizeof(NodeIdx));
    NodeIdx[0][0] = 0;

    int mapping[tour[0]+1];
    mapping[0] = tour[0];
    int max_times = 1, switch_node = 0;
    for (i = 1; i <= tour[0]; i++)
    {
        mapping[i] = i;
        if (tour[i] != DEPOT)
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

    if (max_times < 2)
        return;

    int mapping_node = NodeIdx[switch_node][1];
    for (i = 1; i <= NodeIdx[switch_node][0]; i++)
    {
        mapping[i] = mapping_node;
        NodeIdx[switch_node][i] -= mapping_node - 1;
    }

    mapping[mapping[0]] = 1;
    vnd_swap_seq(mapping, mapping_node);

    NodeIdx[switch_node][0] ++;
    NodeIdx[switch_node][NodeIdx[switch_node][0]] = mapping[0];

    int switch_tour[MAX_NODE_TAG_LENGTH];
    memset(switch_tour, 0, sizeof(switch_tour));
    for (i = 1; i < NodeIdx[switch_node][0]; i++)
    {
        for (j = NodeIdx[switch_node][i+1]; j > NodeIdx[switch_node][i]; j--)
        {
            switch_tour[0] ++;
            switch_tour[switch_tour[0]] = mapping[j];
        }
    }
    switch_tour[0] ++;
    switch_tour[switch_tour[0]] = mapping_node;

    for (i = 1; i <= switch_tour[0]; i++)
    {
        if (tour[switch_tour[i]] == DEPOT)
        {
            break;
        }
    }
    vnd_swap_seq(switch_tour, i);

    int new_tour[5000];
    new_tour[0] = 0;
    new_task_seq[0] = 0;
    for (i = 1; i < switch_tour[0]; i++)
    {
        if (mark_task[switch_tour[i]][switch_tour[i+1]])
        {
            new_task_seq[0] ++;
            new_task_seq[new_task_seq[0]] = FindTask(switch_tour[i], switch_tour[i+1], inst_tasks, req_edge_num*2);
        }
        new_tour[0]++;
        new_tour[new_tour[0]] = tour[switch_tour[i]];
    }

}


void vnd_merge(int *tour, int *Sequence, const int *choice, const Task *inst_tasks)
{
    int i, j;
    int Positions[50], Route[50][250];
    find_ele_positions(Positions, Sequence, 0);

    Route[0][0] = Positions[0] - 1; // PA*
    for(i = 1; i < Positions[0]; i++)
    {
        AssignSubArray(Sequence, Positions[i], Positions[i+1], Route[i]);
    }

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




void vnd_switch(int *new_tour, const int *tour)
{
    // tour: 0 x x x x x 0 x x x x x 0 x x x x x 0
    int i, j;
    int NodeIdx[MAX_NODE_TAG_LENGTH][tour[0]];
    memset(NodeIdx, 0, sizeof(NodeIdx));
    NodeIdx[0][0] = 0;

    int mapping[tour[0]+1];
    mapping[0] = tour[0];
    int max_times = 1, switch_node = 0;
    for (i = 1; i <= tour[0]; i++)
    {
        mapping[i] = i;
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

    if (max_times < 2)
        return;

    int mapping_node = NodeIdx[switch_node][1];
    for (i = 1; i <= NodeIdx[switch_node][0]; i++)
    {
        mapping[i] = mapping_node;
        NodeIdx[switch_node][i] -= mapping_node - 1;
    }

    mapping[mapping[0]] = 1;
    vnd_swap_seq(mapping, mapping_node);

    NodeIdx[switch_node][0] ++;
    NodeIdx[switch_node][NodeIdx[switch_node][0]] = mapping[0];

    int switch_tour[MAX_NODE_TAG_LENGTH];
    memset(switch_tour, 0, sizeof(switch_tour));
    for (i = 1; i < NodeIdx[switch_node][0]; i++)
    {
        for (j = NodeIdx[switch_node][i+1]; j > NodeIdx[switch_node][i]; j--)
        {
            switch_tour[0] ++;
            switch_tour[switch_tour[0]] = mapping[j];
        }
    }
    switch_tour[0] ++;
    switch_tour[switch_tour[0]] = mapping_node;

    for (i = 1; i <= switch_tour[0]; i++)
    {
        if (tour[switch_tour[i]] == 0)
        {
            break;
        }
    }
    vnd_swap_seq(switch_tour, i);

    new_tour[0] = 0;
    for (i = 1; i <= switch_tour[0]; i++)
    {
        new_tour[0]++;
        new_tour[new_tour[0]] = tour[switch_tour[i]];
    }



}

void vnd_switch1(int *new_tour,  const int *tour)
{
    // tour: 0 x x x x 0 x x x x 0
    int i, j;
//    printf("Test vnd_switch firstly! \n");
//    exit(0);
    // 考虑一个环 来switch
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

    int switch_tour[MAX_NODE_TAG_LENGTH];
    memset(switch_tour, 0, sizeof(switch_tour));
    for (i = 1; i < NodeIdx[switch_node][0]; i++)
    {
        for (j = NodeIdx[switch_node][i+1]; j > NodeIdx[switch_node][i]; j--)
        {
            switch_tour[0] ++;
            switch_tour[switch_tour[0]] = tour[j];
        }
    }

    for (j = NodeIdx[switch_node][1]; j > 0; j--)
    {
        switch_tour[0]++;
        switch_tour[switch_tour[0]] = tour[j];
    }

    for (j = tour[0]-1; j > NodeIdx[switch_node][i]; j --)
    {
        switch_tour[0] ++;
        switch_tour[switch_tour[0]] = tour[j];
    }

//    int new_tour[2*tour[0]],
//    memset(new_tour, 0, sizeof(new_tour));

    int tmp_tour[tour[0]];
    memset(tmp_tour, 0, sizeof(tmp_tour));
    int start = 0;
    for (i = 1; i <= switch_tour[0]; i++)
    {
        if (switch_tour[i] == 0)
        {
            start = i;
            break;
        }
    }
    AssignSubArray(switch_tour, 1, start, tmp_tour);
    AssignSubArray(switch_tour, start, switch_tour[0], new_tour);
    JoinArray(new_tour, tmp_tour);

}


void vnd_cut(int *split_task_seq, const int *one_task_seq, const Task *inst_tasks)
{
    int i, j;

    printf("Test vnd_cut firstly! \n");
    exit(0);

    int total_load = 0;
    for (i = 1; i <= one_task_seq[0]; i++)
    {
        total_load += inst_tasks[one_task_seq[i]].demand;
    }

    split_task_seq[0] = 0;

    int load, loss;
    int cut_idx = 1, new_cut_idx = 1;
    int min_loss;

    while (1)
    {
        if (total_load < capacity)
            break;

        load = 0;
        for (i = cut_idx; i <= one_task_seq[0]; i++)
        {
            load += inst_tasks[one_task_seq[i]].demand;
            if (load > capacity || total_load - load > capacity * (ceil(1.0*total_load/capacity) - 1))
            {
                break;
            }
        }

        min_loss = INF;
        for(j = cut_idx; j <= i; j++)
        {
            loss = min_cost[inst_tasks[one_task_seq[j]].tail_node][DEPOT] + min_cost[DEPOT][inst_tasks[one_task_seq[j+1]].head_node]
            - min_cost[inst_tasks[one_task_seq[j]].tail_node][inst_tasks[one_task_seq[j+1]].head_node];
            if (loss < min_loss)
            {
                min_loss = loss;
                new_cut_idx = j;
            }
        }

        split_task_seq[0] ++;
        split_task_seq[split_task_seq[0]] = 0;
        for (i = cut_idx; i <= new_cut_idx; i++)
        {
            split_task_seq[0] ++;
            split_task_seq[split_task_seq[0]] = one_task_seq[i];
        }

        total_load = total_load - load;
        cut_idx = new_cut_idx;
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
}


void vnd_shorten( int *task_seq,  const Task *inst_tasks)
{
    // replace shorten by Frederickson heuristic in VND
    // shorten operator is same as the OPT-2 (for single route) operator in LMA

    // task_seq [6 0 t1 t2 t3 t4 0]
    int i, u, v;

    int change_cost;
    int cost1, cost2;
    for (u = 1; u <= task_seq[0]-2; u ++)
    {
        for (v = u + 2; v <= task_seq[0]; v ++)
        {
            change_cost = min_cost[inst_tasks[task_seq[u]].tail_node][inst_tasks[task_seq[u+1]].head_node]
                    + min_cost[inst_tasks[task_seq[v-1]].tail_node][inst_tasks[task_seq[v]].head_node]
                    - min_cost[inst_tasks[task_seq[u]].tail_node][inst_tasks[task_seq[v-1]].tail_node]
                    - min_cost[inst_tasks[task_seq[u+1]].head_node][inst_tasks[task_seq[v]].head_node];


            if (change_cost > 0)
            {
//                cost1 = get_task_seq_total_cost(task_seq, inst_tasks);
                for (i = u+1; i < v; i++)
                {
                    task_seq[i] = inst_tasks[task_seq[i]].inverse;
                }
                int tmp, end;
                end = v-1;
                for (i = u+1; i <= (u+v)/2; i++)
                {
                    tmp = task_seq[i];
                    task_seq[i] = task_seq[end];
                    task_seq[end] = tmp;
                    end --;
                }
//                for (i = u; i <= v; i++)
//                {
//                    printf("%d \t", task_seq[i]);
//                }
//                printf("\n");
//                cost2 = get_task_seq_total_cost(task_seq, inst_tasks);
//                if (cost2+change_cost != cost1)
//                {
//                    printf("cost error\n");
//                }
            }
        }
    }
}
















