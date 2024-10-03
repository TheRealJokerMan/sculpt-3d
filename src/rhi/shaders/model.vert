//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#version 450
#extension GL_EXT_scalar_block_layout : require

#include "uniforms.hxx"

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_colour;

layout (location = 0) out vec3 out_world;
layout (location = 1) out vec3 out_colour;

layout (set = 0, binding = 0, std430) uniform Camera {
    CameraUniform u_camera;
};

layout (set = 0, binding = 1, std430) uniform Model {
    ModelUniform u_model;
};

void main() 
{
    vec3 pos_object = in_position;
    vec3 pos_world = vec3(u_model.model * vec4(pos_object, 1.0));

    out_colour = in_colour;
    out_world = pos_world;
    gl_Position = u_camera.projection * vec4(pos_world, 1.0);
}
