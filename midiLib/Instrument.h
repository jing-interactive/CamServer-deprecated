#pragma once

#include <windows.h>

namespace Instrument{
	//
	struct INST{
		//        
		TCHAR * _instrument_name ;
		int    _index ;    
	};


	#define  NUM_INTST_PER_FAMILY 8
	#define  NUM_FAMILYS 16


	struct FAMILY{
		//        
		TCHAR      * _family_name ;
		INST   _instruments[NUM_INTST_PER_FAMILY];       
	};


	static FAMILY g_fams[NUM_FAMILYS] = {
		//
		TEXT ("Piano"),
		TEXT ("Acoustic Grand Piano"), 0,
		TEXT ("Bright Acoustic Piano"),1,
		TEXT ("Electric Grand Piano"), 2,
		TEXT ("Honky-tonk Piano"),     3,
		TEXT ("Rhodes Piano"),         4,
		TEXT ("Chorused Piano"),       5,
		TEXT ("Harpsichord"),          6,
		TEXT ("Clavinet"),             7,

		TEXT ("Chromatic Percussion"),
		TEXT ("Celesta"),              8,
		TEXT ("Glockenspiel"),         9,
		TEXT ("Music Box"),          10,
		TEXT ("Vibraphone"),          11,
		TEXT ("Marimba"),             12,
		TEXT ("Xylophone"),           13,
		TEXT ("Tubular Bells"),       14,
		TEXT ("Dulcimer"),            15,

		TEXT ("Organ"),
		TEXT ("Hammond Organ"),       16,
		TEXT ("Percussive Organ"),    17,
		TEXT ("Rock Organ"),          18,
		TEXT ("Church Organ"),        19,
		TEXT ("Reed Organ"),          20,
		TEXT ("Accordian"),           21,
		TEXT ("Harmonica"),           22,
		TEXT ("Tango Accordian"),     23,

		TEXT ("Guitar"),
		TEXT ("Acoustic Guitar (nylon)"), 24,
		TEXT ("Acoustic Guitar (steel)"), 25,
		TEXT ("Electric Guitar (jazz)"),  26,
		TEXT ("Electric Guitar (clean)"), 27,
		TEXT ("Electric Guitar (muted)"), 28,
		TEXT ("Overdriven Guitar"),       29,
		TEXT ("Distortion Guitar"),       30,
		TEXT ("Guitar Harmonics"),        31,

		TEXT ("Bass"),
		TEXT ("Acoustic Bass"),           32,
		TEXT ("Electric Bass (finger)"),  33,
		TEXT ("Electric Bass (pick)"),    34,
		TEXT ("Fretless Bass"),           35,
		TEXT ("Slap Bass 1"),             36,
		TEXT ("Slap Bass 2"),             37,
		TEXT ("Synth Bass 1"),            38,
		TEXT ("Synth Bass 2"),            39,

		TEXT ("Strings"),
		TEXT ("Violin"),                 40,
		TEXT ("Viola"),                   41,
		TEXT ("Cello"),                   42,
		TEXT ("Contrabass"),              43,
		TEXT ("Tremolo Strings"),         44,
		TEXT ("Pizzicato Strings"),       45,
		TEXT ("Orchestral Harp"),         46,
		TEXT ("Timpani"),                 47,

		TEXT ("Ensemble"),
		TEXT ("String Ensemble 1"),       48,
		TEXT ("String Ensemble 2"),       49,
		TEXT ("Synth Strings 1"),        50,
		TEXT ("Synth Strings 2"),         51,
		TEXT ("Choir Aahs"),              52,
		TEXT ("Voice Oohs"),              53,
		TEXT ("Synth Voice"),             54,
		TEXT ("Orchestra Hit"),           55,

		TEXT ("Brass"),
		TEXT ("Trumpet"),                 56,
		TEXT ("Trombone"),                57,
		TEXT ("Tuba"),                    58,
		TEXT ("Muted Trumpet"),          59,
		TEXT ("French Horn"),             60,
		TEXT ("Brass Section"),           61,
		TEXT ("Synth Brass 1"),           62,
		TEXT ("Synth Brass 2"),           63,

		TEXT ("Reed"),
		TEXT ("Soprano Sax"),             64,
		TEXT ("Alto Sax"),                65,
		TEXT ("Tenor Sax"),               66,
		TEXT ("Baritone Sax"),            67,
		TEXT ("Oboe"),                    68,
		TEXT ("English Horn"),            69,
		TEXT ("Bassoon"),                 70,
		TEXT ("Clarinet"),                71,

		TEXT ("Pipe"),
		TEXT ("Piccolo"),                 72,
		TEXT ("Flute "),                  73,
		TEXT ("Recorder"),                74,
		TEXT ("Pan Flute"),               75,
		TEXT ("Bottle Blow"),             76,
		TEXT ("Shakuhachi"),              77,
		TEXT ("Whistle"),                 78,
		TEXT ("Ocarina"),                 79,

		TEXT ("Synth Lead"),
		TEXT ("Lead 1 (square)"),         80,
		TEXT ("Lead 2 (sawtooth)"),       81,
		TEXT ("Lead 3 (caliope lead)"),   82,
		TEXT ("Lead 4 (chiff lead)"),     83,
		TEXT ("Lead 5 (charang)"),        84,
		TEXT ("Lead 6 (voice)"),          85,
		TEXT ("Lead 7 (fifths)"),         86,
		TEXT ("Lead 8 (brass + lead)"),   87,

		TEXT ("Synth Pad"),
		TEXT ("Pad 1 (new age)"),         88,
		TEXT ("Pad 2 (warm)"),            89,
		TEXT ("Pad 3 (polysynth)"),       90,
		TEXT ("Pad 4 (choir)"),           91,
		TEXT ("Pad 5 (bowed)"),           92,
		TEXT ("Pad 6 (metallic)"),        93,
		TEXT ("Pad 7 (halo)"),           94,
		TEXT ("Pad 8 (sweep)"),           95,

		TEXT ("Synth Effects"),
		TEXT ("FX 1 (rain)"),             96,
		TEXT ("FX 2 (soundtrack)"),       97,
		TEXT ("FX 3 (crystal)"),          98,
		TEXT ("FX 4 (atmosphere)"),       99,
		TEXT ("FX 5 (brightness)"),      100,
		TEXT ("FX 6 (goblins)"),         101,
		TEXT ("FX 7 (echoes)"),         102,
		TEXT ("FX 8 (sci-fi)"),          103,

		TEXT ("Ethnic"),
		TEXT ("Sitar"),                  104,
		TEXT ("Banjo"),                  105,
		TEXT ("Shamisen"),               106,
		TEXT ("Koto"),                   107,
		TEXT ("Kalimba"),                108,
		TEXT ("Bagpipe"),                109,
		TEXT ("Fiddle"),                 110,
		TEXT ("Shanai"),                 111,

		TEXT ("Percussive"),
		TEXT ("Tinkle Bell"),            112,
		TEXT ("Agogo"),                  113,
		TEXT ("Steel Drums"),            114,
		TEXT ("Woodblock"),              115,
		TEXT ("Taiko Drum"),             116,
		TEXT ("Melodic Tom"),            117,
		TEXT ("Synth Drum"),             118,
		TEXT ("Reverse Cymbal"),         119,

		TEXT ("Sound Effects"),
		TEXT ("Guitar Fret Noise"),      120,
		TEXT ("Breath Noise"),           121,
		TEXT ("Seashore"),               122,
		TEXT ("Bird Tweet"),             123,
		TEXT ("Telephone Ring"),         124,
		TEXT ("Helicopter"),             125,
		TEXT ("Applause"),               126,
		TEXT ("Gunshot"),                127
	};


	TCHAR* getNameFromIndex(int index);


	int getIndexFromName(const TCHAR* instName);
};

