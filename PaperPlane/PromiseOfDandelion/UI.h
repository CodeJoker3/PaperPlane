#pragma once
#include<Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
//±³¾°
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LONG BackgroundCreate(HWND hwnd);
LONG BackgroundPaint(HWND hwnd);
LONG BackgroundResizeAndMove(HWND hwnd, LPRECT lpRect);
//Ö½·É»úºÍ×Óµ¯
LONG PaperplaneWindowCreate(HINSTANCE hinstance);
LRESULT CALLBACK FlysProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LONG FlysCreate(HWND hwnd);
LONG FlysPaint(HWND hwnd);
//ÔÆ¶ä
LONG CloudWindowCreate(HINSTANCE hinstance);
LRESULT CALLBACK CloudsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LONG CloudsCreate(HWND hwnd);
LONG CloudsPaint(HWND hwnd);

LONG FlysTimer(HWND hwnd);

LONG OnKeydown(HWND hwnd, UINT vk);