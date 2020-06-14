//
// Created by hao on 13/06/2020.
//

# include "TSA.h"

int SingleInsertion(Individual *BestSINeighbor, Individual *CurrSolution, int *MovedTasks, int *ChangedRoutesID, Task *inst_tasks, int (*TabuList)[50],
        double PenPrmt, double BestFitness, double BestFsbFitness )
{
    int i, j, k, u, v, z;

    int count = 0;  // record neighbourhood moves (number of fitness evaluation)
    int Besti, Bestj, Bestu, Bestv, RID1, RID2;
    int Positions[50], Route[50][250];
    find_ele_positions(Positions, CurrSolution->Sequence, 0);

    Route[0][0] = Positions[0] - 1; // PA*
    for(i = 1; i < Positions[0]; i++)
    {
        AssignSubArray(CurrSolution->Sequence, Positions[i], Positions[i+1], Route[i]);
    }

    Individual Neighbor;

    MovedTasks[0] = 1;
    BestSINeighbor->Fitness = INF;

    for (i = 1; i < Positions[0]; i++)
    {
        RID1 = i;
        for (j = 2; j < Route[i][0]; j++) // Route[i]: 0, t1, t2, t3, ..., tn, 0
        {
            for (u = 0; u < Positions[0]; u++)
            {
                if (u == i)
                    continue; // insertion happens in two different routes

                RID2 = u;
                if (u == 0 && Route[0][0]<vehicle_num && Route[i][0] > 3)
                {
                    // assign a new route, the route number < max vehicle, over one tasks in Route[i];
                    AssignArray(CurrSolution->Loads, Neighbor.Loads);
                    Neighbor.Loads[i] -= inst_tasks[Route[i][j]].demand;
                    Neighbor.TotalVioLoad = CurrSolution->TotalVioLoad;

                    if (Neighbor.Loads[i] > capacity)
                    {
                        Neighbor.TotalVioLoad -= inst_tasks[Route[i][j]].demand;
                    } else if ( Neighbor.Loads[i] > capacity - inst_tasks[Route[i][j]].demand)
                    {
                        Neighbor.TotalVioLoad -= Neighbor.Loads[i] + inst_tasks[Route[i][j]].demand - capacity;
                    }

                    Neighbor.Loads[0] ++;
                    Neighbor.Loads[Neighbor.Loads[0]] = inst_tasks[Route[i][j]].demand;

                    if (Neighbor.Loads[Neighbor.Loads[0]] > capacity)
                    {
                        Neighbor.TotalVioLoad += Neighbor.Loads[Neighbor.Loads[0]] - capacity;
                    }

                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                                - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                                + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                                + min_cost[DEPOT][inst_tasks[Route[i][j]].head_node]
                                                + min_cost[inst_tasks[Route[i][j]].tail_node][DEPOT];
                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;

                    // check tabu list
                    if (TabuList[Route[i][j]][u] == 0 || (Neighbor.TotalVioLoad > 0 && Neighbor.Fitness < BestFitness)
                        || (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestSINeighbor->Fitness)
                        {
                            MovedTasks[1] = Route[i][j];
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = 0;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestSINeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestSINeighbor->Loads);
                            BestSINeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestSINeighbor->Fitness = Neighbor.Fitness;
                        }
                    }
                }
                if (u == 0)
                    continue;

                for (v = 2; v <= Route[u][0]; v++) // Route[i]: 0, t1, t2, t3, ..., tn, 0
                {
                    if ( u == i && v == j)
                        continue;

                    AssignArray(CurrSolution->Loads, Neighbor.Loads);
                    Neighbor.Loads[i] -= inst_tasks[Route[i][j]].demand;
                    Neighbor.Loads[u] += inst_tasks[Route[i][j]].demand;
                    Neighbor.TotalVioLoad = CurrSolution->TotalVioLoad;

                    if (Neighbor.Loads[i] > capacity)
                    {
                        Neighbor.TotalVioLoad -= inst_tasks[Route[i][j]].demand;
                    } else if (Neighbor.Loads[i] > capacity - inst_tasks[Route[i][j]].demand)
                    {
                        Neighbor.TotalVioLoad -= (Neighbor.Loads[i] + inst_tasks[Route[i][j]].demand - capacity);
                    }

                    if (Neighbor.Loads[u] > capacity + inst_tasks[Route[i][j]].demand)
                    {
                        Neighbor.TotalVioLoad += inst_tasks[Route[i][j]].demand;

                    } else if (Neighbor.Loads[u] > capacity)
                    {
                        Neighbor.TotalVioLoad += Neighbor.Loads[u] - capacity;
                    }

                    if (Neighbor.Loads[i] == 0)
                    {
                        RID1 = 0;
                        delete_element(Neighbor.Loads, i);
                    }

                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                            - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                            + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                            - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                            + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                            + min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[u][v]].head_node];

                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;

                    if (TabuList[Route[i][j]][u] == 0 || (Neighbor.TotalVioLoad > 0 && Neighbor.Fitness < BestFitness)
                        || (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestSINeighbor->Fitness)
                        {
                            MovedTasks[1] = Route[i][j];
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestSINeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestSINeighbor->Loads);
                            BestSINeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestSINeighbor->Fitness = Neighbor.Fitness;
                        }
                    }
                    // invert selected task
                    z = inst_tasks[Route[i][j]].inverse;
                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[z].head_node]
                                         + min_cost[inst_tasks[z].tail_node][inst_tasks[Route[u][v]].head_node];
                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || (Neighbor.TotalVioLoad > 0 && Neighbor.Fitness < BestFitness)
                        || (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestSINeighbor->Fitness)
                        {
                            MovedTasks[1] = z;
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestSINeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestSINeighbor->Loads);
                            BestSINeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestSINeighbor->Fitness = Neighbor.Fitness;
                        }
                    }
                }
            }
        }
    }

    // Assign Sequence
    if (Bestu == 0)
    {
        // new routes
        BestSINeighbor->Sequence[0] = 1;
        for(k = 1; k < Positions[0]; k++)
        {
            if (k == Besti)
            {
                delete_element(Route[k], Bestj);
                BestSINeighbor->Sequence[0] --; // Route[1] = 0
                JoinArray(BestSINeighbor->Sequence, Route[k]);
            } else {
                BestSINeighbor->Sequence[0] --; // Route[1] = 0
                JoinArray(BestSINeighbor->Sequence, Route[k]);
            }
        }
        BestSINeighbor->Sequence[0] ++;
        BestSINeighbor->Sequence[BestSINeighbor->Sequence[0]] = MovedTasks[1];
        BestSINeighbor->Sequence[0] ++;
        BestSINeighbor->Sequence[BestSINeighbor->Sequence[0]] = 0;

    } else{

        BestSINeighbor->Sequence[0] = 1;
        for(k = 1; k < Positions[0]; k++)
        {
            if (k == Besti)
            {
                delete_element(Route[k], Bestj);
                if (Route[k][0] == 2)
                    continue;

                BestSINeighbor->Sequence[0] --; // Route[1] = 0
                JoinArray(BestSINeighbor->Sequence, Route[k]);
            } else if (k == Bestu){
                add_element(Route[k], MovedTasks[1], Bestv);
                BestSINeighbor->Sequence[0] --; // Route[1] = 0
                JoinArray(BestSINeighbor->Sequence, Route[k]);
            } else {
                BestSINeighbor->Sequence[0] --; // Route[1] = 0
                JoinArray(BestSINeighbor->Sequence, Route[k]);
            }
        }
    }

    return count;

}


