// forward declarations
void GetNeighbours(__global float3* previousPos, float3 currPos, float neighbourhoodSize, Array *neighbours);
float3 GetAverageNeighbourPos(__global float3* previousPos, Array *neighbours);
float3 GetAverageNeighbourVelocity(__global float3* currentVelocity, Array *neighbours);
float3 Seek(float3 target, float3 pos);
float3 Seperation(__global float3* previousPos, Array *neighbours, float3 pos);
float16 Rotate(float16 transformMatrix, float angle, float3 axis);

// dynamic array struct
typedef struct {
  int *array;
  size_t used;
  size_t size;
} Array;
void InitArray(Array *a, size_t initialSize);
void InsertArray(Array *a, int element);
void FreeArray(Array *a);


//weights: [0] = cohesion, [1]=allignment, [2]=seperation
__kernel void Flocking(__global float16* transform, __global float3* previousPos, __global float3* currentVelocity, float* weights, float time, float maxSpeed, float neighbourhoodSize)
{
	size_t globalId = get_global_id(0); // current agent id in workgroup
	constant float3 up = float3(0.f, 1.f, 0.f);
	
	//Create neighbours array
	Array neigbours;
	InitArray(&neigbours, 10);
	GetNeighbours(previousPos, previousPos[globalId], neighbourhoodSize, &neigbours);// register neighbour indices
	
	float3 linearVelocity;
	
	//Cohesion
	float3 averageNeighbourLoc = GetAverageNeighbourPos(previousPos, &neigbours);
	float3 cohesionVelocity = Seek(averageNeighbourLoc, previousPos[globalId]);
	
	linearVelocity = cohesionVelocity * weights[0];//set linear velocity
	
	//Allignment
	float3 allignmentVelocity = GetAverageNeighbourVelocity(currentVelocity, &neigbours);
	allignmentVelocity = fast_normalize(allignmentVelocity);
	
	linearVelocity += allignmentVelocity * weights[1];// add to linear velocity
	
	//Seperation
	float3 seperationVelocity = Seperation(previousPos, neigbours, previousPos[globalId]);
	
	linearVelocity += seperationVelocity * weights[2];// add to linear velocity
	
	float3 direction = normalize(linearVelocity);
	linearVelocity = direction * maxSpeed;
	
	barrier(CLK_GLOBAL_MEM_FENCE); // all previous must be completed on all work items	
	
	
	//change global values
	currentVelocity = linearVelocity;
	previousPos += linearVelocity * maxSpeed;
	
	//translate matrix
	transform[globalId].sCDE = previousPos;
	
	//rotate matrix
	float angle = acos(dot(up, linearVelocity));
	float3 axis = cross(linearVelocity, up);
	transform[globalId] = rotate(transform[globalId], angle, axis);


	FreeArray(neigbours);
}

// Get indices of neigbours
void GetNeighbours(__global float3* previousPos, float3 currPos, float neighbourhoodSize, Array *neighbours)
{
	size_t globalId = get_global_id(0); // current agent id
	size_t globalSize = get_global_size(0);
	
	for(int i = 0; i != globalSize; ++i)
	{
		if(i == globalId)
			continue; //continue if i is current agent
		
		if(fast_distance(currPos, previousPos[i]) <= neighbourhoodSize)
		{
			InsertArray(&neigbours, i);// insert index in array
		}
	}
	
}

float3 GetAverageNeighbourPos(__global float3* previousPos, Array *neighbours)
{
	float3 average = float3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i != neigbours->used; ++i)
	{
		average = average + previousPos[neigbours->array[i]];
	}
	average = average / neigbours->used;
	return average;
}

float3 GetAverageNeighbourVelocity(__global float3* currentVelocity, Array *neighbours)
{
	float3 average = float3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i != neigbours->used; ++i)
	{
		average = average + currentVelocity[neigbours->array[i]];
	}
	average = average / neigbours->used;
	return average;
}

// returns normalized linear velocity
float3 Seek(float3 target, float3 pos)
{
	float3 linearVelocity = target - pos;
	linearVelocity = fast_normalize(linearVelocity);
	return linearVelocity;
}

float3 Seperation(__global float3* previousPos, Array *neighbours, float3 pos)
{
	float3 linearVelocity = float3(0.0f, 0.0f, 0.0f);
	float3 steeringCurrentNeighbour;
	for(int i = 0; i != neigbours->used; ++i)
	{
		steeringCurrentNeighbour = -(previousPos[neigbours->array[i]]- pos);
		steeringCurrentNeighbour /= fast_distance(previousPos[neigbours->array[i]], pos); // longer distance -> smaller velocity
		linearVelocity += steeringCurrentNeighbour;
	}
	
	linearVelocity = fast_normalize(linearVelocity);
	return linearVelocity;
}


float16 Rotate(float16 transformMatrix, float angle, float3 axis)
{
	float c = cos(angle);
	float s = sin(angle);
	
	axis = normalize(axis);
	float3 temp = (1.f - c)*axis;
	float16 rotateMat;
	rotateMat.s0 = c + temp.x * axis.x;
	rotateMat.s1 = temp.x * axis.y + s * axis.z;
	rotateMat.s2 = temp.x * axis.z - s * axis.y;
	
	rotateMat.s4 = temp.y * axis.x - s * axis.z;
	rotateMat.s5 = c + temp.y * axis.y;
	rotateMat.s6 = temp.y * axis.z + s * axis.x;
	
	rotateMat.s8 = temp.z * axis.x + s * axis.y;
	rotateMat.s9 = temp.z * axis.y - s * axis.x;
	rotateMat.sA = c + temp.z * axis.z;
	
	float16 result;
	result.s0123 = transformMatrix.s0123 * rotateMat.s0 + transformMatrix.s4567 * rotateMat.s1 + transformMatrix.s89ab * rotateMat.s2;
	result.s4567 = transformMatrix.s0123 * rotateMat.s4 + transformMatrix.s4567 * rotateMat.s5 + transformMatrix.s89ab * rotateMat.s6;
	result.s89ab = transformMatrix.s0123 * rotateMat.s8 + transformMatrix.s4567 * rotateMat.s9 + transformMatrix.s89ab * rotateMat.sA;
	result.sCDEF = transformMatrix.sCDEF;
	return result;
}

// Array functions
void InitArray(Array *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(int));
  a->used = 0;
  a->size = initialSize;
}

void InsertArray(Array *a, int element) {
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(int));
  }
  a->array[a->used++] = element;
}

void FreeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}
