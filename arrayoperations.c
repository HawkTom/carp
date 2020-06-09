//
// Created by hao on 03/06/2020.
//


#include "functions.h"


int max(int *Array)
{
    int Length = Array[0];
    int i, maximum;

    maximum = 0;
    for (i = 1; i <= Length; i++)
    {
        if (Array[i] > maximum)
        {
            maximum = Array[i];
        }
    }

    return maximum;
}

void ReverseDirection(int *Array, int k1, int k2)
// reverse the subarray of Array[1:n] from the position k1 to position k2
{
    int i, k, tmp;
    double m = (k2-k1+1)/2;
    k = (int)m;

    for (i = k1; i < k1+k; i++)
    {
        tmp = Array[i];
        Array[i] = Array[k1+k2-i];
        Array[k1+k2-i] = tmp;
    }
}


void AssignArray(int *Array1, int *Array2)
// assign array1 to array2
{
    int i;

    for (i = 0; i <= Array1[0]; i++)
    {
        Array2[i] = Array1[i];
    }
}

void AssignSubArray(int *Array1, int k1, int k2, int *Array2)
// assign array1[k1:k2] to array2
{
    int i;

    Array2[0] = k2-k1+1;

    for (i = k1; i <= k2; i++)
    {
        Array2[i-k1+1] = Array1[i];
    }
}

void JoinArray(int *JointArray, int *Array)
{
    int i;
    for (i = 1; i <= Array[0]; i++)
    {
        JointArray[0] ++;
        JointArray[JointArray[0]] = Array[i];
    }
}

int RandChoose(int n)
// randomly choose a number between 1 and n
{
    int random;

    random = rand();

    int k = random%n;

    k++;

    return k;
}

void find_ele_positions(int *positions, int *a, int e)
{
    positions[0] = 0;
    for (int i = 1; i <= a[0]; i++)
    {
        if (a[i] == e)
        {
            positions[0] ++;
            positions[positions[0]] = i;
        }
    }
}

void delete_element(int *a, int k)
{
    if (k < 1 || k > a[0])
    {
        printf("%d %d \n", a[0], k);
        printf("the deleting position is wrong!\n");
        exit(0);
    }

    for (int i = k; i < a[0]; i++)
    {
        a[i] = a[i+1];
    }
    a[0] --;
}

int get_task_seq_total_cost(int *task_seq, const Task *inst_tasks)
{
    int total_cost =  0;
    for (int i = 1; i < task_seq[0]; i++)
    {
        total_cost += min_cost[inst_tasks[task_seq[i]].tail_node][inst_tasks[task_seq[i+1]].head_node]+inst_tasks[task_seq[i]].serv_cost;
    }

    return total_cost;
}

int get_total_vio_load(int *route_seg_load)
{
    int total_vio_load = 0;
    for (int i = 1; i <= route_seg_load[0]; i++)
    {
        if (route_seg_load[i] > capacity)
            total_vio_load += route_seg_load[i]-capacity;
    }

    return total_vio_load;
}

int FindTask(int a, int b, const Task *inst_tasks, int NO_Task)
{
    int i;

    for (i = 1; i <= NO_Task; i++)
    {
        if (inst_tasks[i].tail_node == a && inst_tasks[i].head_node == b)
            return i;
    }

    return 0;
}

void MergeTwoRoutes(int *Route1, int invert1, int *Route2, int invert2, const Task *inst_tasks)
// merge two routes with specific direction
{
    int tmp;
    // 0 ----> 1
    if (invert1)
    {
        for (int i = 1; i <= Route1[1]; i++)
        {
            Route1[i] = inst_tasks[Route1[i]].inverse;
        }
        ReverseDirection(Route1, 1, Route1[0]);
    }

    if (invert2)
    {
        for (int i = 1; i <= Route2[1]; i++)
        {
            Route2[i] = inst_tasks[Route2[i]].inverse;
        }
        ReverseDirection(Route2, 1, Route2[0]);
    }
    JoinArray(Route1, Route2);
}