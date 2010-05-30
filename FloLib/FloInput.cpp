#include "FloInput.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

CInput::CInput(HWND hwnd, HINSTANCE hinstance, 
			   bool use_keyboard, bool use_mouse, bool use_joystick){
	//
	m_lpdi = NULL;       // dinput object
	m_lpdikey = NULL;    // dinput keyboard
	m_lpdimouse = NULL;  // dinput mouse
	m_lpdijoy = NULL;    // dinput joystick 

	memset(m_keyboard,0,sizeof(m_keyboard));
	memset(m_keyboard_prev,0,sizeof(m_keyboard_prev));
 	memset(m_keyboard_hit,0,sizeof(m_keyboard_hit));

	Init(hwnd, hinstance, use_keyboard,use_mouse,use_joystick);
}

int CInput::Init(HWND hwnd, HINSTANCE hinstance,
				 bool use_keyboard, bool use_mouse, bool use_joystick){
	//
	m_hWnd = hwnd;
	if (FAILED(DirectInput8Create(hinstance, DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (void **)&m_lpdi,NULL))){
		//
		return(0);
	   } 

	if (use_mouse){	
			
		// create a mouse device 
		if (m_lpdi->CreateDevice(GUID_SysMouse, &m_lpdimouse, NULL)!=DI_OK)
		   return(0);

	
		// set cooperation level
		// change to EXCLUSIVE FORGROUND for better control
		if (m_lpdimouse->SetCooperativeLevel(hwnd, 
							   DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)!=DI_OK)
		   return(0);

		// set data format
		if (m_lpdimouse->SetDataFormat(&c_dfDIMouse)!=DI_OK)
		   return(0);

		// acquire the mouse
		if (m_lpdimouse->Acquire()!=DI_OK)
		   return(0);

		m_mouse_x_old = m_mouse_x = 0;
		m_mouse_y_old = m_mouse_y = 0;

		m_left_click = false;
		m_right_click = false;
		m_left_down_prev = false;
		m_right_down_prev = false;
		m_left_down = false;
		m_right_down = false;
	}

	if (use_keyboard){
		// create the keyboard device  
		if (m_lpdi->CreateDevice(GUID_SysKeyboard, &m_lpdikey, NULL)!=DI_OK)
		   return(0);

		// set cooperation level
		if (m_lpdikey->SetCooperativeLevel(hwnd, 
						 DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)!=DI_OK)
			return(0);

		// set data format
		if (m_lpdikey->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
		   return(0);

		// acquire the keyboard
		if (m_lpdikey->Acquire()!=DI_OK)
		   return(0);
	}

	if (use_joystick){
		//Î´ÊµÏÖ
	}


	return(1);
}

int CInput::Update(void){
	//	
	HRESULT hr;
	
	m_mouse_x_old = m_mouse_x;
	m_mouse_y_old = m_mouse_y;

	if (m_lpdimouse){
		if (m_lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouse)!=DI_OK)
			return(0);		

		m_left_down = m_mouse.rgbButtons[0] & 0x80;
		m_right_down = m_mouse.rgbButtons[1] & 0x80;	
		
		m_left_click = m_left_down_prev && !m_left_down;//one-click = press-down then up
		m_right_click = m_right_down_prev && !m_right_down;

		m_left_down_prev = m_left_down;
		m_right_down_prev = m_right_down;

	}
	else{
		// mouse isn't plugged in, zero out st ate
		memset(&m_mouse,0,sizeof(m_mouse));		

		// return error
		return(0);
	}

	if (m_lpdikey){
		//
		hr = m_lpdikey->GetDeviceState(256, (void*)m_keyboard);
	
		if( FAILED(hr) ){
			hr = m_lpdikey->Acquire();
			while( hr == DIERR_INPUTLOST ) 
				hr = m_lpdikey->Acquire();
			return (1);
		}
		
		for (int i=0;i<256;i++)
		{
			//one-hit = press-down then up
			m_keyboard_hit[i] = m_keyboard_prev[i] && !m_keyboard[i];
		}

		memcpy((void*)m_keyboard_prev, (void*)m_keyboard, 256);

	}
	else{
		// keyboard isn't plugged in, zero out state
		memset(m_keyboard,0,sizeof(m_keyboard));

		// return error
		return(0);
	}

	if (m_lpdijoy){
		//not finished yet
	}



	m_mouse_x+= m_mouse.lX;
	m_mouse_y+= m_mouse.lY;

/*		if (m_mouse_x >= m_width)
			m_mouse_x = m_width-1;
		else if (m_mouse_x < 0) 
			m_mouse_x = 0;

		if (m_mouse_y >= m_height) 
			m_mouse_y = m_height-1;
		else if (m_mouse_y < 0) 
			m_mouse_y = 0;*/

	::GetCursorPos(&_windowed_cursor);
	::ScreenToClient(m_hWnd, &_windowed_cursor);

	return(1);
}


CInput::~CInput(){
	//
	if (m_lpdijoy){    
		m_lpdijoy->Unacquire();
		m_lpdijoy->Release();
    }
	
	if (m_lpdimouse){    
		m_lpdimouse->Unacquire();
		m_lpdimouse->Release();
    }

	if (m_lpdikey){
		m_lpdikey->Unacquire();
		m_lpdikey->Release();
    } 
}