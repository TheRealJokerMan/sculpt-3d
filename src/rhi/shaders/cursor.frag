//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#version 450
#extension GL_EXT_scalar_block_layout : require

#include "uniforms.hxx"

layout (location = 0) in vec3 in_world;

layout (location = 0) out vec4 out_colour;

layout (set = 0, binding = 0, std430) uniform Camera {
    CameraUniform u_camera;
};

void main()
{
    vec4 colour = vec4(0.0, 0.0, 0.0, 1.0);
    out_colour = colour;
} 
