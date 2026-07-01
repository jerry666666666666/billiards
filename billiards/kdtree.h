#pragma once

#include "ball.h"

// 2d
#define k 2

struct kdnode
{
    DataType data;
    int split;
    kdnode* left;
    kdnode* right;
};


static float Distance(DataType a, DataType b);
kdnode* newNode(DataType data, unsigned depth);
kdnode* kdnode_insert(kdnode* root, DataType data);
kdnode* findMin(kdnode* root, int d);
kdnode* kdnode_delete(kdnode* root, DataType data);
void searchNearest(kdnode* root, DataType target, DataType& nearestdata, float& distance);
kdnode* Nearest(kdnode* root, DataType target, DataType& nearestdata, float& distance);
void kdtree_destroy(kdnode* root);
kdnode* FixKDTree(kdnode* root);
