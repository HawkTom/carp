//
// Created by hao on 08/06/2020.
//

#include "functions.h"

void dfs(int u, int *visited, int *parent, int *low, int *disc, int *time, int (* AdMatrix)[MAX_NODE_TAG_LENGTH]);
void findBridge(int (* AdMatrix)[MAX_NODE_TAG_LENGTH]);
void dynamicChange(const Task *inst_tasks, const Arc *inst_arcs, unsigned int seed);
void executeSolution(Individual *Solution, const Task *inst_tasks);


void nextScenario(Individual *Solution, const Task *inst_tasks, const Arc *inst_arcs, unsigned int seed)
{
    executeSolution(Solution, inst_tasks);
    dynamicChange(inst_tasks, inst_arcs, seed);
}


/*
 input: start point, solution,
 output: stop point, remain capacity;
 */
void executeSolution(Individual *Solution, const Task *inst_tasks)
{
    int i, j;
    int tau = 100, dis = 0;
    int start;
    int stop[50], remain_capacity[50];
    int Position[50];
    int serveTasks[MAX_TASKS_TAG_LENGTH];
    find_ele_positions(Position, Solution->Sequence, 0);
    serveTasks[0] = 0;
    stop[0] = 0;
    remain_capacity[0] = 0;

    int currTask, currNode, currLoad;
    for(i=1; i < Position[0]; i++)
    {

        start = DEPOT;
        currLoad = 0;
        for (j = Position[i]+1; j <= Position[i+1]; j++)
        {
            currTask = Solution->Sequence[j];

            currNode = inst_tasks[currTask].head_node;
            dis += min_cost[start][currNode];
            if (dis > tau)
                break;

            currNode = inst_tasks[currTask].tail_node;
            dis += min_cost[start][currNode];
            if (dis > tau)
                break;

            serveTasks[0] ++;
            serveTasks[serveTasks[0]] = currTask;
            currLoad += inst_tasks[currTask].demand;
            start = currNode;
        }
        if ( currNode != 0)
        {
            stop[0] ++;
            stop[stop[0]] = currNode;

            remain_capacity[0] ++;
            remain_capacity[remain_capacity[0]] = capacity - currLoad;
        }
    }
}

/*
 input: inst_tasks, inst_arcs
 output: new_inst_tasks, new_inst_tasks, new_req_edge_num, new_nonreq_edge_num, new_vertex_num;
 */
