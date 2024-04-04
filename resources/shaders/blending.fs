#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    vec3 darkerColor = vec3(0.9, 0.9, 0.9); // You can adjust these values to control the darkness
    vec3 finalColor = texColor.rgb * darkerColor;
    if(texColor.a < 0.6)
        discard;
    FragColor = vec4(finalColor, texColor.a);

    }