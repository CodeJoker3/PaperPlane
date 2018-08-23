#include<Windows.h>
#include "resource.h"
#include "fly.h"
#include "UI.h"
#include "LinkList.h"
//*********************************************************
#define TIMER_ID 12340
#define PAPERPLANE_HEIGHT 50	//ֽ�ɻ��ĸ߶�
#define PAPERPLANE_WIDTH 50		//ֽ�ɻ��Ŀ��
#define CLOUD_WIDTH 50	//�ƵĿ��
#define CLOUD_HEIGHT 60	//�Ƶĸ߶�

#define PAPERPLANE_MOVE_STEP 8	//ֽ�ɻ�ÿ���ƶ�������

#define COLOR_BULLET RGB(124,252,0)	//�ӵ���ɫ
#define COLOR_BOMB RGB(255,0,0)	//��ը��Ч��ɫ
#define COLOR_CLOUD RGB(0,255,0)

POINT ptPaperplane;
DWORD dwTimerElapse = 40;	//ʱ�䵥λ�������λ����

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
	WNDCLASSEX wc;	//����������
	MSG msg;	//��Ϣ���

	//ע�ᴰ����
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;	//���ڲ˵�����Դ��
	wc.lpszClassName = "myWinClass";	//�����������
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//��鴰����
	if (!RegisterClassEx(&wc))
	{
		GetLastError();
		MessageBox(NULL, "��������CLASSʧ��", "����", MB_ICONERROR | MB_OK);
		return -1;
	}
	//����ע��ɹ�
	//����������
	hwndBackground = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST,	//��չ�Ĵ��ڷ��
		"myWinClass",	//��������������ϵͳҪ�����������͵Ĵ���
		"",	//���ڵı���
		WS_POPUP,	//���ô��ڵ���ʽ����
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	//���ڵ����Ͻǵ�X��Y���꣬��Ⱥ͸߶�
		NULL, NULL, hinstance, NULL	//�����ھ�����˵�����Ӧ�ó���ʵ����������ڴ������ݵ�ָ��
	);
	if (hwndBackground == NULL)
	{
		MessageBox(NULL, "���ڴ���ʧ��", "����", MB_ICONERROR | MB_OK);
		return -1;
	}
	//���ڴ����ɹ�����������

	if (!SetLayeredWindowAttributes(
		hwndBackground, RGB(255,255,255),
		250, LWA_ALPHA))
	{
		DWORD dwError = GetLastError();
	}

	//��ʾ����
	ShowWindow(hwndBackground, SW_SHOW);
	//ˢ�´��ڣ�ʹ���ڽ����ػ�
	UpdateWindow(hwndBackground);
	

	//��Ϣѭ��
	while ( GetMessage(&msg, NULL, 0, 0) !=0|| GetMessage(&msg, NULL, 0, 0) != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//����
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	switch (msg)
	{
	case WM_CREATE:
		BackgroundCreate(hwnd);	//��ʼ������
		SetFocus(hwnd);	  //�����̽�������Ϊ������
		PostMessage(hwnd, WM_LBUTTONDOWN, 0, (DWORD)0x0050050);
		PostMessage(hwnd, WM_LBUTTONUP, 0, (DWORD)0x0050050);
		break;
	// ����ʹ��GDI�ڴ����ϻ���ͼ�εĳ��� �����롱 д�����
	// ���������WM_PAINT��Ϣ�Ĵ�������л���GDIͼ�Σ���ô�ڴ���ˢ��ʱ�ͻᱻ�±�Ĩ���͸���
	case WM_PAINT:
		BackgroundPaint(hwnd);	
		break;
	case WM_MOVING:
		BackgroundResizeAndMove(hwnd,(LPRECT)lParam);	//�������ڴ�С��λ��
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

	MoveWindow(hwnd, 100, 100, MAX_X, MAX_Y, TRUE);	//�ı䴰��λ�úͳߴ�

	//����ֽ�ɻ��ĳ�ʼλ��
	ptPaperplane.x = MAX_X-PAPERPLANE_WIDTH;
	ptPaperplane.y = MAX_Y / 2;

	SetFocus(hwnd);

	return 0;
}
LONG BackgroundPaint(HWND hwnd)
{
	HDC hdc, mdc;
	RECT rect;	//RECT������һ���ṹ�����left��top��right��bottom���ͻ�����

	BITMAP bmp;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);	//����ȡ��ָ�����ڵĿͻ������С

								//ÿ����������ɷ�Ϊ�����֣�������ʾ���ݵ������Client�����ͻ�������
								//NonClient���ǿͻ�����������ʾ���ť�����ڱ���ȡ����ͻ�����Ϣ����WM_PAINT�����ģ����ǿͻ����Ļ�ͼ����WM_NCPAINT������

	mdc = CreateCompatibleDC(hdc);
	SelectObject(mdc, hbmpBackground);	//��λͼˢ�Ž�������

	GetObject(hbmpBackground, sizeof(BITMAP), &bmp);	//�ú����õ�ָ��ͼ�ζ������Ϣ��
														//����ͼ�ζ��󣬺����������Ļ�ṹ�����������߼���ɫ�壩��Ŀ����һ��ָ���Ļ�����,&bmpָ�򻺳���

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

//ֽ�ɻ����ӵ�
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
	wc.lpszMenuName = NULL;	//���ڲ˵�����Դ��
	wc.lpszClassName = "flys";	//�����������
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		GetLastError();
		MessageBox(NULL, "��������CLASSʧ��", "����", MB_ICONERROR | MB_OK);
		return -1;
	}
	hwndPaperPlane = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE,	//��չ�Ĵ��ڷ��
		"flys",	//��������������ϵͳҪ�����������͵Ĵ���
		"",	//���ڵı���
		WS_POPUP,	//���ô��ڵ���ʽ����
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	//���ڵ����Ͻǵ�X��Y���꣬��Ⱥ͸߶�
		NULL, NULL, hinstance, NULL	//�����ھ�����˵�����Ӧ�ó���ʵ����������ڴ������ݵ�ָ��
	);
	if (hwndPaperPlane == NULL)
	{
		MessageBox(NULL, "���ڴ���ʧ��", "����", MB_ICONERROR | MB_OK);
		return -1;
	}
	//���ô���͸����    
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

	//��ֽ�ɻ�
	GetClientRect(hwnd, &rect);

	hdc = GetDC(hwnd);
	mdc = CreateCompatibleDC(hdc);
	mdc_ = CreateCompatibleDC(hdc);

	hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);

	SelectObject(mdc, hBitmap);
	SelectObject(mdc, hbmpPaperPlane);

	GetObject(hbmpPaperPlane, sizeof(BITMAP), &bmp);

	FillRect(mdc, &rect, (HBRUSH)GetStockObject(NULL_BRUSH));	//������

	TransparentBlt(mdc,
		ptPaperplane.x-PAPERPLANE_WIDTH,ptPaperplane.y - PAPERPLANE_HEIGHT / 2,
		PAPERPLANE_WIDTH, PAPERPLANE_HEIGHT,
		mdc_,
		0, 0, bmp.bmWidth, bmp.bmHeight,
		RGB(255,255,255));

	//���ӵ�
	HBRUSH hbrBullet;

	hbrBullet = CreateSolidBrush(COLOR_BULLET);

	int num,i;
	num = get_flys_num();	//�������ڵ�����

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



	//������DC
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