void dynamicChange(const Task *inst_tasks, const Arc *inst_arcs, unsigned int seed)
{
    typedef struct edge
    {
        int head_node;
        int tail_node;
        int trav_cost;
        int demand;
        int link;
        unsigned int change;
    } Edge;


    float p2, p3, p4, p5, p_bdrr, p_crr, p_crbb;
    p2 = 0.5; p3 = 0.9;  // related to cost
    p4 = 0.35; p5 = 0.35; // related to demand
    p_bdrr = 0.5; // breakdown road recover
    p_crbb = 0.6; // congestion road become better
    p_crr = 0.3; // congestion road recover

    int i, j;
    Edge graph[MAX_TASKS_TAG_LENGTH];
    int AdMatrix[MAX_NODE_TAG_LENGTH][MAX_NODE_TAG_LENGTH];

    for (i=1; i <= req_edge_num; i++)
    {
        graph[i].head_node = inst_tasks[i].head_node;
        graph[i].tail_node = inst_tasks[i].tail_node;
        graph[i].trav_cost = inst_tasks[i].dead_cost;
        graph[i].demand = inst_tasks[i].demand;
        graph[i].change = inst_arcs[i].change;
        graph[i].link = inst_arcs[i].link;
        AdMatrix[graph[i].head_node][graph[i].tail_node] = i;
        AdMatrix[graph[i].tail_node][graph[i].head_node] = i;
    }

    for (j=req_edge_num+1; j<=req_edge_num+nonreq_edge_num; j++)
    {
        i = j + req_edge_num;
        graph[j].head_node = inst_arcs[i].head_node;
        graph[j].tail_node = inst_arcs[i].tail_node;
        graph[j].trav_cost = inst_arcs[i].trav_cost;
        graph[j].demand = 0;
        graph[j].change = inst_arcs[i].change;
        graph[j].link = inst_arcs[j].link;
        AdMatrix[graph[j].head_node][graph[j].tail_node] = j;
        AdMatrix[graph[j].tail_node][graph[j].head_node] = j;
    }

    // find all bridges
    // if AdMatrix[i][j] = -1, (i, j) is a bridge;
    findBridge(AdMatrix);

    float rnum[10];
    srand(seed);

    int edge_num = req_edge_num + nonreq_edge_num;
    int newnode = 0;
    int mergenode = 0;

    // Demand change
    int demand_change;
    for (i=1; i<= req_edge_num + nonreq_edge_num; i++)
    {
        if ( (( rand() / (float) RAND_MAX) < p4) && (graph[i].change != 2))
        {
            // add some demands
            demand_change = (int)(( rand() / (float) RAND_MAX ) * (demandub - demandlb));
            if (graph[i].demand + demand_change < capacity)
            {
                graph[i].demand += demand_change; // It is might different from different solution;
            }
            else
            {
                graph[i].demand = capacity;
            }
        }
    }

    // Cost change
    int tmpNode, tmpDmd, tmpCost;
    for (i=1; i<= req_edge_num + nonreq_edge_num; i++)
    {
        for (j=0; j < 10; j++)
        {
            rnum[j] = ( rand() / (float) RAND_MAX);
            //2, 3, 7, 6, 4, 5
        }
        if(graph[i].change == 0)
        {
            // Event 1 happen
            if (rnum[2] < p2 && rnum[3] < p3 )
            {
                // road become congestion
                graph[i].trav_cost += (int)(rnum[7] * (costub - costlb));
                graph[i].change = 1;
            }
            // Event 2 happen
            if (rnum[2] < p2 && rnum[3] >= p3 )
            {
                // road break down
                if (AdMatrix[graph[i].head_node][graph[i].tail_node] != -1)
                {
                    tmpDmd = graph[i].demand;
                    tmpCost = graph[i].trav_cost;
                    tmpNode = graph[i].tail_node;

                    newnode ++;
                    graph[i].tail_node = vertex_num + newnode;
                    graph[i].demand = (int)(rnum[6] * tmpDmd);
                    graph[i].trav_cost = (int)(rnum[6] * tmpCost);
                    graph[i].link = ++edge_num;
                    graph[i].change = 2;

                    newnode ++;
                    graph[edge_num].head_node = vertex_num + newnode;
                    graph[edge_num].tail_node = tmpNode;
                    graph[edge_num].demand = tmpDmd - graph[i].demand;
                    graph[edge_num].trav_cost = tmpCost - graph[i].trav_cost;
                    graph[edge_num].link = i;
                    graph[edge_num].change = 2;
                }
            }
            continue;
        }

        if(graph[i].change == 1)
        {
            if (rnum[4] < p_crr) // congestion road recover
            {
                graph[i].trav_cost = cost_backup[graph[i].head_node][graph[i].tail_node];
                graph[i].change = 0;
            }
            else if (rnum[4] > p_crbb) // congestion road become worse
            {
                graph[i].trav_cost = (int) ((1.0 - rnum[8]) * graph[i].trav_cost + 1.0 * rnum[8] * cost_backup[graph[i].head_node][graph[i].tail_node]);
            }
            else // congestion road become better
            {
                graph[i].trav_cost += (int)(rnum[8] * 0.9 * (costub - costlb));
            }
            continue;
        }

        if(graph[i].change == 2)
        {
            // breakdown road recover
            if (rnum[5] < p_bdrr)
            {
                graph[i].tail_node = graph[graph[i].link].tail_node;
                graph[i].demand += graph[graph[i].link].demand;
                graph[i].trav_cost += graph[graph[i].link].trav_cost;
                graph[i].change = 0;
                graph[graph[i].link].change = 7;
                graph[i].link = 0;
                mergenode += 2;
            }
            continue;
        }
    }

    // update graph => inst_tasks;
    Edge new_edges[MAX_TASKS_TAG_LENGTH];
    int new_vertex_num = vertex_num + newnode - mergenode;
    int new_req_edge_num = 0;
    int new_nonreq_edge_num = 0;
    int vnum = vertex_num;
    j = 0;
    int head, tail;
    for (i = 1; i <= edge_num; i++)
    {
        if (graph[i].change != 7)
        {
            j++;
            if (graph[i].tail_node > vertex_num)
            {
                head = graph[i].head_node;
                tail = ++vnum;
                graph[graph[i].link].link = j;

            } else if (graph[i].head_node > vertex_num)
            {
                head = ++vnum;
                tail = graph[i].tail_node;
                new_edges[j].link = graph[i].link;
                new_edges[graph[i].link].link = j;
            } else {
                head = graph[i].head_node;
                tail = graph[i].tail_node;
                new_edges[j].link = graph[i].link;
            }

            new_edges[j].head_node = head;
            new_edges[j].tail_node = tail;
            new_edges[j].trav_cost = graph[i].trav_cost;
            new_edges[j].demand = graph[i].demand;
            new_edges[j].change = graph[i].change;
            if (new_edges[j].demand > 0)
            {
                new_req_edge_num ++;
            } else{
                new_nonreq_edge_num ++;
            }
        }
    }
    int new_task_num = 2 * new_req_edge_num;
    Task new_inst_tasks[MAX_TASKS_TAG_LENGTH];
    Arc new_inst_arcs[MAX_ARCS_TAG_LENGTH];

    int mapping1[new_req_edge_num+new_nonreq_edge_num+1];
    int mapping2[new_req_edge_num+new_nonreq_edge_num+1];
    memset(mapping1, 0, sizeof(mapping1));
    memset(mapping2, 0, sizeof(mapping2));
    int u = 0, v = new_task_num;
    for (i=1; i <= j; i++)
    {
        if (new_edges[i].demand > 0)
        {
            u ++;
            new_inst_tasks[u].head_node = new_edges[i].head_node;
            new_inst_tasks[u].tail_node = new_edges[i].tail_node;
            new_inst_tasks[u].serv_cost = new_edges[i].trav_cost;
            new_inst_tasks[u].dead_cost = new_edges[i].trav_cost;
            new_inst_tasks[u].demand = new_edges[i].demand;
            new_inst_tasks[u].inverse = u + new_req_edge_num;

            new_inst_tasks[u+new_req_edge_num].head_node =  new_inst_tasks[u].tail_node;
            new_inst_tasks[u+new_req_edge_num].tail_node = new_inst_tasks[u].head_node;
            new_inst_tasks[u+new_req_edge_num].dead_cost = new_inst_tasks[u].dead_cost;
            new_inst_tasks[u+new_req_edge_num].serv_cost = new_inst_tasks[u].serv_cost;
            new_inst_tasks[u+new_req_edge_num].demand = new_inst_tasks[u].demand;
            new_inst_tasks[u+new_req_edge_num].inverse = u;

            new_inst_arcs[u].head_node = new_inst_tasks[u].head_node;
            new_inst_arcs[u].tail_node = new_inst_tasks[u].tail_node;
            new_inst_arcs[u].trav_cost = new_inst_tasks[u].dead_cost;
            new_inst_arcs[u].change = new_edges[i].change;
            mapping1[u] = i;
            mapping2[i] = u;

            new_inst_arcs[u+new_req_edge_num].head_node = new_inst_tasks[u].head_node;
            new_inst_arcs[u+new_req_edge_num].tail_node = new_inst_tasks[u].tail_node;
            new_inst_arcs[u+new_req_edge_num].trav_cost = new_inst_tasks[u].dead_cost;
            new_inst_arcs[u+new_req_edge_num].change = new_inst_arcs[u].change;

        } else{
            v++;
            new_inst_arcs[v].head_node = new_edges[i].head_node;
            new_inst_arcs[v].tail_node = new_edges[i].tail_node;
            new_inst_arcs[v].trav_cost = new_edges[i].trav_cost;
            new_inst_arcs[v].change = new_edges[i].change;
            mapping1[v - new_req_edge_num] = i;
            mapping2[i] = v - new_req_edge_num;

            new_inst_arcs[v+new_nonreq_edge_num].head_node = new_edges[i].head_node;
            new_inst_arcs[v+new_nonreq_edge_num].tail_node = new_edges[i].tail_node;
            new_inst_arcs[v+new_nonreq_edge_num].trav_cost = new_edges[i].trav_cost;
            new_inst_arcs[v+new_nonreq_edge_num].change = new_edges[i].change;
        }
    }

    for(i = 1; i <= new_req_edge_num; i++)
    {
        if(new_inst_arcs[i].change == 2)
        {
            new_inst_arcs[i].link = mapping2[new_edges[mapping1[i]].link];
        }
//        printf("%d: (%d, %d), cost: %d, demand: %d, link: %d, change: %d\n", i, new_inst_arcs[i].head_node, new_inst_arcs[i].tail_node, new_inst_arcs[i].trav_cost, new_inst_tasks[i].demand, new_inst_arcs[i].link, new_inst_arcs[i].change);
    }
    for(i = new_task_num + 1; i <= new_task_num + new_nonreq_edge_num; i++)
    {
        if(new_inst_arcs[i].change == 2)
        {
            new_inst_arcs[i].link = mapping2[new_edges[mapping1[i-new_req_edge_num]].link];
        }
//        printf("%d: (%d, %d), cost: %d, link: %d, change: %d\n", i-new_req_edge_num, new_inst_arcs[i].head_node, new_inst_arcs[i].tail_node, new_inst_arcs[i].trav_cost, new_inst_arcs[i].link, new_inst_arcs[i].change);
    }
    printf("dynamic Change !!!\n");

}


