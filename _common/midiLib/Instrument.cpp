#include "Instrument.h"


TCHAR* Instrument::getNameFromIndex(int index){
	//
	if ( index > 127 || index < 0)
		return TEXT("undefined instrument");

	int iFamily = index / NUM_INTST_PER_FAMILY;
	int iInst = index % NUM_INTST_PER_FAMILY;

	return g_fams[iFamily]._instruments[iInst]._instrument_name;
}

int Instrument::getIndexFromName(const TCHAR* instName){
	//
	for (int f = 0; f < NUM_FAMILYS; f++){
		//
		for (int inst = 0; inst < NUM_INTST_PER_FAMILY; inst++){
			//
			if (0 == wcscmp(instName, g_fams[f]._instruments[inst]._instrument_name) ){
				//
				return g_fams[f]._instruments[inst]._index;
			}
		}
	}
	return(-1);//not found
}