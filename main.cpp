  /*=================================================================}
 /  <<<<<<<<<<<<<<<<<<<<<--- Harsh Engine --->>>>>>>>>>>>>>>>>>>>>>  }
/  e-mail  : xproger@list.ru                                         }
{  github  : https://github.com/xproger/harsh                        }
{====================================================================}
{ LICENSE:                                                           }
{ Copyright (c) 2013, Timur "XProger" Gagiev                         }
{ All rights reserved.                                               }
{                                                                    }
{ Redistribution and use in source and binary forms, with or without /
{ modification, are permitted under the terms of the BSD License.   /
{=================================================================*/

#include <windows.h>
#include "core.h"
#include "render.h"

bool quit = false;

const int WIDTH	 = 800;
const int HEIGHT = 600;

const char * TITLE = "Harsh Engine";	
LARGE_INTEGER timeFreq, startTime;

int getTime() {
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return int(1000 * (time.QuadPart - startTime.QuadPart) / timeFreq.QuadPart);
}

InputKey convKey(int key) {
	if (key < 0 || key > 255) return IK_NONE;
	static const unsigned char keys[256] = {
		0,0,0,0,0,0,0,0, IK_BACK, IK_TAB, 0,0,0, IK_ENTER, 0,0,
		IK_SHIFT, IK_CTRL, IK_ALT, 0,0,0,0,0,0,0,0, IK_ESC, 0,0,0,0,
		IK_SPACE, IK_PGUP, IK_PGDOWN, IK_END, IK_HOME, IK_LEFT, IK_UP, IK_RIGHT, IK_DOWN, IK_INS, IK_DEL,
		IK_0, IK_1, IK_2, IK_3, IK_4, IK_5, IK_6, IK_7, IK_8, IK_9, IK_A, IK_B, IK_C, IK_D, IK_E, IK_F,
		IK_G, IK_H, IK_I, IK_J, IK_K, IK_L, IK_M, IK_N, IK_O, IK_P, IK_Q, IK_R, IK_S, IK_T, IK_U, IK_V,
		IK_W, IK_X, IK_Y, IK_Z, 0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0, IK_PLUS, IK_MINUS, 0,0,0,
		IK_TILDE, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	return (InputKey)keys[key];
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {		
		case WM_ACTIVATEAPP :
			if (short(wParam)) {
				Core::resume();
			} else {
				Core::pause();
			}
			break;
		case WM_DESTROY :
			quit = true; 
			break;
		case WM_SIZE :
			Render::resize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOUSEMOVE :
			Core::inputEvent( InputEvent(IS_MOVE, IK_MOUSE, wParam & MK_LBUTTON ? 0 : (wParam & MK_RBUTTON ? 1 : 2), LOWORD(lParam), HIWORD(lParam)) );
			break;
		case WM_LBUTTONDOWN :
		case WM_LBUTTONUP :
		case WM_LBUTTONDBLCLK :
			Core::inputEvent( InputEvent(message == WM_LBUTTONUP ? IS_UP : IS_DOWN, IK_MOUSE, 0, LOWORD(lParam), HIWORD(lParam)) );
			break;
		case WM_RBUTTONDOWN :
		case WM_RBUTTONUP :
		case WM_RBUTTONDBLCLK :
			Core::inputEvent( InputEvent(message == WM_RBUTTONUP ? IS_UP : IS_DOWN, IK_MOUSE, 1, LOWORD(lParam), HIWORD(lParam)) );
			break;
		case WM_MBUTTONDOWN :
		case WM_MBUTTONUP :
		case WM_MBUTTONDBLCLK :
			Core::inputEvent( InputEvent(message == WM_MBUTTONUP ? IS_UP : IS_DOWN, IK_MOUSE, 2, LOWORD(lParam), HIWORD(lParam)) );
			break;
		case WM_MOUSEWHEEL :
			Core::input->mouse.wheel += (wParam >> 16) / 120;
			break;			
		case WM_CHAR :
			Core::inputEvent( InputEvent((char)wParam) );
			break;
		case WM_KEYDOWN :
		case WM_KEYUP :
			Core::inputEvent( InputEvent(message == WM_KEYUP ? IS_UP : IS_DOWN, convKey(wParam)) );
			break;
		default:
			return DefWindowProcA(hWnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char *argv[]) {
	const long style = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	RECT r;
	SetRect(&r, 0, 0, WIDTH, HEIGHT);
	AdjustWindowRect(&r, style, false);
	HWND handle = CreateWindowA("STATIC", TITLE, style, 0, 0, r.right - r.left, r.bottom - r.top, NULL, NULL, NULL, NULL);
	SetWindowLongA(handle, GWL_WNDPROC, LONG(&WndProc));

	HDC dc = GetDC(handle);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize			= sizeof(pfd);
	pfd.nVersion		= 1;
	pfd.dwFlags			= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits		= 32;
	pfd.cAlphaBits		= 8;
	pfd.cDepthBits		= 24;
	pfd.cStencilBits	= 8;

	SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
	HGLRC rc = wglCreateContext(dc);
	wglMakeCurrent(dc, rc);

	QueryPerformanceFrequency(&timeFreq);
	QueryPerformanceCounter(&startTime);

	Render::resize(WIDTH, HEIGHT);
	Core::init("data.jet", getTime);
	Core::reset();
	Core::resume();
	Core::resize(WIDTH, HEIGHT);
	Core::update();

	MSG msg;
	while (!quit)
		if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		} else {		
			Core::update();		
			Core::render();
			SwapBuffers(dc);
		}
	Core::deinit();



	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rc);
	ReleaseDC(handle, dc);
	DestroyWindow(handle);

	ExitProcess(0);
	return 0;
}
