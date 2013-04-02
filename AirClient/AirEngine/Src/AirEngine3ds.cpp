// 3ds.cpp: implementation of the C3ds class.
//
//////////////////////////////////////////////////////////////////////

#include "AirEngine3ds.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define INT_MAX 2147483647 
//#define INT_MIN 2147483647-1 
#define  FLT_MAX 3.402823466e+38F 
#define  FLT_MIN 1.175494351e-38F 
namespace Air{
	
	namespace	Engine{
	
		C3DS::C3DS()
		{
		//	m_Model.numOfMaterials = 0;
		//	m_Model.numOfObjects = 0;
		}
	
		C3DS::~C3DS()
		{
			for(int i = 0;i<m_Model.vObject.size();i++)
			{
				if(m_Model.vObject[i].pVerts)
					delete []m_Model.vObject[i].pVerts;
				if(m_Model.vObject[i].pTexVerts)
					delete []m_Model.vObject[i].pTexVerts;
				if(m_Model.vObject[i].pFaces)
					delete []m_Model.vObject[i].pFaces;
				if(m_Model.vObject[i].pNormals)
					delete []m_Model.vObject[i].pNormals;
				if(m_Model.vObject[i].pTangents!=NULL){
					delete[]	m_Model.vObject[i].pTangents;
				}
			}
		}
		void ReadString(FILE *file,char *str)
		{
			int i=0;char l_char;
			do
			{
				fread (&l_char, 1, 1, file);
				str[i]=l_char;
				i++;
			}while(l_char != '\0' && i<255);
		}
		bool C3DS::Load3DS ( char *p_filename)
		{
			t3DModel *pModel = &m_Model;
			SInt i; //Index variable
			FILE *l_file; //File pointer
			t3DObject *pObj;
			U16 l_chunk_id; //Chunk identifier
			U32 l_chunk_lenght; //Chunk lenght
			U16 l_qty; //Number of elements in each chunk
			U16 l_face_flags; //Flag that stores some face information
			if ((l_file=fopen (p_filename, "rb"))== NULL) 
				return false; //Open the file
			fseek(l_file,0,SEEK_END);
			U32	uiLength	=	ftell(l_file);
			fseek(l_file,0,SEEK_SET);
			while (ftell (l_file) < uiLength) //Loop to scan the whole file 
			{
				fread (&l_chunk_id, 2, 1, l_file); //Read the chunk header
				fread (&l_chunk_lenght, 4, 1, l_file); //Read the lenght of the chunk
				switch (l_chunk_id)
				{
					//----------------- MAIN3DS -----------------
					// Description: Main chunk, contains all the other chunks
					// Chunk ID: 4d4d 
					// Chunk Lenght: 0 + sub chunks
					//-------------------------------------------
					case 0x4d4d: 
					break;    
	
					//----------------- EDIT3DS -----------------
					// Description: 3D Editor chunk, objects layout info 
					// Chunk ID: 3d3d (hex)
					// Chunk Lenght: 0 + sub chunks
					//-------------------------------------------
					case 0x3d3d:
					break;
					
					//--------------- EDIT_OBJECT ---------------
					// Description: Object block, info for each object
					// Chunk ID: 4000 (hex)
					// Chunk Lenght: len(object name) + sub chunks
					//-------------------------------------------
					case 0x4000: 
					{
						t3DObject obj;
						obj.numOfFaces = 0;
						obj.numOfVerts = 0;
						obj.numTexVertex = 0;
						obj.pFaces = 0;
						obj.pTexVerts = 0;
						obj.pVerts = 0;
					//	memset(&obj,0,sizeof(t3DObject));
						ReadString(l_file,obj.strName);
						pModel->vObject.push_back(obj);
	
					}
					break;
	
					//--------------- OBJ_TRIMESH ---------------
					// Description: Triangular mesh, contains chunks for 3d mesh info
					// Chunk ID: 4100 (hex)
					// Chunk Lenght: 0 + sub chunks
					//-------------------------------------------
					case 0x4100:
					break;
					
					//--------------- TRI_VERTEXL ---------------
					// Description: Vertices list
					// Chunk ID: 4110 (hex)
					// Chunk Lenght: 1 x unsigned short (number of vertices) 
					//             + 3 x float (vertex coordinates) x (number of vertices)
					//             + sub chunks
					//-------------------------------------------
					case 0x4110: 
						fread (&l_qty, sizeof (unsigned short), 1, l_file);
						pObj = &pModel->vObject[pModel->vObject.size()-1];
						pObj->numOfVerts = l_qty;
						pObj->pVerts = new tFloat3[l_qty];
						for (i=0; i<l_qty; i++)
						{
							fread (&pObj->pVerts[i].x, sizeof(float), 1, l_file);
							fread (&pObj->pVerts[i].z, sizeof(float), 1, l_file);
							fread (&pObj->pVerts[i].y, sizeof(float), 1, l_file);
					//		pObj->pVerts[i].x = -pObj->pVerts[i].x;
						}
						break;
	
					//--------------- TRI_FACEL1 ----------------
					// Description: Polygons (faces) list
					// Chunk ID: 4120 (hex)
					// Chunk Lenght: 1 x unsigned short (number of polygons) 
					//             + 3 x unsigned short (polygon points) x (number of polygons)
					//             + sub chunks
					//-------------------------------------------
					case 0x4120://face
						fread (&l_qty, sizeof (unsigned short), 1, l_file);
						pObj = &pModel->vObject[pModel->vObject.size()-1];
						pObj->numOfFaces = l_qty;
						pObj->pFaces = new tFace[l_qty];
					//	memset(pObj->pFaces,0,sizeof(tFace)*l_qty);
						for (i=0; i<l_qty; i++)
						{
							fread (&pObj->pFaces[i].a, sizeof (unsigned short), 1, l_file);
							fread (&pObj->pFaces[i].b, sizeof (unsigned short), 1, l_file);
							fread (&pObj->pFaces[i].c, sizeof (unsigned short), 1, l_file);
							fread (&l_face_flags, sizeof (unsigned short), 1, l_file);
							pObj->pFaces[i].matIndex = -1;
						}
						break;
					case 0x4130:// This is found if the object has a material, either texture map or color
					{
						char strMaterial[255];
						ReadString(l_file,strMaterial);
						short n,nf;
						fread (&n, sizeof(short), 1, l_file);
						
						pObj = &pModel->vObject[pModel->vObject.size()-1];
						// Go through all of the textures
						for( i = 0; i < m_Model.vMaterials.size(); i++)
						{
							// If the material we just read in matches the current texture name
							if(strcmp(strMaterial, m_Model.vMaterials[i].strName) == 0)
							{
								while (n-- > 0) 
								{	// Material number assigned to any face
									// Numero di material assegnato ad ogni faccia
									fread (&nf, sizeof(short), 1, l_file);
									pObj->pFaces[nf].matIndex = i;
								}
								// Now that we found the material, check if it's a texture map.
								// If the strFile has a string length of 1 and over it's a texture
								if(strlen(pModel->vMaterials[i].strFile) > 0) 
								{
								// Set the object's flag to say it has a texture map to bind.
								//	pObj->bHasTexture = true;
								}	
								break;
							}
						}
						
						break;
					}
					//------------- TRI_MAPPINGCOORS ------------
					// Description: Vertices list
					// Chunk ID: 4140 (hex)
					// Chunk Lenght: 1 x unsigned short (number of mapping points) 
					//             + 2 x float (mapping coordinates) x (number of mapping points)
					//             + sub chunks
					//-------------------------------------------
					case 0x4140:
						fread (&l_qty, sizeof (unsigned short), 1, l_file);
						pObj = &pModel->vObject[pModel->vObject.size()-1];
						pObj->pTexVerts = new tVector2[l_qty];
						for (i=0; i<l_qty; i++)
						{
							fread (&pObj->pTexVerts[i].x, sizeof (float), 1, l_file);
							fread (&pObj->pTexVerts[i].y, sizeof (float), 1, l_file);
						}
						break;
					case 0xAFFF://MATERIAL:
					{
						tMaterialInfo newTexture ;//= {0};
					//	memset(&newTexture,0,sizeof(tMaterialInfo));
						newTexture.strFile[0] = '\0';
						newTexture.strName[0] = '\0';
						m_Model.vMaterials.push_back(newTexture);
						break;
					}
					case 0xA000://// This holds the material name
					{
						tMaterialInfo &matInfo = m_Model.vMaterials[m_Model.vMaterials.size()-1];
						ReadString(l_file,matInfo.strName);
						break;
					}
					case 0xA300: //// This holds the file name of the texture
					{
						tMaterialInfo &matInfo = m_Model.vMaterials[m_Model.vMaterials.size()-1];
						ReadString(l_file,matInfo.strFile);
						break;
					}
					case 0xA200:	//texture map flag//	// This is the header for the texture info
						break;
					//----------- Skip unknow chunks ------------
					//We need to skip all the chunks that currently we don't use
					//We use the chunk lenght information to set the file pointer
					//to the same level next chunk
					//-------------------------------------------
					default:
						 fseek(l_file, l_chunk_lenght-6, SEEK_CUR);
				} 
			}
			fclose (l_file); // Closes the file stream
			return true; // Returns ok
		}
		bool C3DS::Init(char *sFileName)
		{
			if(!Load3DS(sFileName))
				return false;
	// 		for(int i = 0;i<m_Model.vMaterials.size();i++)
	// 		{
	// 			if(strlen(m_Model.vMaterials[i].strFile)>0)
	// 			{
	// 				m_Model.vMaterials[i].cTxt.LoadBMP(m_Model.vMaterials[i].strFile);
	// 			}
	// 		}
			return true;
		}
		void C3DS::Draw()
		{
	// 		glEnable(GL_DEPTH_TEST);
	// 		glEnable(GL_TEXTURE_2D);
	// 		glColor3ub(255, 255, 255);
	// 		for(int i = 0;i<m_Model.vObject.size();i++)
	// 		{
	// 			t3DObject &obj = m_Model.vObject[i];	
	// 			for (int j=0;j<obj.numOfFaces;j++)
	// 			{
	// 				
	// 				tFace &face = obj.pFaces[j];
	// 				if(obj.pTexVerts)
	// 					glBindTexture(GL_TEXTURE_2D, m_Model.vMaterials[face.matIndex].cTxt.GetTxtID());
	// 				glBegin(GL_TRIANGLES); // glBegin and glEnd delimit the vertices that define a primitive (in our case triangles)
	// 				//----------------- FIRST VERTEX -----------------
	// 				// Texture coordinates of the first vertex
	// 				if(obj.pTexVerts)
	// 				glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].a ].x,
	// 					obj.pTexVerts[ obj.pFaces[j].a ].y);
	// 				// Coordinates of the first vertex
	// 				glVertex3f( obj.pVerts[face.a].x,
	// 					obj.pVerts[face.a].y,
	// 					obj.pVerts[face.a].z); //Vertex definition
	// 				
	// 				//----------------- SECOND VERTEX -----------------
	// 				// Texture coordinates of the second vertex
	// 				if(obj.pTexVerts)
	// 				glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].b ].x,
	// 					obj.pTexVerts[ obj.pFaces[j].b ].y);
	// 				// Coordinates of the second vertex
	// 				glVertex3f( obj.pVerts[face.b].x,
	// 					obj.pVerts[face.b].y,
	// 					obj.pVerts[face.b].z);
	// 				
	// 				//----------------- THIRD VERTEX -----------------
	// 				// Texture coordinates of the third vertex
	// 				if(obj.pTexVerts)
	// 				glTexCoord2f( obj.pTexVerts[ obj.pFaces[j].c ].x,
	// 					obj.pTexVerts[ obj.pFaces[j].c ].y);
	// 				// Coordinates of the Third vertex
	// 				glVertex3f( obj.pVerts[face.c].x,
	// 					obj.pVerts[face.c].y,
	// 					obj.pVerts[face.c].z);
	// 				glEnd();
	//			}
	//		}
		}
	
