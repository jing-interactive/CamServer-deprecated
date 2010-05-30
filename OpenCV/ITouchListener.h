#ifndef __TOUCHLIB_ITOUCHLISTENER__
#define __TOUCHLIB_ITOUCHLISTENER__

#include <TouchData.h>
#include <touchlib_platform.h>


namespace touchlib 
{
	//! Clients should implement this class
	class TOUCHLIB_EXPORT ITouchListener
	{
	public:
		//! Notify that a finger has just been made active. 
		virtual void fingerDown(TouchData data) = 0;

		//! Notify that a finger has just been made active. 
		virtual void fingerUpdate(TouchData data) = 0;

		//! A finger is no longer active..
		virtual void fingerUp(TouchData data) = 0;
	};
}

#endif  // __TOUCHLIB_ITOUCHLISTENER__
