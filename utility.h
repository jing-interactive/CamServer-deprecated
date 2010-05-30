#pragma once

//little utilities

#ifndef IN
	#define IN
#endif
#ifndef OUT
	#define OUT
#endif

#define SAFE_DELETE(p) {if (p) delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) {if (p) delete[] p; p = NULL;}
#define SAFE_REMOVE(p) {if (p) p->remove(); p = NULL;}
#define SAFE_CLEAR(p) {if (p) p->clear();}

#define RAND() (float) (rand()/(RAND_MAX * 1.0))
#define SIGN(x) (((x) < 0) ? -1 : 1)

#define prefix_m(variable_name) m_##variable_name
#define prefix_m_p(variable_name) m_p##variable_name

#define prefix_set(variable_name) set##variable_name
#define prefix_get(variable_name) get##variable_name

//general
#define set_get_define(variable_type, variable_name)\
protected:\
	variable_type prefix_m(variable_name);\
public:\
	void prefix_set(variable_name)(variable_type variable_name){ prefix_m(variable_name) = variable_name;}\
	variable_type prefix_get(variable_name)(){ return prefix_m(variable_name);}


#define get_define(variable_type, variable_name)\
protected:\
	variable_type prefix_m(variable_name);\
public:\
	variable_type prefix_get(variable_name)(){ return prefix_m(variable_name);}
 
//////////////////////////////////////////////////////////////////////////
//pointer-specific
#define set_get_pointer_define(variable_type, variable_name)\
protected:\
	variable_type * prefix_m_p(variable_name);\
public:\
	void prefix_set(variable_name)(variable_type* variable_name){ prefix_m_p(variable_name) = variable_name;}\
	variable_type * prefix_get(variable_name)(){ return prefix_m_p(variable_name);}

#define get_pointer_define(variable_type, variable_name)\
protected:\
	variable_type * prefix_m_p(variable_name);\
public:\
	variable_type * prefix_get(variable_name)(){ return prefix_m_p(variable_name);}
 