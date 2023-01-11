// array struct
typedef struct {
  int array[500];
  size_t used;
} Array;
void InitArray(Array* a);
void InsertArray(Array* a, int element);
// forward declarations
void GetNeighbours(float3* previousPos, float3 currPos, float neighbourhoodSize, Array* neighbours, int globalId);
float3 GetAverageNeighbourPos(float3* previousPos, Array* neighbours);
float3 GetAverageNeighbourVelocity(float3* prevVelocity, Array* neighbours);
float3 Seek(float3 target, float3 pos);
float3 Seperation(float3* previousPos, Array* neighbours, float3 pos);
float16 Rotate(float16 transformMatrix, float angle, float3 axis);
float3 Wander(float3 previousPos, float3 prevVelocity, uint2 randoms, size_t globalId);

//weights: [0] = cohesion, [1]=allignment, [2]=seperation, [3]=wander
__kernel void Flocking(__global float3* previousPos, __global float3* prevVelocity, __global float* weights, float time, __global uint2* randoms, float speed, float neighbourhoodSize, float maxPos, __global float16* transform, __global float3* currentVelocity)
{
	size_t globalId = get_global_id(0); // current agent id
	
	//Create neighbours array
	private Array neighbours;
	InitArray(&neighbours);
	GetNeighbours(previousPos, previousPos[globalId], neighbourhoodSize, &neighbours, globalId);// register neighbour indices
	
	float3 linearVelocity = (float3)(0.0f, 0.0f, 0.0f);
	//Cohesion
	float3 averageNeighbourLoc = GetAverageNeighbourPos(previousPos, &neighbours);
	float3 cohesionVelocity = Seek(averageNeighbourLoc, previousPos[globalId]);
	
	linearVelocity += fast_normalize(cohesionVelocity) * weights[0];//set linear velocity
	
	//Allignment
	float3 allignmentVelocity = GetAverageNeighbourVelocity(prevVelocity, &neighbours);
	allignmentVelocity = fast_normalize(allignmentVelocity);
	
	linearVelocity += allignmentVelocity * weights[1];// add to linear velocity
	
	//Seperation
	float3 seperationVelocity = Seperation(previousPos, &neighbours, previousPos[globalId]);
	
	linearVelocity += fast_normalize(seperationVelocity) * weights[2];// add to linear velocity
	
	//Wander
	float3 wanderVelocity = Wander(previousPos[globalId], prevVelocity[globalId], randoms[0], globalId);
	
	linearVelocity += wanderVelocity * weights[3];
	
	linearVelocity /= weights[0] + weights[1] + weights[2] + weights[3];
	float3 direction = normalize(linearVelocity);
	linearVelocity = direction * time * speed;
	
	float3 newPos = previousPos[globalId] + linearVelocity;
	// X
	if(newPos.x > maxPos)
	{
		newPos.x = -maxPos;
	}
	else if(newPos.x < -maxPos)
	{
		newPos.x = maxPos;
	}
	// Y
	if(newPos.y > maxPos)
	{
		newPos.y = -maxPos;
	}
	else if(newPos.y < -maxPos)
	{
		newPos.y = maxPos;
	}
	// Z
	if(newPos.z > maxPos)
	{
		newPos.z = -maxPos;
	}
	else if(newPos.z < -maxPos)
	{
		newPos.z = maxPos;
	}
	
	barrier(CLK_GLOBAL_MEM_FENCE); // all previous must be completed on all work items in work group
	
	//change global values
	currentVelocity[globalId] = linearVelocity;
	transform[globalId].sCDE = newPos;
	
	//rotate matrix
	//float angle = acos(dot(up, direction));
	//float3 axis = cross(direction, up);
	//if(angle > FLT_EPSILON || angle < -FLT_EPSILON)
	//	transform[globalId] = Rotate(transform[globalId], angle, axis);
}

__kernel void SetData(__global float3* previousPos, __global float3* prevVelocity, __global float16* transform, __global float3* currentVelocity)
{
	size_t globalId = get_global_id(0); // current agent id
	
	previousPos[globalId] = transform[globalId].sCDE;
	prevVelocity[globalId] = currentVelocity[globalId];
}

// Get indices of neigbours
void GetNeighbours(float3* previousPos, float3 currPos, float neighbourhoodSize, Array* neighbours, int globalId)
{
	for(int i = 0; i != 1000; ++i)
	{
		if(i != globalId && fast_distance(currPos, previousPos[i]) <= neighbourhoodSize)
		{
			InsertArray(neighbours, i);// insert index in array
			if(neighbours->used == 500)
				return;
		}

	}
	
}

// average position in previous frame
float3 GetAverageNeighbourPos(float3* previousPos, Array* neighbours)
{
	float3 average = (float3)(0.0f, 0.0f, 0.0f);
	if(neighbours->used < 1)
	{
		return average;
	}
	
	for(int i = 0; i != neighbours->used; ++i)
	{
		average += previousPos[neighbours->array[i]];
	}
	average = average / neighbours->used;
	return average;
}

// Average velocity in previous frame
float3 GetAverageNeighbourVelocity(float3* prevVelocity, Array* neighbours)
{
	float3 average = (float3)(0.0f, 0.0f, 0.0f);
	if(neighbours->used < 1)
	{
		return average;
	}
	
	for(int i = 0; i != neighbours->used; ++i)
	{
		average += prevVelocity[neighbours->array[i]];
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

// Seperation behaviour
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
		steeringCurrentNeighbour = -(previousPos[neighbours->array[i]] - pos);
		steeringCurrentNeighbour /= fast_distance(previousPos[neighbours->array[i]], pos); // longer distance -> smaller velocity
		linearVelocity += steeringCurrentNeighbour;
	}
	
	linearVelocity = fast_normalize(linearVelocity);
	return linearVelocity;
}

// Wander behaviour
float3 Wander(float3 previousPos, float3 prevVelocity, uint2 randoms, size_t globalId)
{
	int radius = 20.0f;
	int offset = 15.0f;
	// generate random number 
	//https://stackoverflow.com/questions/9912143/how-to-get-a-random-number-in-opencl
	uint seed = randoms.x + globalId;
	uint t = seed ^ (seed << 11);  
	uint result = randoms.y ^ (randoms.y >> 19) ^ (t ^ (t >> 8));
	
	// get angle
	float angle = result % 30 - 15;
	angle = radians(angle);
	// get z coordinate
	int z = result % 30 - 15;
	
	// Create target position
	float3 target = (float3)(cos(angle), sin(angle), z);
	target = normalize(target) * radius;
	target += previousPos + normalize(prevVelocity) * offset;
	
	
	float3 linearVelocity = Seek(target, previousPos);
	return linearVelocity;
	
}


//Rotation matrix
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
	
	transformMatrix.s0123 = (float4)(1.0f,0.0f,0.0f,0.0f);
	transformMatrix.s4567 = (float4)(0.0f,1.0f,0.0f,0.0f);
	transformMatrix.s89ab = (float4)(0.0f,0.0f,1.0f,0.0f);
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
  if (a->used == 500) 
  {
	  return;
  }
  a->array[a->used++] = element;

}
