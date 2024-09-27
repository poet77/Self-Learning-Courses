#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out VS_OUT {
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 Normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler2D displacementmap;
uniform float displacement_bias;
uniform float displacement_scale;
uniform float displacement_lambda;
uniform bool have_denoise;

void main()
{
   // TODO: HW8 - 1_denoise | denoise
    vec3 displacement_pos;
    if (have_denoise)
    {
        // get pixelValue from displacementmap in [0, 1]
        float pixel_value = texture(displacementmap, aTexCoord)[0];
        // displacement = lambda * (0 + 1 * pixel value)
        // newPos = Pos + displacement * Normal
        displacement_pos = aPos - displacement_lambda * (displacement_bias + displacement_scale * pixel_value) * aNormal;
    }
    else
        displacement_pos = aPos;
    //vec4 worldPos = model * vec4(aPos, 1.0);
    vec4 worldPos = model * vec4(displacement_pos, 1.0);
	
	vs_out.WorldPos = worldPos.xyz / worldPos.w;
    vs_out.TexCoord = aTexCoord;
    vs_out.Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
	
    gl_Position = projection * view * worldPos;
}
