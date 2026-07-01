#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
#include <vector>

#include "kdtree.h"
#include "draw.h"

void collision(DataType* a, DataType* b)
{
    float dx = (b->x - a->x);
    float dy = (b->y - a->y);

    float ax = dx / sqrt(pow(dx, 2.0) + pow(dy, 2.0));
    float ay = dy / sqrt(pow(dx, 2.0) + pow(dy, 2.0));

    float vb1;
    float vb2;

    float va1 = (a->speed * a->dirx * ax + a->speed * a->diry * ay) / sqrt(pow(a->dirx, 2.0) + pow(a->diry, 2.0));
    if (b->speed == 0) {
        vb1 = 0;
    }
    else {
        vb1 = (b->speed * b->dirx * ax + b->speed * b->diry * ay) / sqrt(pow(b->dirx, 2.0) + pow(b->diry, 2.0));
    }


    float va2 = (a->speed * a->dirx * ay - a->speed * a->diry * ax) / sqrt(pow(a->dirx, 2.0) + pow(a->diry, 2.0));
    if (b->speed == 0) {
        vb2 = 0;
    }
    else {
        vb2 = (b->speed * b->dirx * ay - b->speed * b->diry * ax) / sqrt(pow(b->dirx, 2.0) + pow(b->diry, 2.0));
    }

    a->dirx = vb1 * ax + va2 * ay;
    a->diry = vb1 * ay - va2 * ax;

    b->dirx = va1 * ax + vb2 * ay;
    b->diry = va1 * ay - vb2 * ax;

    a->speed = sqrt(pow(vb1, 2.0) + pow(va2, 2.0));
    b->speed = sqrt(pow(va1, 2.0) + pow(vb2, 2.0));
}

void tcol(kdnode* root, kdnode* node, DataType* wb)
{
    if (!node) { return; }

    tcol(root, node->left, wb);

    DataType tem = node->data;
    DataType nearest;
    float dist;
    kdnode* n = Nearest(root, node->data, nearest, dist);

    if (dist <= 2 * BALLRADIUS && ((nearest.x - tem.x) * tem.dirx + (nearest.y - tem.y) * tem.diry > 0))
    {
        collision(&(node->data), &(n->data));

    }
    float dist_ = sqrt(pow(wb->x - tem.x, 2.0) + pow(wb->y - tem.y, 2.0));
    if (dist_ <= 2 * BALLRADIUS && ((wb->x - tem.x) * tem.dirx + (wb->y - tem.y) * tem.diry > 0))
    {
        collision(&(node->data), wb);

    }

    tcol(root, node->right, wb);
}

void drawBall(DataType b)
{
    setcolor(COLOR_TABLE[b.c]);
    setfillcolor(COLOR_TABLE[b.c]);
    solidcircle((int)b.x, (int)b.y, BALLRADIUS);
}

void drawTree(kdnode* node)
{
    if (!node) { return; }
    drawTree(node->left);
    drawBall(node->data);
    drawTree(node->right);
}

void treemove(kdnode* node)
{
    if (!node) { return; }
    treemove(node->left);

    if (node->data.speed > SPEEDMINS) {
        node->data.y += node->data.speed * node->data.diry / sqrt(pow(node->data.diry, 2) + pow(node->data.dirx, 2));
        node->data.x += node->data.speed * node->data.dirx / sqrt(pow(node->data.diry, 2) + pow(node->data.dirx, 2));

        node->data.speed -= SPEEDMINS;
    }
    else {
        node->data.speed = 0;
    }


    treemove(node->right);

}

bool tmove(kdnode* node)
{
    if (!node) { return false; }

    if (tmove(node->left) == true) {
        return true;
    }


    if (node->data.speed > SPEEDMINS) {
        return true;
    }
    else {
        node->data.speed = 0;
    }

    if (tmove(node->right) == true) {
        return true;
    }

    return false;
}

static float randCoord()
{
    return (float)(BALLRADIUS + rand() % (WINDOW_H - 2 * BALLRADIUS));
}

static DataType randomBall()
{
    DataType ball;
    ball.x = randCoord();
    ball.y = randCoord();
    ball.c = rand() % 6;
    return ball;
}

static void resetCannonBall(DataType* ball)
{
    ball->c = rand() % 6;
    ball->x = WINDOW_W / 2.0f;
    ball->y = (float)WINDOW_H;
}

void cleanup(kdnode** root)
{
    kdtree_destroy(*root);
    *root = nullptr;
    closegraph();
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
}

int istouchBounds(DataType ball)
{
    return ball.x > WINDOW_W - BALLRADIUS || ball.x < BALLRADIUS ||
        ball.y > WINDOW_H - BALLRADIUS || ball.y < BALLRADIUS;
}

void rebound(kdnode* node)
{
    if (!node) { return; }
    rebound(node->left);
    if (istouchBounds(node->data)) {
        if (node->data.x > WINDOW_W - BALLRADIUS || node->data.x < BALLRADIUS) {
            node->data.dirx = -node->data.dirx;
        }
        else {
            node->data.diry = -node->data.diry;
        }
    }
    rebound(node->right);
}

bool hole(DataType a) 
{
    float tem1 = a.x;
    float tem2 = WINDOW_W - a.x;
    float tem3 = a.y;
    float tem4 = WINDOW_H - a.y;

    return (sqrt(pow(tem1, 2.0) + pow(tem3, 2.0)) < HOLESIZE) || (sqrt(pow(tem1, 2.0) + pow(tem4, 2.0)) < HOLESIZE) ||
        (sqrt(pow(tem2, 2.0) + pow(tem3, 2.0)) < HOLESIZE) || (sqrt(pow(tem2, 2.0) + pow(tem4, 2.0)) < HOLESIZE);
}

kdnode* treehole(kdnode* node, kdnode* root)
{
    if (!node) { return NULL; }

    if (treehole(node->left, root) != NULL) {
        return treehole(node->left, root);
    }

    if (hole(node->data)) {
        return node;
    }

    if (treehole(node->right, root) != NULL) {
        return treehole(node->right, root);
    }

    return NULL;
}