//�ƶ�
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
	wc.lpszMenuName = NULL;	//���ڲ˵�����Դ��
	wc.lpszClassName = "clouds";	//�����������
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		GetLastError();
		MessageBox(NULL, "��������CLASSʧ��", "����", MB_ICONERROR | MB_OK);
		return -1;
	}

	hwndCloud = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE,	//��չ�Ĵ��ڷ��
		"clouds",	//��ֽ�ɻ���ͬһ�ִ�����
		"",	//���ڵı���
		WS_POPUP,	//���ô��ڵ���ʽ����
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,	//���ڵ����Ͻǵ�X��Y���꣬��Ⱥ͸߶�
		NULL, NULL, hinstance, NULL	//�����ھ�����˵�����Ӧ�ó���ʵ����������ڴ������ݵ�ָ��
	);
	if (hwndCloud == NULL)
	{
		MessageBox(NULL, "���ڴ���ʧ��", "����", MB_ICONERROR | MB_OK);
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
	// ע�⣬��switch-case, ÿ��������������ã�ֻ�����뵽һ��case�С�
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

	HBRUSH hbrBomb;	//��ըЧ��
	hbrBomb = CreateSolidBrush(COLOR_BOMB);

	int num, i;
	num = get_flys_num();	//�������ڵ�����

	LPAUTO_FLY auto_fly;

	for (i = 0; i < num; i++)
	{
		auto_fly = get_fly_at(i);
		
		if (auto_fly->type == FLY_TYPE_CLOUD)
		{

			if (auto_fly->state == FLY_HIT)	//�ƶ䱻����
			{
				SelectObject(mdc, hbrBomb);
				Ellipse(mdc,
					get_fly_x(auto_fly) - 25,
					get_fly_y(auto_fly) - 25,
					get_fly_x(auto_fly) + 25,
					get_fly_y(auto_fly) + 25);
			}
			else  //���ƶ�
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
	destory_fly_by_state();	//�������ڵ㣨�Զ������������߱����У����ٸýڵ�
	flys_move_step();	//	�жϲ�����Զ�������״̬

	if (rand() % 1000 < 100) // ��������ƶ�
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
