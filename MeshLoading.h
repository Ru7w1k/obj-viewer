#pragma once

#pragma warning(disable : 4996)

#define POINT_COORDS 3
#define TEXTURE_COORDS 2
#define NORMAL_COORDS 3
#define FACE_TOKENS 4
#define TRIANGE_VERTICES 3
#define QUAD_VERTICES 4
#define BUFFER_SIZE 256

typedef struct Vec2DInt
{
	GLint** pp_arr;
	size_t size;
} vec2dInt_t;

typedef struct Vec2DFloat
{
	GLfloat** pp_arr;
	size_t size;
} vec2dFloat_t;

//Vectors to load mesh data
vec2dFloat_t* gpVertices, * gpTexture, * gpNormals;
vec2dInt_t* gpFaceTriangles, * gpFaceTextures, * gpFaceNormals, * gpFaceQuads;

//Mesh File
FILE* gpMeshFile = NULL;
FILE* gpLogFile = NULL;

//Line in a file
char gpLine[BUFFER_SIZE];

void loadMeshData(char* fileName)
{
	//Function declarations
	void uninitialize();

	//Create vectors to load data
	vec2dInt_t* CreateVec2dInt(void);
	vec2dFloat_t* CreateVec2dFloat(void);

	//Functions for Pushing data to vecotrs
	void PushBackVec2DInt(vec2dInt_t * pVec, int* pArr);
	void PushBackVec2DFloat(vec2dFloat_t * pVec, float* pArr);

	//Wrapper around calloc
	void* CallocWrapper(int, size_t);

	//code
	fopen_s(&gpMeshFile, fileName, "r");
	if (!gpMeshFile)
	{
		fprintf(gpLogFile, "Unable to open Object file: %s \n", fileName);
		uninitialize();
	}

	//Create vectors to load vertex, texrture and normal data
	gpVertices = CreateVec2dFloat();
	gpTexture = CreateVec2dFloat();
	gpNormals = CreateVec2dFloat();

	//Create vectors to hold interger face data
	gpFaceTriangles = CreateVec2dInt();
	gpFaceQuads = CreateVec2dInt();
	gpFaceTextures = CreateVec2dInt();
	gpFaceNormals = CreateVec2dInt();

	//File seperator strings
	char seperatorSpace[] = " ";
	char sepSlash[] = "/";

	//Token Pointers
	//Character pointer for holding first word in blue
	char* firstToken = NULL;
	char* token = NULL;

	//Array of character
	char* faceTokens[FACE_TOKENS];

	int nrTokens;

	//vertex index
	char* tokenVertexIndex = NULL;

	//texture index
	char* tokenTextureIndex = NULL;

	//Normal index
	char* tokenNormalIndex = NULL;

	//Loading starts
	while (fgets(gpLine, BUFFER_SIZE, gpMeshFile) != NULL)
	{
		firstToken = strtok(gpLine, seperatorSpace);

		//If first token indicates vertex data
		if (strcmp(firstToken, "v") == 0)
		{
			GLfloat* pvecPointCoordinate = (GLfloat*)CallocWrapper(POINT_COORDS, sizeof(GLfloat));

			for (int i = 0; i != POINT_COORDS; i++)
				pvecPointCoordinate[i] = (GLfloat)atof(strtok(NULL, seperatorSpace));

			PushBackVec2DFloat(gpVertices, pvecPointCoordinate);
		}

		else if (strcmp(firstToken, "vt") == 0)
		{
			GLfloat* pvecTextureCoordinates = (GLfloat*)CallocWrapper(POINT_COORDS, sizeof(GLfloat));

			for (int i = 0; i != TEXTURE_COORDS; i++)
				pvecTextureCoordinates[i] = (GLfloat)atof(strtok(NULL, seperatorSpace));

			PushBackVec2DFloat(gpTexture, pvecTextureCoordinates);
		}

		else if (strcmp(firstToken, "vn") == 0)
		{
			GLfloat* pvecNormalCoordinates = (GLfloat*)CallocWrapper(NORMAL_COORDS, sizeof(GLfloat));

			for (int i = 0; i != NORMAL_COORDS; i++)
				pvecNormalCoordinates[i] = (GLfloat)atof(strtok(NULL, seperatorSpace));

			PushBackVec2DFloat(gpNormals, pvecNormalCoordinates);
		}

		else if (strcmp(firstToken, "f") == 0)
		{
			GLint* pvecVertexIndices = (GLint*)CallocWrapper(4, sizeof(GLint));
			GLint* pvecTextureIndices = (GLint*)CallocWrapper(4, sizeof(GLint));
			GLint* pvecNormalIndices = (GLint*)CallocWrapper(4, sizeof(GLint));

			//Initialize all above with
			memset((char*)faceTokens, 0, FACE_TOKENS);

			//Extract the information
			nrTokens = 0;
			while (token = strtok(NULL, seperatorSpace))
			{
				if (strlen(token) < 3)
					break;

				faceTokens[nrTokens] = token;
				nrTokens++;
			}

			//TCHAR wtext[255];
			for (int i = 0; i != nrTokens; i++)
			{
				tokenVertexIndex = strtok(faceTokens[i], sepSlash);
				tokenTextureIndex = strtok(NULL, sepSlash);
				tokenNormalIndex = strtok(NULL, sepSlash);

				//convert to int
				pvecVertexIndices[i] = atoi(tokenVertexIndex);
				pvecTextureIndices[i] = atoi(tokenTextureIndex);
				pvecNormalIndices[i] = atoi(tokenNormalIndex);
			}

			if (nrTokens == 3)
				PushBackVec2DInt(gpFaceTriangles, pvecVertexIndices);

			//For handling quad based models
			if (nrTokens == 4)
				PushBackVec2DInt(gpFaceQuads, pvecVertexIndices);

			PushBackVec2DInt(gpFaceTextures, pvecTextureIndices);
			PushBackVec2DInt(gpFaceNormals, pvecNormalIndices);
		}

		memset((void*)gpLine, (int)'\0', BUFFER_SIZE);
	}

	//Close meshfile
	fclose(gpMeshFile);
	gpMeshFile = NULL;

	fprintf(gpLogFile, "gpVertices->size: %zu\ngpTexture->size: %zu\ngpNormals->size: %zu\nfaceTriangles:%zu\n", gpVertices->size, gpTexture->size, gpNormals->size, gpFaceTriangles->size);
}


