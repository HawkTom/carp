//
// Created by hao on 19/06/2020.
//

# include "LMA.h"

int* rand_perm(int num)
{
    int *a = (int *)malloc((num+1)*sizeof(int));
    int *left_ele = (int *)malloc((num+1)*sizeof(int));
    left_ele[0] = num;
    for (int i = 1; i <= num; i++)
    {
        left_ele[i] = i;
    }

    a[0] = num;
    for (int i = 1; i <= num; i++)
    {
        int k = rand_choose(left_ele[0]);
        a[i] = left_ele[k];
        delete_element(left_ele, k);
    }

    free(left_ele);

    return a;
}


void evaluate_route(Individual *indi, const Task *inst_tasks)
{
    indi->Assignment[0] = 0;
    for (int i = 1; i <= indi->Sequence[0]; i++)
    {
        if (indi->Sequence[i] != 0)
        {
            indi->Assignment[0] ++;
            indi->Assignment[indi->Assignment[0]] = indi->Sequence[i];
        }
    }

    indi->TotalCost = split(indi->Sequence, indi->Assignment, inst_tasks);
}

