
#version 330 core

in vec3 Normal;
out vec4 FragColor;

uniform vec3 objectColor;

void main(){
    vec3 lightDir=normalize(vec3(-0.5,1.0,0.3));

    vec3 N=normalize(Normal);

    float diffuse=max(dot(N,lightDir),0.0);
    float ambient=0.20;
    float brightness=ambient+0.80*diffuse;

    FragColor=vec4(objectColor*brightness,1.0);
}
