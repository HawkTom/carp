//
// Created by hao on 01/06/2020.
//

#ifndef CARP_FUNCTIONS_H
#define CARP_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_TASKS_TAG_LENGTH 1001
#define MAX_ARCS_TAG_LENGTH 1001
#define MAX_NODE_TAG_LENGTH 300
#define INF 1000000000

#define MAX_TASK_SEG_LENGTH 550
#define MAX_ROUTE_TAG_LENGTH 50

#define MAX_TASK_TAG_LENGTH 381
#define MAX_TASK_SEQ_LENGTH 250

extern int req_arc_num;
extern int nonreq_arc_num;
extern int vertex_num;
extern int req_edge_num;
extern int nonreq_edge_num;
extern int vehicle_num;
extern int capacity;
extern int task_num;
extern int total_arc_num;
extern int DEPOT;
extern int trav_cost[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];
extern int serve_cost[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];

extern int min_cost[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];
extern int shortest_path[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];

extern int cost_backup[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];
extern int costlb;
extern int costub;
extern int demandlb;
extern int demandub;


typedef struct task
{
    int head_node;
    int tail_node;
    int dead_cost;
    int serv_cost;
    int demand;
    int inverse;
} Task;

// format: head ----> tail
typedef struct arc
{
    int head_node;
    int tail_node;
    int trav_cost;
    unsigned int change;
    int link;
} Arc;



typedef struct individual
{
    int Sequence[250]; // task id with depot inside
    int Assignment[250];
    int TotalCost;
    int Loads[50];
    int TotalVioLoad;
    double Fitness;
//    int start[50]; // for DCARP
} Individual;

void mod_dijkstra();

void readMap(Task *inst_tasks, Arc *inst_arcs);

void nextScenario(Individual *Solution, const Task *inst_tasks, const Arc *inst_arcs, unsigned int seed);



int max(int *Array);

void find_ele_positions(int *positions, int *a, int e);

void delete_element(int *a, int k);

void add_element(int *a, int e, int k);

int get_task_seq_total_cost(int *task_seq, const Task *inst_tasks);

int get_total_vio_load(int *route_seg_load);

int FindTask(int a, int b, const Task *ARPTask, int NO_Task);

void AssignArray(int *Array1, int *Array2);

void AssignSubArray(int *Array1, int k1, int k2, int *Array2);

void JoinArray(int *JointArray, int *Array);

int RandChoose(int n);

void ReverseDirection(int *Array, int k1, int k2);



#endif //CARP_FUNCTIONS_H
