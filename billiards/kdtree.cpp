#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
#include <vector>

#include "kdtree.h"
using namespace std;

kdnode* newNode(DataType data, unsigned depth)
{
    kdnode* temp = new kdnode;
    temp->data = data;
    temp->split = depth % k;
    temp->left = temp->right = nullptr;
    return temp;
}

static kdnode* insertRec(kdnode* root, DataType data, unsigned depth)
{
    if (root == nullptr)
        return newNode(data, depth);

    unsigned cd = depth % k;

    if (cd == 0)
    {
        if (data.x < root->data.x)
            root->left = insertRec(root->left, data, depth + 1);
        else
            root->right = insertRec(root->right, data, depth + 1);
    }
    else
    {
        if (data.y < root->data.y)
            root->left = insertRec(root->left, data, depth + 1);
        else
            root->right = insertRec(root->right, data, depth + 1);
    }

    return root;
}


kdnode* kdnode_insert(kdnode* root, DataType data)
{
    return insertRec(root, data, 0);
}

static kdnode* minNode(kdnode* x, kdnode* y, kdnode* z, int d)
{
    kdnode* res = x;
    if (d == 0)
    {
        if (y != nullptr && y->data.x < res->data.x) res = y;
        if (z != nullptr && z->data.x < res->data.x) res = z;
    }
    else
    {
        if (y != nullptr && y->data.y < res->data.y) res = y;
        if (z != nullptr && z->data.y < res->data.y) res = z;
    }
    return res;
}

static kdnode* findMinRec(kdnode* root, int d, unsigned depth)
{
    if (root == nullptr)
        return nullptr;

    unsigned cd = depth % k;

    if (cd == d)
    {
        if (root->left == nullptr)
            return root;
        return findMinRec(root->left, d, depth + 1);
    }

    return minNode(root,
        findMinRec(root->left, d, depth + 1),
        findMinRec(root->right, d, depth + 1), d);
}

kdnode* findMin(kdnode* root, int d)
{
    return findMinRec(root, d, 0);  
}

static bool arePointsSame(DataType data1, DataType data2)
{
    return data1.x == data2.x && data1.y == data2.y;
}

static kdnode* deleteNodeRec(kdnode* root, DataType data, int depth)
{
    if (root == nullptr)
        return nullptr;

    int cd = depth % k;

    if (arePointsSame(root->data, data))
    {
        if (root->right != nullptr)
        {
            kdnode* Min = findMin(root->right, cd);
            root->data = Min->data;
            root->right = deleteNodeRec(root->right, Min->data, depth + 1);
        }
        else if (root->left != nullptr)
        {
            kdnode* Min = findMin(root->left, cd);
            root->data = Min->data;
            root->right = deleteNodeRec(root->left, Min->data, depth + 1);
            root->left = nullptr;
        }
        else
        {
            delete root;
            return nullptr;
        }
        return root;
    }

    if (cd == 0)
    {
        if (data.x < root->data.x)
            root->left = deleteNodeRec(root->left, data, depth + 1);
        else
            root->right = deleteNodeRec(root->right, data, depth + 1);
    }
    else
    {
        if (data.y < root->data.y)
            root->left = deleteNodeRec(root->left, data, depth + 1);
        else
            root->right = deleteNodeRec(root->right, data, depth + 1);
    }

    return root;
}

kdnode* kdnode_delete(kdnode* root, DataType data)
{
    return deleteNodeRec(root, data, 0);
}

static float Distance(DataType a, DataType b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

static void Addpath(kdnode* root, DataType target, stack<kdnode*>& search_path)
{
    kdnode* pSearch = root;
    while (pSearch != nullptr)
    {
        search_path.push(pSearch);
        if (pSearch->split == 0)
            pSearch = (target.x < pSearch->data.x) ? pSearch->left : pSearch->right;
        else
            pSearch = (target.y < pSearch->data.y) ? pSearch->left : pSearch->right;
    }
}

void searchNearest(kdnode* root, DataType target, DataType& nearestdata, float& distance)
{
    stack<kdnode*> search_path;
    Addpath(root, target, search_path);

    DataType nearest = search_path.top()->data;
    float dist = Distance(nearest, target);

    while (!search_path.empty())
    {
        kdnode* pBack = search_path.top();
        search_path.pop();

        float d = Distance(pBack->data, target);
        if (d < dist)
        {
            nearest = pBack->data;
            dist = d;
        }

        if (pBack->left != nullptr || pBack->right != nullptr)
        {
            int s = pBack->split;
            float splitDist = (s == 0) ? fabsf(pBack->data.x - target.x)
                                       : fabsf(pBack->data.y - target.y);

            if (splitDist < dist)
            {
                kdnode* pSearch;
                if (s == 0)
                    pSearch = (target.x < pBack->data.x) ? pBack->right : pBack->left;
                else
                    pSearch = (target.y < pBack->data.y) ? pBack->right : pBack->left;

                if (pSearch != nullptr)
                    Addpath(pSearch, target, search_path);
            }
        }
    }

    nearestdata = nearest;
    distance = dist;
}

// 函数改为返回kdnode*，参数只出参最近数据和距离
kdnode* Nearest(kdnode* root, DataType target, DataType& nearestdata, float& distance)
{
    stack<kdnode*> search_path;
    Addpath(root, target, search_path);
    distance = 1e8f;
    kdnode* resNode = nullptr;

    while (!search_path.empty())
    {
        kdnode* pBack = search_path.top();
        search_path.pop();

        float d = Distance(pBack->data, target);
        // 距离大于极小值，排除自身节点
        if (d > 1e-6f && d < distance)
        {
            distance = d;
            nearestdata = pBack->data;
            resNode = pBack;
        }

        int s = pBack->split;
        float splitDist = (s == 0) ? fabs(pBack->data.x - target.x) : fabs(pBack->data.y - target.y);
        if (splitDist < distance)
        {
            kdnode* pSearch;
            if (s == 0)
                pSearch = target.x < pBack->data.x ? pBack->right : pBack->left;
            else
                pSearch = target.y < pBack->data.y ? pBack->right : pBack->left;
            if (pSearch) Addpath(pSearch, target, search_path);
        }
    }
    return resNode;
}

void kdtree_destroy(kdnode* root)
{
    if (root)
    {
        kdtree_destroy(root->left);
        kdtree_destroy(root->right);
        delete root;
    }
}


static void collectAll(kdnode* p, vector<DataType>& buf)
{
    if (!p) return;
    collectAll(p->left, buf);
    buf.push_back(p->data);
    collectAll(p->right, buf);
}

kdnode* FixKDTree(kdnode* root)
{
    vector<DataType> arr;
    collectAll(root, arr);
    kdtree_destroy(root);
    kdnode* newRoot = nullptr;
    for (auto& d : arr)
    {
        newRoot = kdnode_insert(newRoot, d);
    }
    return newRoot;
}