	#define	Read(pReturn,uiSize,uiCount,pData)	\
		{\
			memcpy(pReturn,pData,uiSize*uiCount);\
			pData+=uiSize*uiCount;\
			uiPos+=uiSize*uiCount;\
		}
	#define ReadStr(pData,pString)\
		{\
			int i=0;\
			do{\
				Read (&pString[i++], 1, 1, pData);\
			}while(*pData != '\0' && i<255);\
			pData++;uiPos++;pString[i]='\0';\
		}
	
	
		U1 C3DS::LoadFromMemery( U8* pData,U32 uiSize ){
			
			U16 usChunkId		=	0; //Chunk identifier
			U32 uiChunkLenght	=	0; //Chunk lenght
			U16 l_qty			=	0; //Number of elements in each chunk
			U16 l_face_flags	=	0; //Flag that stores some face information
			U8*	l_file			=	pData;
			t3DObject*	pObj	=	NULL;
			t3DModel *pModel	=	&m_Model;
	
			U32	uiPos	=	0;
			while(uiPos<uiSize){
				Read (&usChunkId, 2, 1, l_file); //Read the chunk header
				Read (&uiChunkLenght, 4, 1, l_file); //Read the lenght of the chunk
				switch (usChunkId){
					//----------------- MAIN3DS -----------------
					// Description: Main chunk, contains all the other chunks
					// Chunk ID: 4d4d 
					// Chunk Lenght: 0 + sub chunks
					//-------------------------------------------
					case 0x4d4d: 
						break;    
	
						//----------------- EDIT3DS -----------------
						// Description: 3D Editor chunk, objects layout info 
						// Chunk ID: 3d3d (hex)
						// Chunk Lenght: 0 + sub chunks
						//-------------------------------------------
					case 0x3d3d:
						break;
	
						//--------------- EDIT_OBJECT ---------------
						// Description: Object block, info for each object
						// Chunk ID: 4000 (hex)
						// Chunk Lenght: len(object name) + sub chunks
						//-------------------------------------------
					case 0x4000: 
						{
							t3DObject obj;
							obj.numOfFaces = 0;
							obj.numOfVerts = 0;
							obj.numTexVertex = 0;
							obj.pFaces = 0;
							obj.pTexVerts = 0;
							obj.pVerts = 0;
							obj.pNormals	=	NULL;
							//	memset(&obj,0,sizeof(t3DObject));
							ReadStr(l_file,obj.strName);
							pModel->vObject.push_back(obj);
	
						}
						break;
	
						//--------------- OBJ_TRIMESH ---------------
						// Description: Triangular mesh, contains chunks for 3d mesh info
						// Chunk ID: 4100 (hex)
						// Chunk Lenght: 0 + sub chunks
						//-------------------------------------------
					case 0x4100:
						break;
	
						//--------------- TRI_VERTEXL ---------------
						// Description: Vertices list
						// Chunk ID: 4110 (hex)
						// Chunk Lenght: 1 x unsigned short (number of vertices) 
						//             + 3 x float (vertex coordinates) x (number of vertices)
						//             + sub chunks
						//-------------------------------------------
					case 0x4110: 
						Read (&l_qty, sizeof (unsigned short), 1, l_file);
						pObj = &pModel->vObject[pModel->vObject.size()-1];
						pObj->numOfVerts = l_qty;
						pObj->pVerts = new tFloat3[l_qty];
						for (SInt	i=0; i<l_qty; i++)
						{
							Read (&pObj->pVerts[i].x, sizeof(float), 1, l_file);
							Read (&pObj->pVerts[i].z, sizeof(float), 1, l_file);
							Read (&pObj->pVerts[i].y, sizeof(float), 1, l_file);
							//		pObj->pVerts[i].x = -pObj->pVerts[i].x;
						}
						break;
	
						//--------------- TRI_FACEL1 ----------------
						// Description: Polygons (faces) list
						// Chunk ID: 4120 (hex)
						// Chunk Lenght: 1 x unsigned short (number of polygons) 
						//             + 3 x unsigned short (polygon points) x (number of polygons)
						//             + sub chunks
						//-------------------------------------------
					case 0x4120://face
						Read (&l_qty, sizeof (unsigned short), 1, l_file);
						pObj = &pModel->vObject[pModel->vObject.size()-1];
						pObj->numOfFaces = l_qty;
						pObj->pFaces = new tFace[l_qty];
						//	memset(pObj->pFaces,0,sizeof(tFace)*l_qty);
						for (SInt	i=0; i<l_qty; i++)
						{
							U16 a,b,c;
							Read (&a, sizeof (unsigned short), 1, l_file);
							Read (&b, sizeof (unsigned short), 1, l_file);
							Read (&c, sizeof (unsigned short), 1, l_file);
							pObj->pFaces[i].a	=	a;
							pObj->pFaces[i].b	=	b;
							pObj->pFaces[i].c	=	c;
							Read (&l_face_flags, sizeof (unsigned short), 1, l_file);
							pObj->pFaces[i].matIndex = -1;
						}
						break;
					case 0x4130:// This is found if the object has a material, either texture map or color
						{
							char strMaterial[255];
							ReadStr(l_file,strMaterial);
							short n,nf;
							Read (&n, sizeof(short), 1, l_file);
	
							pObj = &pModel->vObject[pModel->vObject.size()-1];
							// Go through all of the textures
							for( SInt	i = 0; i < m_Model.vMaterials.size(); i++)
							{
								// If the material we just read in matches the current texture name
								if(strcmp(strMaterial, m_Model.vMaterials[i].strName) == 0)
								{
									while (n-- > 0) 
									{	// Material number assigned to any face
										// Numero di material assegnato ad ogni faccia
										Read (&nf, sizeof(short), 1, l_file);
										pObj->pFaces[nf].matIndex = i;
									}
									// Now that we found the material, check if it's a texture map.
									// If the strFile has a string length of 1 and over it's a texture
									if(strlen(pModel->vMaterials[i].strFile) > 0) 
									{
										// Set the object's flag to say it has a texture map to bind.
										//	pObj->bHasTexture = true;
									}	
									break;
								}
							}
	
							break;
						}
						//------------- TRI_MAPPINGCOORS ------------
						// Description: Vertices list
						// Chunk ID: 4140 (hex)
						// Chunk Lenght: 1 x unsigned short (number of mapping points) 
						//             + 2 x float (mapping coordinates) x (number of mapping points)
						//             + sub chunks
						//-------------------------------------------
					case 0x4140:
						Read (&l_qty, sizeof (unsigned short), 1, l_file);
						pObj = &pModel->vObject[pModel->vObject.size()-1];
						pObj->pTexVerts = new tVector2[l_qty];
						for (SInt	i=0; i<l_qty; i++)
						{
							Read (&pObj->pTexVerts[i].x, sizeof (float), 1, l_file);
							Read (&pObj->pTexVerts[i].y, sizeof (float), 1, l_file);
						}
						break;
					case 0xAFFF://MATERIAL:
						{
							tMaterialInfo newTexture ;//= {0};
							//	memset(&newTexture,0,sizeof(tMaterialInfo));
							newTexture.strFile[0] = '\0';
							newTexture.strName[0] = '\0';
							m_Model.vMaterials.push_back(newTexture);
							break;
						}
					case 0xA000://// This holds the material name
						{
							tMaterialInfo &matInfo = m_Model.vMaterials[m_Model.vMaterials.size()-1];
							ReadStr(l_file,matInfo.strName);
							break;
						}
					case 0xA300: //// This holds the file name of the texture
						{
							tMaterialInfo &matInfo = m_Model.vMaterials[m_Model.vMaterials.size()-1];
							ReadStr(l_file,matInfo.strFile);
							break;
						}
					case 0xA200:	//texture map flag//	// This is the header for the texture info
						break;
						//----------- Skip unknow chunks ------------
						//We need to skip all the chunks that currently we don't use
						//We use the chunk lenght information to set the file pointer
						//to the same level next chunk
						//-------------------------------------------
					default:
						l_file+=uiChunkLenght-6;uiPos+=uiChunkLenght-6;//Read(l_file, l_chunk_lenght-6, SEEK_CUR);
				}
			}
			return true;
		}
	
