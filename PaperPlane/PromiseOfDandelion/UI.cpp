#include<Windows.h>
#include "resource.h"
#include "fly.h"
#include "UI.h"
#include "LinkList.h"
//*********************************************************
#define TIMER_ID 12340
#define PAPERPLANE_HEIGHT 50	//纸飞机的高度
#define PAPERPLANE_WIDTH 50		//纸飞机的宽度
#define CLOUD_WIDTH 50	//云的宽度
#define CLOUD_HEIGHT 60	//云的高度

#define PAPERPLANE_MOVE_STEP 8	//纸飞机每次移动的像素

#define COLOR_BULLET RGB(124,252,0)	//子弹颜色
#define COLOR_BOMB RGB(255,0,0)	//爆炸特效颜色
#define COLOR_CLOUD RGB(0,255,0)

POINT ptPaperplane;
DWORD dwTimerElapse = 40;	//时间单位间隔，单位毫米

HINSTANCE hinst;

HWND hwndPaperPlane;
HWND hwndCloud;
HWND hwndBackground;

HBITMAP hbmpPaperPlane;
HBITMAP hbmpCloud;
HBITMAP hbmpBackground;

//*********************************************************
int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow	
)
{
	WNDCLASSEX wc;	//创建窗口类
	MSG msg;	//消息句柄

	//注册窗口类
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;	//窗口菜单的资源名
	wc.lpszClassName = "myWinClass";	//窗口类的名字
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//检查窗口类
	if (!RegisterClassEx(&wc))
	{
		GetLastError();
		MessageBox(NULL, "创建窗口CLASS失败", "错误！", MB_ICONERROR | MB_OK);
		return -1;
	}
	//窗口注册成功
	//创建主窗口
	hwndBackground = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST,	//拓展的窗口风格
		"myWinClass",	//窗口类名，告诉系统要创建哪种类型的窗口
		"",	//窗口的标题
		WS_POPUP,	//设置窗口的样式参数
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	//窗口的左上角的X和Y坐标，宽度和高度
		NULL, NULL, hinstance, NULL	//父窗口句柄，菜单处理，应用程序实例句柄，窗口创建数据的指针
	);
	if (hwndBackground == NULL)
	{
		MessageBox(NULL, "窗口创建失败", "错误！", MB_ICONERROR | MB_OK);
		return -1;
	}
	//窗口创建成功，继续运行

	if (!SetLayeredWindowAttributes(
		hwndBackground, RGB(255,255,255),
		250, LWA_ALPHA))
	{
		DWORD dwError = GetLastError();
	}

	//显示窗口
	ShowWindow(hwndBackground, SW_SHOW);
	//刷新窗口，使窗口进行重绘
	UpdateWindow(hwndBackground);
	

	//消息循环
	while ( GetMessage(&msg, NULL, 0, 0) !=0|| GetMessage(&msg, NULL, 0, 0) != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//背景
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	switch (msg)
	{
	case WM_CREATE:
		BackgroundCreate(hwnd);	//初始化背景
		SetFocus(hwnd);	  //将键盘焦点设置为主窗口
		PostMessage(hwnd, WM_LBUTTONDOWN, 0, (DWORD)0x0050050);
		PostMessage(hwnd, WM_LBUTTONUP, 0, (DWORD)0x0050050);
		break;
	// 所有使用GDI在窗口上绘制图形的程序都 “必须” 写在这里。
	// 如果不是在WM_PAINT消息的处理过程中绘制GDI图形，那么在窗口刷新时就会被新被抹除和覆盖
	case WM_PAINT:
		BackgroundPaint(hwnd);	
		break;
	case WM_MOVING:
		BackgroundResizeAndMove(hwnd,(LPRECT)lParam);	//调整窗口大小和位置
		break;
	case WM_KILLFOCUS:
		KillTimer(hwndPaperPlane, TIMER_ID);
		break;
	case WM_SETFOCUS:
		SetTimer(hwndPaperPlane, TIMER_ID, dwTimerElapse, NULL);
		break;
	case WM_SIZE:
		GetWindowRect(hwnd, &rect);
		BackgroundResizeAndMove(hwnd, &rect);
		break;
	case WM_KEYDOWN:
		OnKeydown(hwnd, (UINT)wParam);
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_TIMER:
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd); 
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
LONG BackgroundCreate(HWND hwnd)
{
	PaperplaneWindowCreate(hinst);
	CloudWindowCreate(hinst);

	hbmpBackground = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	//hbmpBackground = LoadBitmap(NULL, MAKEINTRESOURCE(IDB_BITMAP3));
	if (hbmpBackground == NULL)
	{
		MessageBox(hwnd, "bmp file not find", "ERROR!",
			MB_ICONERROR | MB_OK);
		return -1;
	}

	MoveWindow(hwnd, 100, 100, MAX_X, MAX_Y, TRUE);	//改变窗口位置和尺寸

	//设置纸飞机的初始位置
	ptPaperplane.x = MAX_X-PAPERPLANE_WIDTH;
	ptPaperplane.y = MAX_Y / 2;

	SetFocus(hwnd);

	return 0;
}
LONG BackgroundPaint(HWND hwnd)
{
	HDC hdc, mdc;
	RECT rect;	//RECT类型是一个结构体包括left、top、right、bottom（客户区）

	BITMAP bmp;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);	//用于取得指定窗口的客户区域大小

								//每个窗口区域可分为两部分，用于显示内容的区域叫Client区（客户区），
								//NonClient（非客户区）用于显示命令按钮及窗口标题等。画客户区消息是由WM_PAINT发出的，而非客户区的绘图是由WM_NCPAINT发出的

	mdc = CreateCompatibleDC(hdc);
	SelectObject(mdc, hbmpBackground);	//将位图刷放进画布里

	GetObject(hbmpBackground, sizeof(BITMAP), &bmp);	//该函数得到指定图形对象的信息，
														//根据图形对象，函数把填满的或结构，或表项（用于逻辑调色板）数目放入一个指定的缓冲区,&bmp指向缓冲区

	StretchBlt(hdc,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		mdc,
		0, 0,
		bmp.bmWidth, bmp.bmHeight,
		SRCCOPY);

	DeleteObject(hbmpBackground);
	DeleteDC(mdc);
	ReleaseDC(hwnd, hdc);
	DeleteDC(hdc);

	return 0;
}
LONG BackgroundResizeAndMove(HWND hwnd, LPRECT lpRect)
{
	SetWindowPos(hwndPaperPlane, HWND_TOPMOST,
		(lpRect->left + lpRect->right - PAPERPLANE_WIDTH) / 2,
		lpRect->bottom - PAPERPLANE_HEIGHT,
		PAPERPLANE_WIDTH,
		PAPERPLANE_HEIGHT,
		SWP_SHOWWINDOW);

	SetFocus(hwnd);

	return 0;
}

