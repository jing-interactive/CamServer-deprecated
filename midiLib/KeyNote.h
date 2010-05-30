#pragma once

//midi对应的键位

#include <string.h>

namespace KeyNote{
	// Data for translating scan codes to octaves and notes
	// ----------------------------------------------------        
       
	struct KEY{
		//
		int _octave ;
		int _note ;
		int _yPos ;
		int _xPos ;

		TCHAR *  _name;        
	};
        
	static KEY g_keys[] = {
												  //   Scan    Char    Oct    Note
												  //   ----    ----    ---    ----
		-1,    -1,    1,     -1,    NULL,		  //     0      None
		-1,    -1,    -1,    -1,    NULL,		  //     1      Esc
		-1,    -1,    0,     0,     TEXT (""),    //     2      1
		5,     1,     0,     2,     TEXT ("C#"),  //     3      2      5      C#
		5,     3,     0,     4,     TEXT ("D#"),  //     4      3      5      D#
		-1,    -1,    0,     6,     TEXT (""),    //     5      4
		5,     6,     0,     8,     TEXT ("F#"),  //     6      5      5      F#
		5,     8,     0,     10,    TEXT ("G#"),  //     7      6      5      G#
		5,     10,    0,     12,    TEXT ("A#"),  //     8      7      5      A#
		-1,    -1,    0,     14,    TEXT (""),    //     9      8
		6,     1,     0,     16,    TEXT ("C#"),  //     10     9      6      C#
		6,     3,     0,     18,    TEXT ("D#"),  //     11     0      6      D#
		-1,    -1,    0,     20,    TEXT (""),    //     12     -
		6,     6,     0,     22,    TEXT ("F#"),  //     13     =      6      F#
		-1,    -1,    -1,    -1,    NULL,		  //     14     Back
		-1,    -1,    -1,    -1,    NULL,		  //     15     Tab
		5,     0,     1,     1,     TEXT ("C"),   //     16     q      5      C
		5,     2,     1,     3,     TEXT ("D"),   //     17     w      5      D
		5,     4,     1,     5,     TEXT ("E"),   //     18     e      5        E
		5,     5,     1,     7,     TEXT ("F"),   //     19     r      5       F
		5,     7,     1,     9,     TEXT ("G"),   //     20     t      5      G
		5,     9,     1,     11,    TEXT ("A"),   //     21     y      5        A
		5,     11,    1,     13,    TEXT ("B"),   //     22     u      5        B
		6,     0,     1,     15,    TEXT ("C"),   //     23     i      6      C
		6,     2,     1,     17,    TEXT ("D"),   //     24     o      6        D
		6,     4,     1,     19,    TEXT ("E"),   //     25     p      6        E
		6,     5,     1,     21,    TEXT ("F"),   //     26     [      6        F
		6,     7,     1,     23,    TEXT ("G"),   //     27     ]      6        G
		-1,    -1,    -1,    -1,    NULL,         //     28    Ent
		-1,    -1,    -1,    -1,    NULL,         //     29    Ctrl
		3,     8,     2,     2,     TEXT ("G#"),  //     30     a      3        G#
		3,     10,    2,     4,     TEXT ("A#"),  //     31     s      3        A#
		-1,    -1,    2,     6,     TEXT (""),    //     32     d
		4,     1,     2,     8,     TEXT ("C#"),  //     33     f      4        C#
		4,     3,     2,     10,    TEXT ("D#"),  //     34     g      4        D#
		-1,    -1,    2,     12,    TEXT (""),    //     35     h
		4,     6,     2,     14,    TEXT ("F#"),  //     36     j      4        F#
		4,     8,     2,     16,    TEXT ("G#"),  //     37     k      4        G#
		4,     10,    2,     18,    TEXT ("A#"),  //     38     l      4        A#
		-1,    -1,    2,     20,    TEXT (""),    //     39     ;
		5,     1,     2,     22,    TEXT ("C#"),  //     40     '      5        C#
		-1,    -1,    -1,    -1,    NULL,         //     41     `
		-1,    -1,    -1,    -1,    NULL,         //     42    Shift
		-1,    -1,    -1,    -1,    NULL,         //     43     \      (not line continuation)
		3,     9,     3,     3,     TEXT ("A"),   //     44     z      3        A
		3,     11,    3,     5,     TEXT ("B"),   //     45     x      3        B
		4,     0,     3,     7,     TEXT ("C"),   //     46     c      4        C
		4,     2,     3,     9,     TEXT ("D"),   //     47     v      4        D
		4,     4,     3,     11,    TEXT ("E"),   //     48     b      4        E
		4,     5,     3,     13,    TEXT ("F"),   //     49     n      4        F
		4,     7,     3,     15,    TEXT ("G"),   //     50     m      4        G
		4,     9,     3,     17,    TEXT ("A"),   //     51     ,      4        A
		4,     11,    3,     19,    TEXT ("B"),   //     52     .      4        B
		5,     0,     3,     21,    TEXT ("C")    //     53     /      5        C     
	};

	const int NUM_NOTES  = sizeof g_keys / sizeof g_keys[0];

	inline int totalNote(int oct, int note){
		//
		return (12 * oct + note);
	}

	static bool getOctNoteByName(IN TCHAR* noteName, OUT int& oct, OUT int& note){
		//
		for (int i = 0; i < NUM_NOTES; i++){
			//
			if (g_keys[i]._name && !wcscmp(noteName, g_keys[i]._name) ){
				//
				oct = g_keys[i]._octave;
				note = g_keys[i]._note;
				return(true);
			}
		}
		//got no result
		oct = -1;
		note = -1;
		return(false);
	}

	static int getNoteByName(TCHAR* noteName){
		//
		int oct = 0;
		int note = 0;
		for (int i = 0; i < NUM_NOTES; i++){
			//
			if (g_keys[i]._name && !wcscmp(noteName, g_keys[i]._name) ){
				//
				oct = g_keys[i]._octave;
				note = g_keys[i]._note;				
			}
		}
		return(totalNote(oct,note));
	}

};