int DoubleInsertion(Individual *BestDINeighbor, Individual *CurrSolution, int *MovedTasks, int *ChangedRoutesID, Task *inst_tasks, int (*TabuList)[50],
                    double PenPrmt, double BestFitness, double BestFsbFitness)
{
    int i, j, k, u, v, z, w;

    int count = 0;  // record neighbourhood moves (number of fitness evaluation)
    int Besti, Bestj, Bestu, Bestv, RID1, RID2;
    int Positions[50], Route[50][250];
    find_ele_positions(Positions, CurrSolution->Sequence, 0);

    Route[0][0] = Positions[0] - 1; // PA*
    for(i = 1; i < Positions[0]; i++)
    {
        AssignSubArray(CurrSolution->Sequence, Positions[i], Positions[i+1], Route[i]);
    }

    Individual Neighbor;

    MovedTasks[0] = 2;
    BestDINeighbor->Fitness = INF;


    for (i = 1; i < Positions[0]; i++)
    {
        if (Route[i][0] < 4)
            continue;
        RID1 = i;
        for (j = 2; i < Route[i][0]-1; i++) // Route[i]: 0, t1, t2, t3, ..., tn, 0
        {
            for (u = 0; u < Positions[0]; u++)
            {
                if (u == i)
                    continue;

                RID2 = u;

                if ( u==0 && Route[0][0] < vehicle_num && Route[i][0] > 4)
                {
                    // new routes
                    AssignArray(CurrSolution->Loads, Neighbor.Loads);
                    Neighbor.Loads[i] -= (inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand);
                    Neighbor.TotalVioLoad = CurrSolution->TotalVioLoad;

                    if (Neighbor.Loads[i] > capacity)
                    {
                        Neighbor.TotalVioLoad -= (inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand);
                    } else if ( Neighbor.Loads[i] > (capacity - inst_tasks[Route[i][j]].demand - inst_tasks[Route[i][j+1]].demand))
                    {
                        Neighbor.TotalVioLoad -= (Neighbor.Loads[i] - capacity + inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand);
                    }

                    Neighbor.Loads[0] ++;
                    Neighbor.Loads[Neighbor.Loads[0]] = inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand;

                    if (Neighbor.Loads[Neighbor.Loads[0]] > capacity)
                    {
                        Neighbor.TotalVioLoad += (Neighbor.Loads[Neighbor.Loads[0]] -capacity);
                    }
                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                                                 - min_cost[inst_tasks[Route[i][j+1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                                                 + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                                                 + min_cost[DEPOT][inst_tasks[Route[i][j]].head_node]
                                                                 + min_cost[inst_tasks[Route[i][j+2]].tail_node][DEPOT];
                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;

                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[i][j+1]][u] == 0 || (Neighbor.TotalVioLoad > 0
                        && Neighbor.Fitness < BestFitness) || (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestDINeighbor->Fitness)
                        {
                            MovedTasks[1] = Route[i][j];
                            MovedTasks[2] = Route[i][j+1];
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = 0;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestDINeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestDINeighbor->Loads);
                            BestDINeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestDINeighbor->Fitness = Neighbor.Fitness;
                        }
                    }

                }

                if (u == 0)
                    continue;

                for (v = 2; v <= Route[u][0]; v++)
                {
                    if (u == i && v == j )
                        continue;
                    AssignArray(CurrSolution->Loads, Neighbor.Loads);
                    Neighbor.Loads[i] -= (inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand);
                    Neighbor.Loads[u] += (inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand);
                    Neighbor.TotalVioLoad = CurrSolution->TotalVioLoad;

                    if ( Neighbor.Loads[i] > capacity)
                    {
                        Neighbor.TotalVioLoad -= (inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand);
                    } else if (Neighbor.Loads[i] > capacity - (inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand))
                    {
                        Neighbor.TotalVioLoad -= (Neighbor.Loads[i] - capacity + inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand);
                    }

                    if (Neighbor.Loads[i] > capacity + (inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand))
                    {
                        Neighbor.TotalVioLoad += inst_tasks[Route[i][j]].demand + inst_tasks[Route[i][j+1]].demand;
                    } else if (Neighbor.Loads[i] > capacity)
                    {
                        Neighbor.TotalVioLoad += (capacity - Neighbor.Loads[i]);
                    }
                    if (Neighbor.Loads[i] == 0)
                    {
                        RID1 = 0;
                        delete_element(Neighbor.Loads, i);
                    }
                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         - min_cost[inst_tasks[Route[i][j+1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                         + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                         - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         + min_cost[inst_tasks[Route[i][j+1]].tail_node][inst_tasks[Route[u][v]].head_node];
                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[i][j+1]][u] == 0 || (Neighbor.TotalVioLoad > 0
                       && Neighbor.Fitness < BestFitness) || (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestDINeighbor->Fitness)
                        {
                            MovedTasks[1] = Route[i][j];
                            MovedTasks[2] = Route[i][j+1];
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestDINeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestDINeighbor->Loads);
                            BestDINeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestDINeighbor->Fitness = Neighbor.Fitness;
                        }
                    }

                    w = inst_tasks[Route[i][j]].inverse;
                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         - min_cost[inst_tasks[Route[i][j+1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                         - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                         - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[w].head_node]
                                         + min_cost[inst_tasks[w].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         + min_cost[inst_tasks[Route[i][j+1]].tail_node][inst_tasks[Route[u][v]].head_node];
                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[i][j+1]][u] == 0 || (Neighbor.TotalVioLoad > 0
                                                                                             && Neighbor.Fitness < BestFitness) || (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestDINeighbor->Fitness)
                        {
                            MovedTasks[1] = w;
                            MovedTasks[2] = Route[i][j+1];
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestDINeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestDINeighbor->Loads);
                            BestDINeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestDINeighbor->Fitness = Neighbor.Fitness;
                        }
                    }

                    z = inst_tasks[Route[i][j+1]].inverse;
                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         - min_cost[inst_tasks[Route[i][j+1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                         - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                         - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         + min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[z].head_node]
                                         + min_cost[inst_tasks[z].tail_node][inst_tasks[Route[u][v]].head_node];
                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[i][j+1]][u] == 0 || (Neighbor.TotalVioLoad > 0
                        && Neighbor.Fitness < BestFitness) || (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestDINeighbor->Fitness)
                        {
                            MovedTasks[1] = Route[i][j];
                            MovedTasks[2] = z;
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestDINeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestDINeighbor->Loads);
                            BestDINeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestDINeighbor->Fitness = Neighbor.Fitness;
                        }
                    }

                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         - min_cost[inst_tasks[Route[i][j+1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                         - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j+2]].head_node]
                                         - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[w].head_node]
                                         + min_cost[inst_tasks[w].tail_node][inst_tasks[z].head_node]
                                         + min_cost[inst_tasks[z].tail_node][inst_tasks[Route[u][v]].head_node];
                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[i][j+1]][u] == 0 || (Neighbor.TotalVioLoad > 0
                                                                                             && Neighbor.Fitness < BestFitness) || (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestDINeighbor->Fitness)
                        {
                            MovedTasks[1] = w;
                            MovedTasks[2] = z;
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestDINeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestDINeighbor->Loads);
                            BestDINeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestDINeighbor->Fitness = Neighbor.Fitness;
                        }
                    }
                }
            }
        }
    }
    if (Bestu == 0)
    {
        BestDINeighbor->Sequence[0] = 1;
        for (k = 1; k < Positions[0]; k++)
        {
            if ( k == Besti )
            {
                delete_element(Route[k], Bestj+1);
                delete_element(Route[k], Bestj);
                BestDINeighbor->Sequence[0]--;
                JoinArray(BestDINeighbor->Sequence, Route[k]);
            } else {
                BestDINeighbor->Sequence[0]--;
                JoinArray(BestDINeighbor->Sequence, Route[k]);
            }
        }
        BestDINeighbor->Sequence[0] ++;
        BestDINeighbor->Sequence[BestDINeighbor->Sequence[0]] = MovedTasks[1];
        BestDINeighbor->Sequence[0] ++;
        BestDINeighbor->Sequence[BestDINeighbor->Sequence[0]] = MovedTasks[2];
        BestDINeighbor->Sequence[0] ++;
        BestDINeighbor->Sequence[BestDINeighbor->Sequence[0]] = 0;
    } else{

        BestDINeighbor->Sequence[0] = 1;
        for (k = 1; k < Positions[0]; k++)
        {
            if (k == Besti)
            {
                delete_element(Route[k], Bestj+1);
                delete_element(Route[k], Bestj);
                if (Route[k][0] == 2)
                    continue;

                BestDINeighbor->Sequence[0]--;
                JoinArray(BestDINeighbor->Sequence, Route[k]);
            } else if (k == Bestu)
            {
                add_element(Route[k], MovedTasks[2], Bestv);
                add_element(Route[k], MovedTasks[1], Bestv);
                BestDINeighbor->Sequence[0] --;
                JoinArray(BestDINeighbor->Sequence, Route[k]);
            } else{
                BestDINeighbor->Sequence[0] --;
                JoinArray(BestDINeighbor->Sequence, Route[k]);
            }
        }
    }

    return count;

}


