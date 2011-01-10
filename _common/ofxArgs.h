/*
 *  ofxArgs.h
 *  graffitiWall_TUIO
 *
 *  Created by Pat Long on 10/02/09.
 *  Copyright 2009 Tangible Interaction. All rights reserved.
 *
 */

//	-opt1 "value 1"
//	opt2="value 2"
//	-flag1

#ifndef _OFXARGS
#define _OFXARGS

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

using std::vector;
using std::map;
using std::cout;
using std::endl;

class ofxArgs{
private:
	int argc;
	vector<std::string> args;
	map<std::string,std::string> opts;

	int strToInt(std::string value);
	float strToFloat(std::string value);

public:
	ofxArgs(int argc, char* argv[]);

	int getCount();

	bool contains(std::string key);

	float getFloat(std::string key);
	int getInt(std::string key);
	std::string getString(std::string key);

	float getFloat(int index);
	int getInt(int index);
	std::string getString(int index);

	void printArgs();
	void printOpts();
};

#endif
