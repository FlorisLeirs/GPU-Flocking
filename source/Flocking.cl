// dynamic array struct
typedef struct {
  int array[50];
  size_t used;
} Array;
void InitArray(Array* a);
void InsertArray(Array* a, int element);
//void FreeArray(Array *a);
// forward declarations
void GetNeighbours(__global float3* previousPos, float3 currPos, float neighbourhoodSize, Array* neighbours, int globalId);
float3 GetAverageNeighbourPos(float3* previousPos, Array* neighbours);
float3 GetAverageNeighbourVelocity(float3* currentVelocity, Array* neighbours);
float3 Seek(float3 target, float3 pos);
float3 Seperation(float3* previousPos, Array* neighbours, float3 pos);
float16 Rotate(float16 transformMatrix, float angle, float3 axis);




//weights: [0] = cohesion, [1]=allignment, [2]=seperation
__kernel void Flocking(__global float16* transform, __global float3* previousPos, __global float3* currentVelocity, __constant float* weights, __constant float* time, __constant float* maxSpeed, __constant float* neighbourhoodSize)
{
	size_t globalId = get_global_id(0); // current agent id in workgroup
	float3 up = (float3)(0.0f, 1.0f, 0.0f);
	
	//Create neighbours array
	Array neighbours;
	InitArray(&neighbours);
	GetNeighbours(previousPos, previousPos[globalId], neighbourhoodSize[0], &neighbours, globalId);// register neighbour indices
	
	float3 linearVelocity = (float3)(0.0f, 0.0f, 0.0f);
	//Cohesion
	float3 averageNeighbourLoc = GetAverageNeighbourPos(previousPos, &neighbours);
	float3 cohesionVelocity = Seek(averageNeighbourLoc, previousPos[globalId]);
	
	linearVelocity += fast_normalize(cohesionVelocity) * weights[0];//set linear velocity
	
	//Allignment
	float3 allignmentVelocity = GetAverageNeighbourVelocity(currentVelocity, &neighbours);
	allignmentVelocity = fast_normalize(allignmentVelocity);
	
	linearVelocity += allignmentVelocity * weights[1];// add to linear velocity
	
	//Seperation
	float3 seperationVelocity = Seperation(previousPos, &neighbours, previousPos[globalId]);
	
	linearVelocity += normalize(seperationVelocity) * weights[2];// add to linear velocity
	
	float3 direction = normalize(linearVelocity);
	linearVelocity = direction * maxSpeed[0] * time[0];
	//linearVelocity = (float3)(1.0f, 0.0f, 0.0f) * time[0];
	
	barrier(CLK_GLOBAL_MEM_FENCE); // all previous must be completed on all work items	
	
	
	//change global values
	currentVelocity[globalId] = linearVelocity;
	previousPos[globalId] += linearVelocity;
	
	//translate matrix
	transform[globalId].sCDE = previousPos[globalId];
	
	//rotate matrix
	float angle = acos(dot(up, direction));
	float3 axis = cross(direction, up);
	if(angle > FLT_EPSILON || angle < -FLT_EPSILON)
		transform[globalId] = Rotate(transform[globalId], angle, axis);


	//FreeArray(&neigbours);
}

// Get indices of neigbours
void GetNeighbours(__global float3* previousPos, float3 currPos, float neighbourhoodSize, Array* neighbours, int globalId)
{
	for(int i = 0; i != 1000; ++i)
	{
		if(i != globalId && fast_distance(currPos, previousPos[i]) <= neighbourhoodSize)
		{
			InsertArray(neighbours, i);// insert index in array
			if(neighbours->used == 200)
				return;
		}

	}
	
}

float3 GetAverageNeighbourPos(float3* previousPos, Array* neighbours)
{
	float3 average = (float3)(0.0f, 0.0f, 0.0f);
	if(neighbours->used < 1)
	{
		return average;
	}
	
	for(int i = 0; i != neighbours->used; ++i)
	{
		average = average + previousPos[neighbours->array[i]];
	}
	average = average / neighbours->used;
	return average;
}

float3 GetAverageNeighbourVelocity(float3* currentVelocity, Array* neighbours)
{
	float3 average = (float3)(0.0f, 0.0f, 0.0f);
	if(neighbours->used < 1)
	{
		return average;
	}
	
	for(int i = 0; i != neighbours->used; ++i)
	{
		average = average + currentVelocity[neighbours->array[i]];
	}
	average = average / neighbours->used;
	return average;
}

// returns normalized linear velocity
float3 Seek(float3 target, float3 pos)
{
	float3 linearVelocity = target - pos;
	linearVelocity = fast_normalize(linearVelocity);
	return linearVelocity;
}

float3 Seperation(float3* previousPos, Array *neighbours, float3 pos)
{
	float3 linearVelocity = (float3)(0.0f, 0.0f, 0.0f);
		if(neighbours->used < 1)
	{
		return linearVelocity;
	}
	
	float3 steeringCurrentNeighbour;
	for(int i = 0; i != neighbours->used; ++i)
	{
		steeringCurrentNeighbour = -(previousPos[neighbours->array[i]]- pos);
		steeringCurrentNeighbour /= fast_distance(previousPos[neighbours->array[i]], pos); // longer distance -> smaller velocity
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
void InitArray(Array* a)
{
	a->used = 0;
}
void InsertArray(Array* a, int element) 
{
  if (a->used == 50) 
  {
	  return;
  }
  
  a->array[a->used] = element;
  a->used += 1;
}

//void FreeArray(Array *a) {
 // free(a->array);
//  a->array = NULL;
//  a->used = a->size = 0;
//}