int SWAP(Individual *BestSWAPNeighbor, Individual *CurrSolution, int *MovedTasks, int *ChangedRoutesID, Task *inst_tasks, int (*TabuList)[50],
         double PenPrmt, double BestFitness, double BestFsbFitness)
{
    int i, j, k, u, v, z, w;

    int count = 0;  // record neighbourhood moves (number of fitness evaluation)
    int Besti, Bestj, Bestu, Bestv, RID1, RID2;
    int Positions[50], Route[50][250];
    find_ele_positions(Positions, CurrSolution->Sequence, 0);

    Route[0][0] = Positions[0] - 1; // PA*
    for(i = 1; i < Positions[0]; i++)
    {
        AssignSubArray(CurrSolution->Sequence, Positions[i], Positions[i+1], Route[i]);
    }

    Individual Neighbor;

    MovedTasks[0] = 2;
    BestSWAPNeighbor->Fitness = INF;

    for (i = 1;  i < Route[0][0]; i++)
    {
        RID1 = i;
        for (j = 2; j < Route[i][0]; j++) // Route[i]: 0, t1, t2, t3, ..., tn, 0
        {
            for (u = i+1; u <= Route[0][0]; u++)
            {
                RID2 = u;
                for (v = 2; v < Route[u][0]; v++)
                {
                    AssignArray(CurrSolution->Loads, Neighbor.Loads);
                    Neighbor.Loads[i] -= inst_tasks[Route[i][j]].demand - inst_tasks[Route[u][v]].demand;
                    Neighbor.Loads[u] += inst_tasks[Route[i][j]].demand - inst_tasks[Route[u][v]].demand;
                    Neighbor.TotalVioLoad = CurrSolution->TotalVioLoad;

                    if (inst_tasks[Route[i][j]].demand > inst_tasks[Route[u][v]].demand)
                    {
                        if (Neighbor.Loads[i] > capacity)
                        {
                            Neighbor.TotalVioLoad -= inst_tasks[Route[i][j]].demand - inst_tasks[Route[u][v]].demand;
                        } else if (Neighbor.Loads[i] > capacity - inst_tasks[Route[i][j]].demand + inst_tasks[Route[u][v]].demand)
                        {
                            Neighbor.TotalVioLoad -= Neighbor.Loads[i] + inst_tasks[Route[i][j]].demand - inst_tasks[Route[u][v]].demand - capacity;
                        }

                        if (Neighbor.Loads[u] > capacity + inst_tasks[Route[i][j]].demand - inst_tasks[Route[u][v]].demand)
                        {
                            Neighbor.TotalVioLoad += inst_tasks[Route[i][j]].demand - inst_tasks[Route[u][v]].demand;
                        } else if (Neighbor.Loads[u] > capacity)
                        {
                            Neighbor.TotalVioLoad += Neighbor.Loads[u] - capacity;
                        }
                    } else{
                        if (Neighbor.Loads[u] > capacity)
                        {
                            Neighbor.TotalVioLoad -= inst_tasks[Route[u][v]].demand - inst_tasks[Route[i][j]].demand;
                        }
                        else if (Neighbor.Loads[u] > capacity - inst_tasks[Route[u][v]].demand + inst_tasks[Route[i][j]].demand)
                        {
                            Neighbor.TotalVioLoad -= Neighbor.Loads[u] + inst_tasks[Route[u][v]].demand - inst_tasks[Route[i][j]].demand - capacity;
                        }

                        if (Neighbor.Loads[i] > capacity + inst_tasks[Route[u][v]].demand - inst_tasks[Route[i][j]].demand)
                        {
                            Neighbor.TotalVioLoad += inst_tasks[Route[u][v]].demand - inst_tasks[Route[i][j]].demand;
                        }
                        else if (Neighbor.Loads[i] > capacity)
                        {
                            Neighbor.TotalVioLoad += Neighbor.Loads[i] - capacity;
                        }
                    }
                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                            - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                            - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                            - min_cost[inst_tasks[Route[u][v]].tail_node][inst_tasks[Route[u][v+1]].head_node]
                            + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                            + min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[u][v+1]].head_node]
                            + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                            + min_cost[inst_tasks[Route[u][v]].tail_node][inst_tasks[Route[i][j+1]].head_node];

                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[u][v]][i] == 0 || (Neighbor.TotalVioLoad > 0 && Neighbor.Fitness < BestFitness)	|| (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestSWAPNeighbor->Fitness)
                        {
                            MovedTasks[1] = Route[i][j];
                            MovedTasks[2] = Route[u][v];
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestSWAPNeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestSWAPNeighbor->Loads);
                            BestSWAPNeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestSWAPNeighbor->Fitness = Neighbor.Fitness;
                        }
                    }

                    w = inst_tasks[Route[i][j]].inverse;
                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         - min_cost[inst_tasks[Route[u][v]].tail_node][inst_tasks[Route[u][v+1]].head_node]
                                         + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[w].head_node]
                                         + min_cost[inst_tasks[w].tail_node][inst_tasks[Route[u][v+1]].head_node]
                                         + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         + min_cost[inst_tasks[Route[u][v]].tail_node][inst_tasks[Route[i][j+1]].head_node];

                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[u][v]][i] == 0 || (Neighbor.TotalVioLoad > 0 && Neighbor.Fitness < BestFitness)	|| (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestSWAPNeighbor->Fitness)
                        {
                            MovedTasks[1] = w;
                            MovedTasks[2] = Route[u][v];
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestSWAPNeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestSWAPNeighbor->Loads);
                            BestSWAPNeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestSWAPNeighbor->Fitness = Neighbor.Fitness;
                        }
                    }

                    z = inst_tasks[Route[u][v]].inverse;
                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         - min_cost[inst_tasks[Route[u][v]].tail_node][inst_tasks[Route[u][v+1]].head_node]
                                         + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         + min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[u][v+1]].head_node]
                                         + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[z].head_node]
                                         + min_cost[inst_tasks[z].tail_node][inst_tasks[Route[i][j+1]].head_node];

                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[u][v]][i] == 0 || (Neighbor.TotalVioLoad > 0 && Neighbor.Fitness < BestFitness)	|| (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestSWAPNeighbor->Fitness)
                        {
                            MovedTasks[1] = Route[i][j];
                            MovedTasks[2] = z;
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestSWAPNeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestSWAPNeighbor->Loads);
                            BestSWAPNeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestSWAPNeighbor->Fitness = Neighbor.Fitness;
                        }
                    }

                    Neighbor.TotalCost = CurrSolution->TotalCost - min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[Route[i][j]].head_node]
                                         - min_cost[inst_tasks[Route[i][j]].tail_node][inst_tasks[Route[i][j+1]].head_node]
                                         - min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[Route[u][v]].head_node]
                                         - min_cost[inst_tasks[Route[u][v]].tail_node][inst_tasks[Route[u][v+1]].head_node]
                                         + min_cost[inst_tasks[Route[u][v-1]].tail_node][inst_tasks[w].head_node]
                                         + min_cost[inst_tasks[w].tail_node][inst_tasks[Route[u][v+1]].head_node]
                                         + min_cost[inst_tasks[Route[i][j-1]].tail_node][inst_tasks[z].head_node]
                                         + min_cost[inst_tasks[z].tail_node][inst_tasks[Route[i][j+1]].head_node];

                    count ++;
                    Neighbor.Fitness = Neighbor.TotalCost + PenPrmt * Neighbor.TotalVioLoad;
                    if (TabuList[Route[i][j]][u] == 0 || TabuList[Route[u][v]][i] == 0 || (Neighbor.TotalVioLoad > 0 && Neighbor.Fitness < BestFitness)	|| (Neighbor.TotalVioLoad == 0 && Neighbor.Fitness < BestFsbFitness))
                    {
                        if (Neighbor.Fitness < BestSWAPNeighbor->Fitness)
                        {
                            MovedTasks[1] = w;
                            MovedTasks[2] = z;
                            Besti = i;
                            Bestj = j;
                            Bestu = u;
                            Bestv = v;
                            ChangedRoutesID[0] = RID1;
                            ChangedRoutesID[1] = RID2;

                            BestSWAPNeighbor->TotalCost = Neighbor.TotalCost;
                            AssignArray(Neighbor.Loads, BestSWAPNeighbor->Loads);
                            BestSWAPNeighbor->TotalVioLoad = Neighbor.TotalVioLoad;
                            BestSWAPNeighbor->Fitness = Neighbor.Fitness;
                        }
                    }
                }
            }
        }
    }

    BestSWAPNeighbor->Sequence[0] = 1;
    for (k = 1; k < Positions[0]; k++)
    {
        if (k == Besti)
        {
            delete_element(Route[k], Bestj);
            add_element(Route[k], MovedTasks[2], Bestj);
            BestSWAPNeighbor->Sequence[0] --;
            JoinArray(BestSWAPNeighbor->Sequence, Route[k]);
        } else if (k == Bestu)
        {
            delete_element(Route[k], Bestv);
            add_element(Route[k], MovedTasks[1], Bestv);
            BestSWAPNeighbor->Sequence[0] --;
            JoinArray(BestSWAPNeighbor->Sequence, Route[k]);
        } else{
            BestSWAPNeighbor->Sequence[0] --;
            JoinArray(BestSWAPNeighbor->Sequence, Route[k]);
        }
    }
    return count;

}





























