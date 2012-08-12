// ObjFileGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#define NUM_QUAD	10000

void	QuadArray(int iNumQuad	=	NUM_QUAD){
	float	vertex[]={
		-1,1,0,
		1,1,0,
		-1,-1,0,
		1,-1,0
	};
	int idx[]={
		1,2,3,
		3,2,4
	};
	FILE*	pFile	=	fopen("1.obj","w");
	if(pFile!=NULL){
		for(int i=0;i<iNumQuad;i++){
			vertex[2]	=	i;
			vertex[5]	=	i;
			vertex[8]	=	i;
			vertex[11]	=	i;
			fprintf(pFile,
					"v %f %f %f\n"
					"v %f %f %f\n"
					"v %f %f %f\n"
					"v %f %f %f\n",
					vertex[0],vertex[1],vertex[2],
					vertex[3],vertex[4],vertex[5],
					vertex[6],vertex[7],vertex[8],
					vertex[9],vertex[10],vertex[11]);
		}

		for(int i=0;i<iNumQuad;i++){
			fprintf(pFile,
					"f %d %d %d\n"
					"f %d %d %d\n",
					1+i*4,2+i*4,3+i*4,
					3+i*4,2+i*4,4+i*4);
		}

		fclose(pFile);
	}

}
int _tmain(int argc, _TCHAR* argv[])
{
	QuadArray();
	return 0;
}

