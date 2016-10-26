/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre      09/04/07
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include "system\typedefs.h"                        // Required
#include "user\defines.h"
#include "user\loaderModule.h"

/** V A R I A B L E S ********************************************************/
#pragma udata

/** P R I V A T E  P R O T O T Y P E S ***************************************/
BOOL isEqual(byte str1[8], byte str2[8]);

/** D E C L A R A T I O N S **************************************************/
#pragma code sys

void loadModule(byte idModule, byte binaryStream[8]){

}
// No quiero usar strcmp para no incluir string y que consuma mas memoria
BOOL isEqual(byte str1[8], byte str2[8]){
	byte j = 0;
	BOOL result = TRUE, termine = FALSE;
	for(j = 0; j<8 && !termine; j++){
		if((str1[j] == (char)'\0') || (str2[j] == (char)'\0')){
			termine = TRUE;
		}
		if((char)str1[j] != (char)str2[j]){
			result = FALSE;
		}
	}
	return result;
}

rom near char* getUserTableDirection(byte moduleId[8]){
	rom near char * i = (rom near char *)DIRECTION_TABLE;
	uTab* tabla; 
	byte dest[8];
	byte j = 0;
	while (*i != MEM_VACIO){
		tabla = (uTab*) i;
		for (j = 0; j < 8; j++){       // hacking para poder comparar strings
			dest[j] = (tabla->id)[j];  // para poderse comparar ambos strings deben estar en igual espacio de memoria (RAM / ROM)
		}
		if (isEqual(dest, moduleId)){
			return i;
		}
		i = i + TAM_U_TAB;
	}
	return (rom char*)ERROR;	
}

byte getUserTableSize(){
	rom near char * i = (rom near char *)DIRECTION_TABLE;
	byte size = 0;
	while (*i != MEM_VACIO){
		i = i + TAM_U_TAB;
		size++;
	}
	return size;	
}

//Precondicion: Capas superiores se encargan de hacer el chequeo de que no se exceda del espacio de modulos
void getModuleName(byte line, char* modName){
	byte j;
	rom near char * i = (rom near char *)DIRECTION_TABLE;
	uTab* tabla; 
	i = i + (line * TAM_U_TAB);
	tabla = (uTab*) i;
	for (j = 0; j < 8; j++){       
		modName[j] = (tabla->id)[j];  
	}
	//memcpy(modName, tabla->id, 8); no anda, sera porque estan en espacios de memoria separados?(RAM/ROM)	
}

pUserFunc getModuleInitDirection(rom near char* direction){
	uTab* tabla = (uTab*) direction;
	return tabla->pfI;
}

pUserFunc getModuleReleaseDirection(rom near char* direction){
	uTab* tabla = (uTab*) direction;
	return tabla->pfR;
}

pUserFunc getModuleConfigureDirection(rom near char* direction){
	uTab* tabla = (uTab*) direction;
	return tabla->pfC;
}

/** EOF loaderModule.c ***************************************************************/
