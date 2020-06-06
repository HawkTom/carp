/*
 TSA
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "functions.h"
#include "heutistic.h"


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

void ShowMatrix(int (*Min)[MAX_NODE_TAG_LENGTH], int dim);

int main()
{
    FILE *fp;

    char dummy[50];


    Task inst_tasks[MAX_TASKS_TAG_LENGTH];
    Arc inst_arcs[MAX_ARCS_TAG_LENGTH];

    fp = fopen("../egl-e1-C.dat", "r");

    while (fscanf(fp, "%s", dummy) != EOF)
    {

        if (strcmp(dummy, "VERTICES")==0)
        {
            fscanf(fp, "%s", dummy);
            fscanf(fp, "%d", &vertex_num);
        }
        else if (strcmp(dummy, "ARISTAS_REQ") == 0)
        {
            fscanf(fp, "%s", dummy);
            fscanf(fp, "%d", &req_edge_num);
        }
        else if (strcmp(dummy, "ARISTAS_NOREQ")==0)
        {
            fscanf(fp, "%s", dummy);
            fscanf(fp, "%d", &nonreq_edge_num);
        }
        else if (strcmp(dummy, "VEHICULOS")==0)
        {
            fscanf(fp, "%s", dummy);
            fscanf(fp, "%d", &vehicle_num);
        }
        else if (strcmp(dummy, "CAPACIDAD")==0)
        {
            fscanf(fp, "%s", dummy);
            fscanf(fp, "%d", &capacity);
        }
        else if (strcmp(dummy, "LISTA_ARISTAS_REQ")==0) {

            fscanf(fp, "%s", dummy);
            task_num = 2 * req_edge_num + req_arc_num;
            total_arc_num = task_num + 2 * nonreq_edge_num + nonreq_arc_num;
            for (int i = 1; i <= req_edge_num; i++) {
                fscanf(fp, "%s", dummy);
                fscanf(fp, "%d,", &inst_tasks[i].head_node);
                fscanf(fp, "%d)", &inst_tasks[i].tail_node);
                fscanf(fp, "%s", dummy);
                fscanf(fp, "%d", &inst_tasks[i].serv_cost);
                fscanf(fp, "%s", dummy);
                fscanf(fp, "%d", &inst_tasks[i].demand);

                inst_tasks[i].dead_cost = inst_tasks[i].serv_cost;
                inst_tasks[i].inverse = i + req_edge_num;

                inst_tasks[i + req_edge_num].head_node = inst_tasks[i].tail_node;
                inst_tasks[i + req_edge_num].tail_node = inst_tasks[i].head_node;
                inst_tasks[i + req_edge_num].dead_cost = inst_tasks[i].dead_cost;
                inst_tasks[i + req_edge_num].serv_cost = inst_tasks[i].serv_cost;
                inst_tasks[i + req_edge_num].demand = inst_tasks[i].demand;
                inst_tasks[i + req_edge_num].inverse = i;

                inst_arcs[i].head_node = inst_tasks[i].head_node;
                inst_arcs[i].tail_node = inst_tasks[i].tail_node;
                inst_arcs[i].trav_cost = inst_tasks[i].dead_cost;
                inst_arcs[i + req_edge_num].head_node = inst_arcs[i].tail_node;
                inst_arcs[i + req_edge_num].tail_node = inst_arcs[i].head_node;
                inst_arcs[i + req_edge_num].trav_cost = inst_arcs[i].trav_cost;
            }
        }
        else if (strcmp(dummy, "LISTA_ARISTAS_NOREQ")==0)
        {
            fscanf(fp, "%s", dummy);
            for (int i=task_num+1; i<=task_num+nonreq_edge_num;i++)
            {
                fscanf(fp, "%s", dummy);
                fscanf(fp, "%d,", &inst_arcs[i].head_node);
                fscanf(fp, "%d)", &inst_arcs[i].tail_node);
                fscanf(fp, "%s", dummy);
                fscanf(fp, "%d", &inst_arcs[i].trav_cost);

                inst_arcs[i + nonreq_edge_num].head_node = inst_arcs[i].tail_node;
                inst_arcs[i + nonreq_edge_num].tail_node = inst_arcs[i].head_node;
                inst_arcs[i + nonreq_edge_num].trav_cost = inst_arcs[i].trav_cost;

            }
        }
        else if (strcmp(dummy, "DEPOSITO")==0)
        {
            fscanf(fp, "%s", dummy);
            fscanf(fp, "%d", &DEPOT);
        }

    }

    fclose(fp);

    // inst_tasks[dummy_cycle]

    for (int i=1; i<=vertex_num; i++)
    {
        for (int j=1; j<=vertex_num; j++)
        {
            trav_cost[i][j] = INF;
            serve_cost[i][j] = 0;
        }
    }

//    trav_cost[1][0] = 0;
//    trav_cost[0][1] = 0;

    for (int i=1; i<=task_num; i++)
    {
        serve_cost[inst_tasks[i].head_node][inst_tasks[i].tail_node] = inst_tasks[i].serv_cost;
    }

    for (int i=1; i<=total_arc_num; i++)
    {
        trav_cost[inst_arcs[i].head_node][inst_arcs[i].tail_node] = inst_arcs[i].trav_cost;
    }

    mod_dijkstra();
//    ShowMatrix(min_cost, vertex_num);

    int NVer, NRE, NRA, NNR, NVeh, Cap, LLB;
    NVer = vertex_num;
    NRE = req_edge_num;
    NRA = req_arc_num;
    NNR = 2*nonreq_edge_num+nonreq_arc_num;
    NVeh = vehicle_num;
    Cap = capacity;
//    LLB = LowerBound;

    // Initialization
    Individual InitSolution;
    int serve_mark[MAX_TASKS_TAG_LENGTH];
    for (int i=1; i<=task_num; i++)
    {
        serve_mark[i] = 1;
    }


    augment_merge(&InitSolution, inst_tasks);
    path_scanning(&InitSolution, inst_tasks, serve_mark);

    int Positions[MAX_ROUTE_TAG_LENGTH];
    int FHRoute[MAX_TASK_SEG_LENGTH], Route[MAX_TASK_SEG_LENGTH];

    find_ele_positions(Positions, InitSolution.Sequence, 0);

    AssignSubArray(InitSolution.Sequence, Positions[3], Positions[4], Route);

    FredericksonHeuristic(FHRoute, Route, inst_tasks);

    // Parameter
    int FSI = 1;
    int FDI = 1;
    int FSWAP = 1;

    // parmater for iteration
    int npi, mnpi, nti, mnti;
    mnpi = 900 * sqrt(NRE+NRA);
    npi = 1;
    nti = 1;

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
