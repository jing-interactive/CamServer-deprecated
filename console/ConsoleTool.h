#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


extern bool isPrevKeyDown[256];
	
bool isKeyToggle(int vk, bool* keyStatus = NULL);

void playWave(char* filename);

#ifndef SHORTCOLOURS_H
#define SHORTCOLOURS_H

#define FW FOREGROUND_RED | \
           FOREGROUND_GREEN | \
           FOREGROUND_BLUE
#define FR FOREGROUND_RED
#define FG FOREGROUND_GREEN
#define FB FOREGROUND_BLUE
#define FY FOREGROUND_RED | \
           FOREGROUND_GREEN
#define FC FOREGROUND_GREEN | \
           FOREGROUND_BLUE
#define FM FOREGROUND_BLUE | \
           FOREGROUND_RED 
#define FWI FOREGROUND_RED | \
            FOREGROUND_GREEN | \
            FOREGROUND_BLUE | \
            FOREGROUND_INTENSITY
#define FRI FOREGROUND_RED | \
            FOREGROUND_INTENSITY
#define FGI FOREGROUND_GREEN | \
            FOREGROUND_INTENSITY
#define FBI FOREGROUND_BLUE | \
            FOREGROUND_INTENSITY
#define FYI FOREGROUND_RED | \
            FOREGROUND_GREEN | \
            FOREGROUND_INTENSITY
#define FCI FOREGROUND_GREEN | \
            FOREGROUND_BLUE | \
            FOREGROUND_INTENSITY
#define FMI FOREGROUND_BLUE | \
            FOREGROUND_RED | \
            FOREGROUND_INTENSITY 
#define FNULL 0
 
#define BW BACKGROUND_RED | \
           BACKGROUND_GREEN | \
           BACKGROUND_BLUE
#define BR BACKGROUND_RED
#define BG BACKGROUND_GREEN
#define BB BACKGROUND_BLUE
#define BY BACKGROUND_RED | \
           BACKGROUND_GREEN
#define BC BACKGROUND_GREEN | \
           BACKGROUND_BLUE
#define BM BACKGROUND_BLUE | \
           BACKGROUND_RED 
#define BWI BACKGROUND_RED | \
            BACKGROUND_GREEN | \
            BACKGROUND_BLUE | \
            BACKGROUND_INTENSITY
#define BRI BACKGROUND_RED | \
            BACKGROUND_INTENSITY
#define BGI BACKGROUND_GREEN | \
            BACKGROUND_INTENSITY
#define BBI BACKGROUND_BLUE | \
            BACKGROUND_INTENSITY
#define BYI BACKGROUND_RED | \
            BACKGROUND_GREEN | \
            BACKGROUND_INTENSITY
#define BCI BACKGROUND_GREEN | \
            BACKGROUND_BLUE | \
            BACKGROUND_INTENSITY
#define BMI BACKGROUND_BLUE | \
            BACKGROUND_RED | \
            BACKGROUND_INTENSITY 
#define BNULL 0

#endif


// #define FOREGROUND_BLUE      0x0001 // text color contains blue.
// #define FOREGROUND_GREEN     0x0002 // text color contains green.
// #define FOREGROUND_RED       0x0004 // text color contains red.
// #define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
// #define BACKGROUND_BLUE      0x0010 // background color contains blue.
// #define BACKGROUND_GREEN     0x0020 // background color contains green.
// #define BACKGROUND_RED       0x0040 // background color contains red.
// #define BACKGROUND_INTENSITY 0x0080 // background color is intensified.

enum e_ForeColor
{
	fBlue = 0x0001,
	fGreen = 0x0002,
	fRed = 0x0004,
	fDeep = 0x0008,
	fWhite = fBlue | fGreen | fRed,

	fDeepBlue = fDeep | fBlue,
	fDeepGreen = fDeep | fGreen,
	fDeepRed = fDeep | fRed,
	fDeepWhite = fDeep | fWhite,
};

enum e_BackColor
{
	bBlue = 0x0010,
	bGreen = 0x0020,
	bRed = 0x0040,
	bDeep = 0x0080,

	bWhite = bBlue | bGreen | bRed,

	bDeepBlue = bDeep | bBlue,
	bDeepGreen = bDeep | bGreen,
	bDeepRed = bDeep | bRed,
	bDeepWhite = bDeep | bWhite,
};

class Console{
	//
public:
	Console(int width, int height, char* title);
	~Console();
	void setTitle(char*);
	void setSize(WORD w, WORD h);

	void locate(int x, int y);
	void locate(COORD coord);

	void setForeColor(int ForeColor);
	void setBackColor(int BackColor);
	void fillWith(int x,int y,int w,int h, char* icon);

	void clearScreen();
	void setCursorInfo(bool show, int size = 50);

	HWND getHWnd();
	int messageBox(IN LPCSTR lpText, IN UINT uType = MB_OK);

	char getCharAt(int x, int y);

	bool updateEvent(INPUT_RECORD& input);

	void displayBuffer();

	void setChar(int x, int y, char c, short color)
	{
		_char_infos[y*_width+x].Char.AsciiChar = c;
		_char_infos[y*_width+x].Attributes = color;
	}
	void setUnicodeChar(int x, int y, char* c, short color)
	{
		_char_infos[y*_width+x].Char.UnicodeChar = c[0];
		_char_infos[y*_width+x].Attributes = color;
		_char_infos[y*_width+x+1].Char.UnicodeChar = c[1];
		_char_infos[y*_width+x+1].Attributes = color;
	}
private:

	HWND getConsoleWindowHandle();

private:
	
	HANDLE _hOut;
	HANDLE _hIn;
	HWND   _hWnd;
	char	_title[MAX_PATH];

	int _fore_color;
	int _back_color;

	int _width;
	int _height;

	COORD _size;

	CHAR_INFO* _char_infos;
	short* _screen_buffer;	
};
