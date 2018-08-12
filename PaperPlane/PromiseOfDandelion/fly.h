#pragma once
#include<Windows.h>
#include "LinkList.h"
//窗口的尺寸
#define MAX_X 1200
#define MAX_Y 500

typedef enum _FLY_STATE	//	自动飞行物状态
{
	FLY_OUT,
	FLY_HIT,
	FLY_MOVED,
}FLY_STATE;

typedef enum _FLY_TYPE
{
	FLY_TYPE_BULLET,	//子弹
	FLY_TYPE_CLOUD,	//云朵
	FLY_TYPE_FLASH,	//闪电
}FLY_TYPE;

typedef struct _AUTO_FLY	//飞行物
{
	FLY_TYPE type;
	int x;
	int y;
	double dir;				// 移动时，x相对于y
	int x_step;				// x方向每个时间周期移动的距离
	FLY_STATE state;
	unsigned int power;
	int ratio_x;	// 在这个范围内击中都有效。
	int ratio_y;	// 在这个范围内击中都有效。

}AUTO_FLY, *LPAUTO_FLY;


#define RAND_DIR ((double)(rand()*2-RAND_MAX))/(RAND_MAX*5)

#define gen_bullet(x, y)	create_fly(FLY_TYPE_BULLET,	x, y, RAND_DIR, rand()%10+5, 0, 0, 0)	//创造子弹
#define gen_cloud()			create_fly(FLY_TYPE_CLOUD, MAX_X, rand()%MAX_Y, 0, rand()%3+2, 0, 10, 10)	//创造云朵
//#define gen_flash()			create_fly(FLY_TYPE_FLASH, rand()%MAX_X, 0, 0.6, rand()%3+2, 0, 10, 10)	//创造闪电

int flys_init();
LPAUTO_FLY create_fly(
	FLY_TYPE type,
	int x,
	int y,
	double dir,		// 移动时，y相对于x
	int x_step,	// x方向每个时间周期移动的距离
				//FLY_STATE state,
	unsigned int power,
	int ratio_x,		// 在这个范围内击中都有效。
	int ratio_y		// 在这个范围内击中都有效。
);
void flys_destory();
void destory_fly_at(unsigned int i);

unsigned int get_flys_num();
LPAUTO_FLY get_fly_at(unsigned int i);
void fly_move(LPAUTO_FLY auto_fly);
unsigned int get_fly_x(LPAUTO_FLY auto_fly);
unsigned int get_fly_y(LPAUTO_FLY auto_fly);
void fly_out(LPAUTO_FLY auto_fly);
void flys_move_step();
void destory_fly_by_state();