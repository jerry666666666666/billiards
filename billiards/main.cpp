#include<Windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <graphics.h>
#include <conio.h>
#include <tchar.h>
#include "kdtree.h"
#include "ball.h"
#include "draw.h"
#ifdef _DEBUG
#include <crtdbg.h>
#endif
using namespace std;



int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int nShow)
{
    initgraph(WINDOW_W, WINDOW_H);
    setbkcolor(GREEN);
    cleardevice();

    kdnode* root = NULL;

    for (int i = 0; i < NUM_BALLS; ++i)
    {
        DataType ball;
        ball.x = ballx0[i];
        ball.y = bally0[i];
        ball.c = ballc0[i];
        ball.dirx = 0;
        ball.diry = 0;
        ball.speed = 0;
   
        DataType nearest;
        float dist;


        root = kdnode_insert(root, ball);
        drawBall(ball);
    }

    DataType cannon;
    cannon.x = 200;
    cannon.y = WINDOW_H / 2;
    cannon.c = 5;
    cannon.dirx = 0;
    cannon.diry = 0;
    cannon.speed = 0;


    int moving = 0;
    int winh = 0;



    while (root)
    {
        if (MouseHit())
        {
            MOUSEMSG m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN && !moving)
            {
                if (winh) {
                    cannon.x = (float)(m.x );
                    cannon.y = (float)(m.y );

                    winh = 0;
                }
                else {
                    cannon.dirx = (float)(m.x - cannon.x);
                    cannon.diry = (float)(m.y - cannon.y);

                    moving = 1;

                    //读入速度
                    TCHAR tempString[50];
                    InputBox(tempString, 10, _T("speed:(max:10)"));
                    _stscanf_s(tempString, TEXT("%f"), &cannon.speed);
                    cannon.speed /= 2;
                    //cannon.speed = 2.0f;
                }

            }
            else if (m.uMsg == WM_RBUTTONUP)
            {
                cleanup(&root);
                return 0;
            }
        }

        BeginBatchDraw();
        cleardevice();


        if (moving)
        {
            //球移动

            treemove(root);
            root = FixKDTree(root);
            if (cannon.speed > SPEEDMINS) {

                cannon.x +=  cannon.speed * cannon.dirx / sqrt(pow(cannon.diry, 2) + pow(cannon.dirx, 2));
                cannon.y +=  cannon.speed * cannon.diry / sqrt(pow(cannon.diry, 2) + pow(cannon.dirx, 2));


                cannon.speed -= SPEEDMINS;
            }
            else {
                cannon.speed = 0;
            }


            if ((!tmove(root))&&cannon.speed< SPEEDMINS) {
                moving = 0;
            }

            //掉袋

            for (;;) {
                kdnode* tem = treehole(root, root);
                if (tem != NULL) {
                    root = kdnode_delete(root, tem->data);
                }
                else {
                    break;
                }
            }
            if (hole(cannon)) {
                cannon.x =  WINDOW_W*2;
                cannon.speed = 0;
                winh = 1;
            }

            //反弹
            rebound(root);
            if (istouchBounds(cannon)) {
                if (cannon.x > WINDOW_W - BALLRADIUS || cannon.x < BALLRADIUS) {
                    cannon.dirx = -cannon.dirx;
                }
                else {
                    cannon.diry = -cannon.diry;
                }
            }

            //碰撞
            DataType nearest;
            float dist;
            searchNearest(root, cannon, nearest, dist);
            if (dist <= 2 * BALLRADIUS&& ((nearest.x - cannon.x) * cannon.dirx + (nearest.y - cannon.y) * cannon.diry > 0))
            {
                root = kdnode_delete(root, nearest);
              
                collision(&cannon, &nearest);

                root = kdnode_insert(root, nearest);
            }

            tcol(root,root, &cannon);

        }

        //画洞
        setcolor(WHITE);
        setfillcolor(BLACK);
        solidcircle(0, WINDOW_H, HOLESIZE);
        solidcircle(WINDOW_W, WINDOW_H, HOLESIZE);
        solidcircle(WINDOW_W, 0, HOLESIZE);
        solidcircle(0, 0, HOLESIZE);

        //画球
        drawTree(root);
        drawBall(cannon);

        Sleep(1);
        EndBatchDraw();
    }

    cleanup(&root);
    return 0;
}