vec2dInt_t* CreateVec2dInt(void)
{
	//Function declaration
	void* CallocWrapper(int no, size_t size);

	//code
	return (vec2dInt_t*)CallocWrapper(1, sizeof(vec2dInt_t));
}

vec2dFloat_t* CreateVec2dFloat(void)
{
	//Function declaration
	void* CallocWrapper(int, size_t);

	//code
	return (vec2dFloat_t*)CallocWrapper(1, sizeof(vec2dFloat_t));
}

void PushBackVec2DInt(vec2dInt_t* pVec, int* pArr)
{
	//Function declaration
	void* ReallocWrapper(void* p, size_t newSize);

	pVec->pp_arr = (GLint**)ReallocWrapper(pVec->pp_arr, (pVec->size + 1) * sizeof(int**));
	pVec->size++;
	pVec->pp_arr[pVec->size - 1] = pArr;
}
void PushBackVec2DFloat(vec2dFloat_t* pVec, float* pArr)
{
	//Function declaration
	void* ReallocWrapper(void* p, size_t newSize);

	pVec->pp_arr = (GLfloat**)ReallocWrapper(pVec->pp_arr, (pVec->size + 1) * sizeof(GLfloat**));
	pVec->size++;
	pVec->pp_arr[pVec->size - 1] = pArr;
}

void CleanVec2DInt(vec2dInt_t** ppVec)
{
	vec2dInt_t* pVec = *ppVec;
	for (size_t i = 0; i != pVec->size; i++)
		free(pVec->pp_arr[i]);
	free(pVec);
	*ppVec = NULL;
}

void CleanVec2DFloat(vec2dFloat_t** ppVec)
{
	vec2dFloat_t* pVec = *ppVec;
	for (size_t i = 0; i != pVec->size; i++)
		free(pVec->pp_arr[i]);
	free(pVec);
	*ppVec = NULL;
}

//Calloc Wrapper
void* CallocWrapper(int noOfelements, size_t sizeOfElement)
{
	//function declarations
	void uninitialize();

	void* ptr = calloc(noOfelements, sizeOfElement);
	if (!ptr)
	{
		fprintf(gpLogFile, "Calloc Error: Unable to allocate memory \n");
		uninitialize();
	}

	return ptr;
}

//Realloc wrapper
void* ReallocWrapper(void* p, size_t newSize)
{
	//function declarations
	void uninitialize();

	void* ptr = realloc(p, newSize);
	if (!ptr)
	{
		fprintf(gpLogFile, "Unable to allocate memory \n");
		uninitialize();
	}

	return ptr;
}