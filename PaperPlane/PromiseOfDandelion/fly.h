#pragma once
#include<Windows.h>
#include "LinkList.h"
//���ڵĳߴ�
#define MAX_X 1200
#define MAX_Y 500

typedef enum _FLY_STATE	//	�Զ�������״̬
{
	FLY_OUT,
	FLY_HIT,
	FLY_MOVED,
}FLY_STATE;

typedef enum _FLY_TYPE
{
	FLY_TYPE_BULLET,	//�ӵ�
	FLY_TYPE_CLOUD,	//�ƶ�
	FLY_TYPE_FLASH,	//����
}FLY_TYPE;

typedef struct _AUTO_FLY	//������
{
	FLY_TYPE type;
	int x;
	int y;
	double dir;				// �ƶ�ʱ��x�����y
	int x_step;				// x����ÿ��ʱ�������ƶ��ľ���
	FLY_STATE state;
	unsigned int power;
	int ratio_x;	// �������Χ�ڻ��ж���Ч��
	int ratio_y;	// �������Χ�ڻ��ж���Ч��

}AUTO_FLY, *LPAUTO_FLY;


#define RAND_DIR ((double)(rand()*2-RAND_MAX))/(RAND_MAX*5)

#define gen_bullet(x, y)	create_fly(FLY_TYPE_BULLET,	x, y, RAND_DIR, rand()%10+5, 0, 0, 0)	//�����ӵ�
#define gen_cloud()			create_fly(FLY_TYPE_CLOUD, MAX_X, rand()%MAX_Y, 0, rand()%3+2, 0, 10, 10)	//�����ƶ�
//#define gen_flash()			create_fly(FLY_TYPE_FLASH, rand()%MAX_X, 0, 0.6, rand()%3+2, 0, 10, 10)	//��������

int flys_init();
LPAUTO_FLY create_fly(
	FLY_TYPE type,
	int x,
	int y,
	double dir,		// �ƶ�ʱ��y�����x
	int x_step,	// x����ÿ��ʱ�������ƶ��ľ���
				//FLY_STATE state,
	unsigned int power,
	int ratio_x,		// �������Χ�ڻ��ж���Ч��
	int ratio_y		// �������Χ�ڻ��ж���Ч��
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