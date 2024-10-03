//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#version 450
#extension GL_EXT_scalar_block_layout : require

#include "uniforms.hxx"

layout (location = 0) in vec3 in_world;
layout (location = 1) in vec3 in_colour;

layout (location = 0) out vec4 out_colour;

layout (set = 0, binding = 0, std430) uniform Camera {
    CameraUniform u_camera;
};

layout (set = 0, binding = 1, std430) uniform Model {
    ModelUniform u_model;
};

void main()
{
    vec3 pos_world = in_world;

    vec3 dx = dFdx(pos_world);
    vec3 dy = dFdy(pos_world);
    vec3 normal_world = normalize(cross(dy, dx));
    
    vec3 colour = in_colour;

    vec3 light_dir = normalize(u_camera.eye.xyz - pos_world);
    float d = 0.3 + 0.6 * max(0.0, dot(normal_world, light_dir));

    out_colour = vec4(d * colour, 1.0);
} 
