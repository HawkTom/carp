/*
 TSA
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "functions.h"
#include "heutistic.h"
#include "TSA/TSA.h"
#include "MAENS/MAENS.h"


int req_arc_num = 0; //NRA
int req_edge_num; //NRE
int nonreq_arc_num = 0;
int nonreq_edge_num;
int vertex_num;
int vehicle_num;
int capacity;
int task_num;
int total_arc_num;
int DEPOT;
int trav_cost[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];
int serve_cost[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];
int min_cost[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];
int shortest_path[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];

int cost_backup[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];



void ShowMatrix(int (*Min)[MAX_NODE_TAG_LENGTH], int dim);

int main()
{

    Task inst_tasks[MAX_TASKS_TAG_LENGTH];
    Arc inst_arcs[MAX_ARCS_TAG_LENGTH];

    readMap(inst_tasks, inst_arcs);


    for (int i=1; i<=vertex_num; i++)
    {
        for (int j=1; j<=vertex_num; j++)
        {
            trav_cost[i][j] = INF;
            serve_cost[i][j] = 0;
        }
    }

    trav_cost[1][0] = 0;
    trav_cost[0][1] = 0;

    for (int i=1; i<=task_num; i++)
    {
        serve_cost[inst_tasks[i].head_node][inst_tasks[i].tail_node] = inst_tasks[i].serv_cost;
    }

    for (int i=1; i<=total_arc_num; i++)
    {
        trav_cost[inst_arcs[i].head_node][inst_arcs[i].tail_node] = inst_arcs[i].trav_cost;
        cost_backup[inst_arcs[i].head_node][inst_arcs[i].tail_node] = inst_arcs[i].trav_cost;
    }

    mod_dijkstra();
//    ShowMatrix(min_cost, vertex_num);
    MAENS(inst_tasks);



//    augment_merge(&InitSolution, inst_tasks);
//    path_scanning(&InitSolution, inst_tasks, serve_mark);

    printf("Hello, World!\n");
    return 0;
}

void ShowMatrix(int (*Min)[MAX_NODE_TAG_LENGTH], int dim)
{
    for (int i=0; i<=dim; i++)
    {
        for (int j=0; j<=dim; j++)
        {
            printf("%d \t", Min[i][j]);
        }
        printf("\n");
    }
}