void findBridge(int (* AdMatrix)[MAX_NODE_TAG_LENGTH])
{
    int visited[vertex_num+1];
    int disc[vertex_num+1];
    int low[vertex_num+1];
    int parent[vertex_num+1];

    memset(visited, 0, sizeof(visited));
    memset(disc, INF, sizeof(disc));
    memset(low, INF, sizeof(low));
    memset(parent, -1, sizeof(parent));

    int time = 0;
    for (int i=1; i <= vertex_num; i++)
    {
        if ( !visited[i])
            dfs(i, visited, parent, low, disc, &time, AdMatrix);
    }

}

void dfs(int u, int *visited, int *parent, int *low, int *disc, int *time, int (* AdMatrix)[MAX_NODE_TAG_LENGTH])
{
    visited[u] = 1;
    disc[u] = *time;
    low[u] = *time;
    *time  += 1;

    int v;
    for (int i=1; i <= vertex_num; i++)
    {
        if (AdMatrix[u][i] > 0)
        {
            v = i;
            if ( !visited[v] )
            {
                parent[v] = u;
                dfs(v, visited, parent, low, disc, time, AdMatrix);

                low[u] = (low[u] < low[v] )? low[u] : low[v];
                if (low[v] > disc[u])
                {
                    AdMatrix[u][v] = -1;
                    AdMatrix[v][u] = -1;
//                    printf("bridge: %d, %d\n", u, v); //get bridges
                }
            }
            else if ( v != parent[u])
            {
                low[u] = (low[u] < disc[v] )? low[u] : disc[v];
            }
        }

    }


}