		U1 C3DS::ComputeNormal(U1	bTangent){
			if(m_Model.vObject.empty())
				return false;
			t3DObject&	obj	=	m_Model.vObject[0];
	
			UInt	uiNumFace	=	obj.numOfFaces;
			UInt	uiNumVertex	=	obj.numOfVerts;
			obj.pNormals	=	new	tFloat3[obj.numOfVerts];
			if(bTangent){
				obj.pTangents	=	new	tFloat3[uiNumVertex];
			}
			for(UInt	i=0;i<uiNumVertex;i++){
				//存储相关面列表
				std::list<tFace>	lstFace;
				//找出跟该顶点相关的面
				for(UInt	j=0;j<uiNumFace;j++){
					if(	obj.pFaces[j].a	==	i||
						obj.pFaces[j].b	==	i||
						obj.pFaces[j].c	==	i){
						lstFace.push_back(obj.pFaces[j]);
					}
				}
				//定义一个结构体 存储法线和权重
				struct NormalWeight{
					tFloat3	normal;
					Real		weight;
				};
				//法线权重列表
				std::list<NormalWeight>	lstNormal;
				//遍历面列表 计算出每个面的发现
				std::list<tFace>::iterator	itr	=	lstFace.begin();
				for(;itr!=lstFace.end();itr++){
					//计算法线
					static	Float3	v[6];
					static	NormalWeight	vNormal;
// 					tFace	f	=	(tFace)(*itr);
// 					v[0]	=	obj.pVerts[f.a];
// 					v[1]	=	obj.pVerts[f.b];
// 					v[2]	=	obj.pVerts[f.c];
// 					
// 					v[3]	=	v[0]-v[1];//v[3].normalise();
// 					v[4]	=	v[0]-v[2];//v[4].normalise();
// 	
// 					vNormal.normal	=	-v[3].crossProduct(v[4]);
// 					//先将每个面权重设置为同一个值
// 					
// 	 				v[3].normalise();
// 	 				v[4].normalise();
// 					vNormal.weight	=	acos(v[3].dotProduct(v[4]));
// 					vNormal.normal.normalise();
// 	
					//vNormal.weight	=	acos(v[3].dotProduct(v[4]));
	
					lstNormal.push_back(vNormal);
				}
	
				std::list<NormalWeight>	lstTangent;
				//for()
				lstFace.clear();
	
				//求出法线的平均值
				NormalWeight tempNormal;
				tempNormal.normal	=	tFloat3(0,0,0);
				tempNormal.weight	=	0.0f;
				std::list<NormalWeight>::iterator	it	=	lstNormal.begin();
				for(;it!=lstNormal.end();it++){
					NormalWeight	vNormal	=	(NormalWeight)(*it);
// 					tempNormal.normal	+=	vNormal.normal*vNormal.weight;
// 					tempNormal.weight	+=	vNormal.weight;
				}
	
				if(tempNormal.weight==0.0f)
					tempNormal.weight	=	1.0f;
				//存入法线列表中
// 				obj.pNormals[i]	=	tempNormal.normal/tempNormal.weight;
// 				obj.pNormals[i].normalise();
	
				//AChar	str[MAX_NAME];
				//sprintf(str,"%d	=	%f %f %f\n",i,obj.pNormals[i].x,obj.pNormals[i].y,obj.pNormals[i].z);
				//OutputDebugStringA(str);
				lstNormal.clear();
			}
	
	
	// 		v1 = normalize(a-b);
	// 		v2 = normalize(a-c);
	// 		normalWeight = acos(dot(v1,v2)); 
	// 
	// 		normal = 0,0,0
	// 			totalWeight = 0
	// 			foreach triangle connect to vertex a
	// 			normal += trangleNormal * normalWeight;
	// 		totalWeight += normalWeight;
	// 
	// 		finalNormal = normal/totalWeight;
	
	
	
			return true;
		}
	}
};