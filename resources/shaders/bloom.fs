#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

void main()
{
    const float gamma = 0.85;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    if(bloom){
        hdrColor += bloomColor;
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        result = pow(result, vec3(1.0 / gamma));
        float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
        if(brightness > 1.0)
            BrightColor = vec4(result, 1.0);
        else
            BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
        FragColor = vec4(result, 1.0);
    }else{
        vec3 result = pow(hdrColor, vec3(1.0/gamma));
        float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
        if(brightness > 1.0)
            BrightColor = vec4(result, 1.0);
        else
            BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
        FragColor = vec4(result, 1.0);
    }
}