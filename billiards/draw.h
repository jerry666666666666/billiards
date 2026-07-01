#pragma once
#include <graphics.h>

#define WINDOW_W  1000
#define WINDOW_H  600
#define NUM_BALLS 15
#define SPEEDMINS 0.003
#define HOLESIZE 3*BALLRADIUS

static COLORREF COLOR_TABLE[] = { BLUE,  RED, YELLOW, MAGENTA, BROWN ,WHITE,BLACK };
static int ballx0[] = { 830,830,830,830,830,800,800,800,800,770,770,770,740,740,710 };
static int bally0[] = { 360,330,300,270,240,255,285,315,345,330,300,270,285,315,300 };
static int ballc0[] = { 0,1,0,1,0,1,0,1,0,1,6,0,1,0,1 };

void collision(DataType* a, DataType* b);
void tcol(kdnode* root, kdnode* node, DataType* wb);
void drawBall(DataType b);
void drawTree(kdnode* node); 
void treemove(kdnode* node);
bool tmove(kdnode* node); 
int istouchBounds(DataType ball);
void rebound(kdnode* node);
bool hole(DataType a);
kdnode* treehole(kdnode* node, kdnode* root);
void cleanup(kdnode** root);