//纸飞机和子弹
LONG PaperplaneWindowCreate(HINSTANCE hinstance)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = FlysProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255,255,255));
	wc.lpszMenuName = NULL;	//窗口菜单的资源名
	wc.lpszClassName = "flys";	//窗口类的名字
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		GetLastError();
		MessageBox(NULL, "创建窗口CLASS失败", "错误！", MB_ICONERROR | MB_OK);
		return -1;
	}
	hwndPaperPlane = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE,	//拓展的窗口风格
		"flys",	//窗口类名，告诉系统要创建哪种类型的窗口
		"",	//窗口的标题
		WS_POPUP,	//设置窗口的样式参数
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	//窗口的左上角的X和Y坐标，宽度和高度
		NULL, NULL, hinstance, NULL	//父窗口句柄，菜单处理，应用程序实例句柄，窗口创建数据的指针
	);
	if (hwndPaperPlane == NULL)
	{
		MessageBox(NULL, "窗口创建失败", "错误！", MB_ICONERROR | MB_OK);
		return -1;
	}
	//设置窗口透明度    
	if (!SetLayeredWindowAttributes(hwndPaperPlane, RGB(255,255,255),255, LWA_COLORKEY | LWA_ALPHA))
	{
		DWORD dwError = GetLastError();
	}

	ShowWindow(hwndPaperPlane, SW_SHOW);
	UpdateWindow(hwndPaperPlane);

	return 0;
}
LRESULT CALLBACK FlysProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		FlysCreate(hwnd); 
		break;
	case WM_PAINT:
		FlysPaint(hwnd);
		break;
	case WM_KEYDOWN:
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_TIMER:
		FlysTimer(hwnd);
		FlysPaint(hwnd);
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
LONG FlysCreate(HWND hwnd)
{
	hbmpPaperPlane = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hbmpPaperPlane== NULL)
	{
		MessageBox(hwnd, "bmp file not find", "ERROR!",
			MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}

	flys_init();
	SetTimer(hwnd, TIMER_ID, dwTimerElapse, NULL);

	SetFocus(hwnd);

	return 0;
}
LONG FlysPaint(HWND hwnd)
{
	RECT rect;
	HDC hdc, mdc, mdc_;
	HBITMAP hBitmap;
	BITMAP bmp;

	//画纸飞机
	GetClientRect(hwnd, &rect);

	hdc = GetDC(hwnd);
	mdc = CreateCompatibleDC(hdc);
	mdc_ = CreateCompatibleDC(hdc);

	hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);

	SelectObject(mdc, hBitmap);
	SelectObject(mdc, hbmpPaperPlane);

	GetObject(hbmpPaperPlane, sizeof(BITMAP), &bmp);

	FillRect(mdc, &rect, (HBRUSH)GetStockObject(NULL_BRUSH));	//填充矩形

	TransparentBlt(mdc,
		ptPaperplane.x-PAPERPLANE_WIDTH,ptPaperplane.y - PAPERPLANE_HEIGHT / 2,
		PAPERPLANE_WIDTH, PAPERPLANE_HEIGHT,
		mdc_,
		0, 0, bmp.bmWidth, bmp.bmHeight,
		RGB(255,255,255));

	//画子弹
	HBRUSH hbrBullet;

	hbrBullet = CreateSolidBrush(COLOR_BULLET);

	int num,i;
	num = get_flys_num();	//获得链表节点数量

	LPAUTO_FLY auto_fly;

	for (i = 0; i < num; i++)
	{
		auto_fly = get_fly_at(i);
		if (auto_fly->type == FLY_TYPE_BULLET)
		{
			SelectObject(mdc, hbrBullet);
			Ellipse(mdc,
				get_fly_x(auto_fly) - 4,
				get_fly_y(auto_fly) - 6,
				get_fly_x(auto_fly) + 4,
				get_fly_y(auto_fly) + 6);
		}
	}



	//拷贝到DC
	BitBlt(hdc,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		mdc,
		0, 0,
		SRCCOPY);

	DeleteObject(hbrBullet);
	DeleteObject(hBitmap);

	DeleteDC(mdc_);
	DeleteDC(mdc);
	ReleaseDC(hwnd, hdc);

	return 0;
}

