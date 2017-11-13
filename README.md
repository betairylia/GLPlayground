# GLPlayground
Ananannother happy happy playground >w☆（kira

The low-level APIs are just tooooooooo annoying... (Vulkan DX12 etc.)  
They are F&^@$KING my brain...  
I need warm warm OpenGL to play with lol  

## CS_Voxel Drawer
A simple voxel engine that uses compute shader to render the voxels.  
here is an animated GIF:  
![](CS_VoxelDrawer/animated.gif)

**Compile**  
dep_glew_glfw_glut.zip was compiled for Visual Studio 2015 x64.  
You need extract that zip file in root folder. (**NOT** something like dep_glew_glfw_glut/...)  
If you have other compilers, maybe you need complie those librarys from their own source.  
you may need copy the .dll s from glut/bin and glew/bin.

Block Count = (10 x 10) x (32 x 32 x 32) = 320 (X) x 32 (Y) x 320 (Z)

It gets ~40FPS on my Razer 2017 ( GTX1060 6GB - 7700HQ - 16GB RAM ),  
and gets ~100FPS on my desktop in my lab ( 1 of 3x GTX TITAN X 12GB - ? - 32GB RAM ).

>Workflow:  
CPU computes chunk data (with OpenMP),  
Submit it to GPU,  
GPU calculate the block position and instance count,  
and finally glDrawArraysIndirect().
