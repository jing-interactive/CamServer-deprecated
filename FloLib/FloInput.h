//updated by vinjn @ 2006-4-22
#ifndef FLO_INPUT
#define FLO_INPUT

#pragma once

#define DIRECTINPUT_VERSION  0x0800

#include <WINDOWS.H>
#include <dinput.h>

class CInput{
	//

public:
	CInput(HWND hwnd, HINSTANCE hinstance, 
		bool use_keyboard = true, bool use_mouse =true , bool use_joystick = false);
	~CInput();
	int Init(HWND hwnd, HINSTANCE hinstance, 
		bool use_keyboard, bool use_mouse, bool use_joystick);
	int Update(void);
	int GetFullScreenMouseX(void) { return m_mouse_x;}
	int GetFullScreenMouseY(void) { return m_mouse_y;}
	bool LeftDown(void)  { return m_left_down;}
	bool RightDown(void)  { return m_right_down;}
	bool LeftClicked(void)  { return m_left_click;}
	bool RighClicked(void)  { return m_right_click;}

	bool KeyDown(int key) { return m_keyboard[key];}
	bool KeyHit(int key) { return m_keyboard_hit[key];}//hit @ keyboard equals click @ mouse
//	bool KeyUp(int key) { return m_keyboard[key]&0x80==0;}
	int GetMouseX(void){
		return _windowed_cursor.x;
	}
	int GetMouseY(void){
		return _windowed_cursor.y;
	}

	int GetMouseDeltaX(void)  {return m_mouse.lX;}
	int GetMouseDeltaY(void)  {return m_mouse.lY;}


private:
	UCHAR m_keyboard[256]; // contains keyboard state table
	UCHAR m_keyboard_prev[256]; // contains keyboard state table
	bool m_keyboard_hit[256]; // contains keyboard state table

	DIMOUSESTATE m_mouse;  // contains state of mouse

	HWND m_hWnd;
	POINT _windowed_cursor;
	int m_mouse_x;
	int m_mouse_y;
	int m_mouse_x_old;
	int m_mouse_y_old;

	bool m_left_down;
	bool m_right_down;
	bool m_left_down_prev;
	bool m_right_down_prev;
	bool m_left_click;
	bool m_right_click;

	LPDIRECTINPUT8       m_lpdi;       // dinput object
	LPDIRECTINPUTDEVICE8 m_lpdikey;    // dinput keyboard
	LPDIRECTINPUTDEVICE8 m_lpdimouse;  // dinput mouse
	LPDIRECTINPUTDEVICE8 m_lpdijoy;    // dinput joystick 
	GUID                 m_joystickGUID; // guid for main joystick
	char                 m_joyname[80];  // name of joystick
	DIJOYSTATE joy_state;      // contains state of joystick
	int joystick_found;        // tracks if stick is plugged in
};


#endif
