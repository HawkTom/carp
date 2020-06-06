//
// Created by hao on 01/06/2020.
//

#include <stdio.h>
#include "functions.h"

void mod_dijkstra()
{
    int i, j, k, m, minimum;
    printf("Dijastra\n");
    for (i = 1; i <= vertex_num; i++)
    {
        for (j = 1; j <= vertex_num; j++)
        {
            if (j == i)
                continue;

            shortest_path[i][j][0] = 1;
            shortest_path[i][j][1] = i;
            min_cost[i][j] = INF;
        }
    }

    int mark[MAX_NODE_TAG_LENGTH], dist[MAX_NODE_TAG_LENGTH], dist1[MAX_NODE_TAG_LENGTH], nearest_neighbor[MAX_NODE_TAG_LENGTH];

    for (i = 1; i <= vertex_num; i++)
    {
        mark[i] = 1;

        for (j = 1; j <= vertex_num; j++)
        {
            if (j == i)
                continue;

            mark[j] = 0;
            dist[j] = trav_cost[i][j];
            dist1[j] = dist[j];
        }

        for (k = 1; k < vertex_num; k++)
        {
            minimum = INF;
            nearest_neighbor[0] = 0;

            for (j = 1; j <= vertex_num; j++)
            {
                if (mark[j])
                    continue;

                if (dist1[j] == INF)
                    continue;

                if (dist1[j] < minimum)
                    minimum = dist1[j];
            }

            if (minimum == INF)
                continue;

            for (j = 1; j <= vertex_num; j++)
            {
                if (mark[j])
                    continue;

                if (dist1[j] == minimum)
                {
                    nearest_neighbor[0] ++;
                    nearest_neighbor[nearest_neighbor[0]] = j;
                }
            }

            int v = nearest_neighbor[1];
            dist1[v] = INF;
            mark[v] = 1;

            if (shortest_path[i][v][0] == 0 || (shortest_path[i][v][0] > 0 && shortest_path[i][v][shortest_path[i][v][0]] != v))
            {
                shortest_path[i][v][0] ++;
                shortest_path[i][v][shortest_path[i][v][0]] = v;
            }

            for (j = 1; j <= vertex_num; j++)
            {
                if (mark[j])
                    continue;

                if (minimum+trav_cost[v][j] < dist[j])
                {
                    dist[j] = minimum+trav_cost[v][j];
                    dist1[j] = minimum+trav_cost[v][j];
                    for (m = 0; m <= shortest_path[i][v][0]; m++)
                    {
                        shortest_path[i][j][m] = shortest_path[i][v][m];
                    }
                }
            }

            for (j = 1; j <= vertex_num; j++)
            {
                if (j == i)
                    continue;

                min_cost[i][j] = dist[j];
            }
        }
    }

    for (i = 1; i <= vertex_num; i++)
    {
        for (j = 1; j <= vertex_num; j++)
        {
            if (shortest_path[i][j][0] == 1)
                shortest_path[i][j][0] = 0;
        }
    }

    for (i = 1; i <= vertex_num; i++)
    {
        shortest_path[i][i][0] = 1;
        shortest_path[i][i][1] = i;
        min_cost[i][i] = 0;
    }
}


