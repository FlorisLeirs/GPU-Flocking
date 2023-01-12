# GPU-Flocking
## What is flocking?
Flocking is an algorithm that combines a set of simple individual behaviours that result in group behaviour that looks like a flock of birds. The behaviour of boids(agents) is influenced by other nearby boids. 


https://user-images.githubusercontent.com/49953730/211052802-4010bd0d-23ff-413b-b986-0d60a188948c.mp4



### Why GPU?
Even though a GPU is typically used for graphics it can be useful for other things too. GPGPU or General-purpose computing on graphics processing units is an algorithm that uses the GPU for tasks that you would normally do on the CPU. The reason using this algorithm can make a very big difference in performance is because of how a GPU works. GPUs have a lower frequency than CPUs, but they have a lot **more cores**. The advantage of having a lot of cores is being able to use **parallel processing** on these cores, just like graphics data. Another advantage of the GPU is that, even though there usually is a lot less memory available, the memory is faster to access in comparison with a CPU that needs to access data stored in RAM. 
This parallel proccessing is useful for flocking when there is a large amount of agents. If the CPU needs to handle for example 20,000 agents it would take a lot of time to calculate everything because even though the CPU has a high frequency the calculations for all of the agents happen one after the other or only a few parallel to each other. Because of the large amount of cores in the GPU the program can execute a lot of commands in parallel. 

![04](https://user-images.githubusercontent.com/49953730/211055561-efd36223-824c-4c0c-a0e4-f3562a94fb0c.gif)

However when only using a low amount of agents using the GPU will negatively impact your performance. This is due to the time it takes to read data from the GPU with the CPU.


## Implementation
In my Flocking implementation I combine four different behaviours. 

-**Cohesion**: Makes the boids move to the average position of all boids in their neighbourhood.

-**Alignment**: Gives boids the average velocity of all boids in their neighbourhood.

-**Separation**: Makes boids move away from other nearby boids in their neighbourhood. The velocities are divided by the distance between the boids to make this behaviour stronger for boids that are close.

-**Wander**: Generates a random linear velocity for the boid.

Each behaviour has a certain *weight*. The normalized velocity of a behaviour is multiplied by that weight before the behaviours are added to each other.

Instead of doing my implementation in an engine like Unity, I chose to do my own thing. I used **OpenCL** as a framework because it is a cross-platform framework that works with most types of processors. For the well known Nvidia CUDA this is not the case, the framework only works with a limited set of Nvidia GPU's that have specialized hardware to further enhance performance. For the graphics part of my implementation I used OpenGL with GLFW. I chose to give all boids a colour based on their position to make differentiating boids easier.

![06](https://user-images.githubusercontent.com/49953730/211106256-4a37bbe7-27d2-43f4-a670-b55d0caf8f93.gif)

### OpenCL
OpenCL uses a kernel *language based on C* which is used by developers to write the progams that need to be executed on accelerator devices(GPU, CPU, FPGA...). These kernels are *compiled by the host CPU*. The host CPU is the CPU on which the main program is running. The host CPU can then use a *command queue* to let a device execute a kernel. The arguments for the kernels also need to be set by the host CPU. If an argument is an array the data is passed through **buffers**, for scalars a buffer is not needed. 
When the host CPU tells the device to execute a kernel it is also important to tell the kernel how many work-items there should be. All the work items are divided into work groups. The size of these groups can be specified by the programmer, but the compiler can also automatically do this. In every work group the work items execute everything in parallel. This means different work groups are not synchronized. Work groups are also important for memory, OpenCL uses different types of memory. There is global memory which is accessible by all work items, then there is local memory which is separate for every work group, there is also private memory which is only accessible by the work item it was created on and constant memory which is globally accessible read-only memory. The fact that global memory can be changed from any work item but synchronization is only possible within the same work-group could lead to a problem. To be sure that the velocity and position remained the same throughout the calculations I made a second kernel. In this kernel I change the values used in the calculations to the newly calculated values. These newly calculated values are stored in different data members that aren't involved in any calculations.

I created a UI to make the behaviour weights, speed and neighbourhood radius adjustable. Every update, delta time and two random generated values, used for the wander behaviour, are also changed. After the kernel code has been executed the CPU reads out a **transform matrix for each boid**(vector of floats). In the render function this vector is then passed on to the renderer which uses only the translation part of the matrices. This is because for a reason I haven't found, it doesn't work if I use float3 values in my kernel. Strangely the performance impact of having to read a lot more floats every update is not that big. 
OpenCL C does not allow any dynamic memory allocation which was a problem to register neighbours. Every boid now has an array with 500 elements to store the indices of its neighbours.

## Final result
I started on this project with a goal of being able to have a framerate of 60fps with 20,000 boids. I am very happy to be able to say that my implementation has far surpassed what I hoped the result would be. As you can see in the graph beneath **20,000 boids runs at ~197fps**.

![image](https://user-images.githubusercontent.com/49953730/211105287-1be126c7-2352-4f17-8410-c0a8a9de37d7.png)

I found that when I used a lot of boids some I obtained some visually stunning results. For the following examples I used respectively 100,000 boids, 200,000 boids and 200,000 boids.

![PaintSplat_01](https://user-images.githubusercontent.com/49953730/211105778-e829d1de-342e-4af8-aea2-039762a29cfb.gif)

![Capture_02](https://user-images.githubusercontent.com/49953730/211105889-32b520c9-e534-4dd5-8b10-cdba0804666e.PNG)

![200k_04](https://user-images.githubusercontent.com/49953730/211107415-b379cae5-5f30-42bf-aebd-7c52d78d4085.gif)


## Future work
There still some improvements that could make a big difference. Because of the limited time I had for this project I wasn't able to implement spatial partitioning. Another improvement I looked into was coupling OpenCL and OpenGL. This way they would use the same buffer, that means that instead of having the CPU read the data and then send it back to the GPU for OpenGL the data could just stay in the GPU.

There is also always the possibility to add some more behaviours into the mix.

## Articles
[Introduction to GPGPU for AI](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter45_Introduction_to_GPGPU_for_AI.pdf) by Conan Bourke and Tomasz Bednarz
