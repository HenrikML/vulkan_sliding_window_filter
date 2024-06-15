# Vulkan Sliding Window Filter (WIP)

Author: Henrik Lauronen

Requirements:
- CMake 3.25 or higher
- Vulkan 1.3.256 or higher

## Goals

The aim of this project is to implement a basic image processing filter using the Vulkan API and GLSL shaders.

## Results
### Input:
![Input image](VulkanSWF/VulkanSWF/images/input.jpg)
### 15x15 Box filter:
![Input filtered by a 15x15 box filter](results/output_box.jpg)
### 15x15 Gaussian filter (sigma = 4.0):
![Input filtered by a 15x15 box filter](results/output_gaussian.jpg)