//云朵
LONG CloudWindowCreate(HINSTANCE hinstance)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = CloudsProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
	wc.lpszMenuName = NULL;	//窗口菜单的资源名
	wc.lpszClassName = "clouds";	//窗口类的名字
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		GetLastError();
		MessageBox(NULL, "创建窗口CLASS失败", "错误！", MB_ICONERROR | MB_OK);
		return -1;
	}

	hwndCloud = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE,	//拓展的窗口风格
		"clouds",	//跟纸飞机用同一种窗口类
		"",	//窗口的标题
		WS_POPUP,	//设置窗口的样式参数
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	//窗口的左上角的X和Y坐标，宽度和高度
		NULL, NULL, hinstance, NULL	//父窗口句柄，菜单处理，应用程序实例句柄，窗口创建数据的指针
	);
	if (hwndCloud == NULL)
	{
		MessageBox(NULL, "窗口创建失败", "错误！", MB_ICONERROR | MB_OK);
		return -1;
	}
	if (!SetLayeredWindowAttributes(hwndCloud, RGB(255, 255, 255), 255, LWA_COLORKEY | LWA_ALPHA))
	{
		DWORD dwError = GetLastError();
	}
	ShowWindow(hwndCloud, SW_SHOW);
	UpdateWindow(hwndCloud);

	return 0;
}
LRESULT CALLBACK CloudsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//PAINTSTRUCT ps;
	//RECT rect;
	// 注意，是switch-case, 每次这个函数被调用，只会落入到一个case中。
	switch (msg)
	{
	case WM_CREATE:
		CloudsCreate(hwnd);
		break;
	case WM_PAINT:
		CloudsPaint(hwnd);
		break;
	case WM_KEYDOWN:
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_TIMER:
		FlysTimer(hwnd);
		CloudsPaint(hwnd);
		break;
	case WM_DESTROY:
		ExitProcess(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}
LONG CloudsCreate(HWND hwnd)
{
	hbmpCloud = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hbmpCloud == NULL)
	{
		MessageBox(hwnd, "bmp file not find", "ERROR!",
			MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}

	return 0;
}
LONG CloudsPaint(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	HDC hdc, mdc;
	BITMAP bmp;

	hdc = GetDC(hwnd);
	mdc = CreateCompatibleDC(hdc);
	GetObject(hbmpCloud, sizeof(BITMAP), &bmp);

	HBRUSH hbrBomb;	//爆炸效果
	hbrBomb = CreateSolidBrush(COLOR_BOMB);

	int num, i;
	num = get_flys_num();	//获得链表节点数量

	LPAUTO_FLY auto_fly;

	for (i = 0; i < num; i++)
	{
		auto_fly = get_fly_at(i);
		
		if (auto_fly->type == FLY_TYPE_CLOUD)
		{

			if (auto_fly->state == FLY_HIT)	//云朵被击中
			{
				SelectObject(mdc, hbrBomb);
				Ellipse(mdc,
					get_fly_x(auto_fly) - 25,
					get_fly_y(auto_fly) - 25,
					get_fly_x(auto_fly) + 25,
					get_fly_y(auto_fly) + 25);
			}
			else  //画云朵
			{
				SelectObject(mdc, hbmpCloud);
				TransparentBlt(hdc,
					ptPaperplane.x - PAPERPLANE_WIDTH, ptPaperplane.y - PAPERPLANE_HEIGHT / 2,
					PAPERPLANE_WIDTH, PAPERPLANE_HEIGHT,
					mdc,
					0, 0, bmp.bmWidth, bmp.bmHeight,
					RGB(255, 255, 255));
			}
		}

	}

	BitBlt(hdc,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		mdc,
		0, 0,
		SRCCOPY);

	DeleteDC(hdc);
	DeleteDC(mdc);
	ReleaseDC(hwnd, hdc);

	return 0;
}

LONG FlysTimer(HWND hwnd)
{
	destory_fly_by_state();	//如果链表节点（自动飞行物）出界或者被击中，销毁该节点
	flys_move_step();	//	判断并标记自动飞行物状态

	if (rand() % 1000 < 100) // 随机产生云朵
	{
		gen_cloud();
	}

	return 0;
}

LONG OnKeydown(HWND hwnd, UINT vk)
{
	UINT key = vk;

	switch (key)
	{
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case 'S':
		if (HIWORD(GetKeyState('S')))
		{
			gen_bullet(ptPaperplane.x, ptPaperplane.y);
			//PlaySound("C:\\Windows\\Media\\ir_end.wav", NULL, SND_FILENAME);
		}
		if (HIWORD(GetKeyState(VK_LEFT)))
		{
			ptPaperplane.x -= PAPERPLANE_MOVE_STEP;
			if (ptPaperplane.x < 0)
				ptPaperplane.x = 0;
		}
		else if (HIWORD(GetKeyState(VK_RIGHT)))
		{
			ptPaperplane.x += PAPERPLANE_MOVE_STEP;
			if (ptPaperplane.x > MAX_X)
				ptPaperplane.x = MAX_X;
		}
		else if (HIWORD(GetKeyState(VK_UP)))
		{
			ptPaperplane.y -= PAPERPLANE_MOVE_STEP;
			if (ptPaperplane.y < 0)
				ptPaperplane.y = 0;
		}
		else if (HIWORD(GetKeyState(VK_DOWN)))
		{
			ptPaperplane.y += PAPERPLANE_MOVE_STEP;
			if (ptPaperplane.y > MAX_Y)
				ptPaperplane.y = MAX_Y;
		}

		FlysPaint(hwndPaperPlane);
		break;
	default:
		break;
	}
	return 0;
}
