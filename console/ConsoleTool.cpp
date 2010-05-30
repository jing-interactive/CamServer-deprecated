#include "ConsoleTool.h"


bool isPrevKeyDown[256];


bool isKeyToggle(int vk, bool* keyStatus){
    //
	if (KEY_DOWN((vk)) && !(isPrevKeyDown[vk])){
		(isPrevKeyDown[vk]) = true;
		if (keyStatus != NULL)
			*keyStatus =! *keyStatus;//key status needs update
		return true;
	}
	if (!KEY_DOWN((vk))){
		isPrevKeyDown[vk] = false;
	}
	return false;
}

void playWave(char* filename)
{
	::PlaySound(filename,NULL,SND_FILENAME|SND_ASYNC);
}

int Console::messageBox(IN LPCSTR lpText, IN UINT uType)
{
	return ::MessageBox(_hWnd, lpText, _title, uType);
}


HWND Console::getHWnd()
{
	return _hWnd;
}

Console::Console(int width, int height, char* title){
	//
//	_hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	_hIn = ::GetStdHandle(STD_INPUT_HANDLE);
	
	_width = width;
	_height = height;

	setTitle(title);	

	_hWnd = getConsoleWindowHandle();

    _hOut=CreateConsoleScreenBuffer(GENERIC_WRITE,0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    SetConsoleActiveScreenBuffer(_hOut);
    COORD size;
    size.X=width;
    size.Y=height;
    SetConsoleScreenBufferSize(_hOut,size);
    // Disable processing of data & wrapping: 
    SetConsoleMode(_hOut,0);
	_char_infos = new CHAR_INFO[_height*_width];
	_screen_buffer = new short[_height*_width];
	memset(_screen_buffer, 0, sizeof(short)*_height*_width);

    CONSOLE_SCREEN_BUFFER_INFO SBInfo;

    GetConsoleScreenBufferInfo(_hOut,
                               &SBInfo);
 
	_size = SBInfo.dwSize;

} 

Console::~Console()
{
	delete[] _char_infos;
	delete[] _screen_buffer;
}

void Console::setSize(WORD w, WORD h)
{
	_size.X = w;
	_size.Y = h;

	SMALL_RECT DisplayArea = {0, 0, w-1, h-1};

    SetConsoleWindowInfo(_hOut,
                         TRUE,
                         &DisplayArea);

}

HWND Console::getConsoleWindowHandle()
{
  char title[512];
  HWND hWnd;
 
  GetConsoleTitle(title, sizeof(title));
  hWnd = FindWindow(NULL, title);

  return(hWnd);
}

void Console::setTitle(char* title){
	//
	SetConsoleTitle(title);

	strcpy(_title, title);
}

void Console::locate(COORD coord)
{
	SetConsoleCursorPosition(_hOut, coord);
}

void Console::locate(int x, int y){
	//
    COORD crspos; /* cursor position */

    crspos.X = (short)x;
    crspos.Y = (short)y;
    SetConsoleCursorPosition(_hOut, crspos);
}


void Console::setForeColor(int ForeColor)
{
	_fore_color = ForeColor;
	::SetConsoleTextAttribute(_hOut,_fore_color|_back_color);	
}

void Console::setBackColor(int BackColor)
{
	_back_color = BackColor;
	::SetConsoleTextAttribute(_hOut,_fore_color|_back_color);
}

void Console::clearScreen(){
	//
	//system("cls");
	memset(_char_infos, 0, sizeof(CHAR_INFO)*_width*_height);
}


void Console::setCursorInfo(bool show, int size/* = 50*/)
{
	//
    CONSOLE_CURSOR_INFO ConCurInf;    

    ConCurInf.dwSize = size;
    ConCurInf.bVisible = show;

    SetConsoleCursorInfo(_hOut, &ConCurInf);
}


char Console::getCharAt(int x, int y)
{
	COORD Where;
    DWORD NumRead;
	char Letter;

    Where.X = x;
    Where.Y = y;
    ReadConsoleOutputCharacter(_hOut,
                               &Letter,
                               1,
                               Where,
                               &NumRead);

	return Letter;
}

bool Console::updateEvent(INPUT_RECORD& input)
{
    DWORD NumRead;
    return ReadConsoleInput(_hIn,
                     &input,
                     1,
                     &NumRead);
	
}


void Console::fillWith(int x,int y,int w,int h, char* icon)
{
	for (int i=0;i<w;i++)
	{
		for (int j=0;j<h;j++)
		{
			locate(x+i*2,y+j);
			printf(icon);
		}
	}
}



void Console::displayBuffer()
{
    // Convert textmode screen map to CHAR_INFO array
//     for (int i=0;i<_height*_width;i++)
//     {
//         _char_infos[i].Char.UnicodeChar=_screen_buffer[i]&0xff;
//         _char_infos[i].Attributes=(_screen_buffer[i]>>8)&0xff;
//     }

    // set up size structs
    COORD max,src;
		max.X=_width;
		max.Y=_height;
		src.X=0;
		src.Y=0;
    SMALL_RECT outrect;
		outrect.Top=0;
		outrect.Left=0;
		outrect.Right=_width-1;
		outrect.Bottom=_height-1;

    // write result to the console
    WriteConsoleOutput(_hOut,_char_infos,max,src,&outrect);
    // try to hide the cursor:
    CONSOLE_CURSOR_INFO cci;
    cci.bVisible=FALSE;
    cci.dwSize=1;   
    SetConsoleCursorInfo(_hOut,&cci);
}
