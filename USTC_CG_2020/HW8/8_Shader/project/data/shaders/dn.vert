#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out VS_OUT {
    vec3 WorldPos;
    vec2 TexCoord;
    mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler2D displacementmap;
uniform float displacement_coefficient;

void main()
{
    // TODO HW8 - 0_displacement_normal | calculate displacement
    // get pixelValue from displacementmap in [0, 1]
    float pixel_value = texture(displacementmap, aTexCoord)[0];
    // displacement = lambda * (0 + 2 * pixel value)
    // newPos = Pos + displacement * Normal
    vec3 displacement_pos = displacement_coefficient * (0 + 2 * pixel_value) * aNormal + aPos;

    vec4 worldPos = model * vec4(displacement_pos, 1.0);
    vs_out.WorldPos = worldPos.xyz / worldPos.w;
	
    vs_out.TexCoord = aTexCoord;
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	
    // TODO HW8 - 0_displacement_normal | calculate TBN
    // world positions of normal, tangent and bitangent
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(cross(N, T));

    //vec3 N = normalize(normalMatrix * aNormal);
    //vec3 T = vec3(0);
	//vec3 B = vec3(0);
	vs_out.TBN = mat3(T, B, N);

    gl_Position = projection * view * worldPos;
}