
#version 330 core

in vec3 Normal;
out vec4 FragColor;

void main(){
    vec3 baseColor=vec3(0.5, 0.5, 0.5);
    vec3 lightDir=normalize(vec3(-0.5,1.0,0.3));

    vec3 N=normalize(Normal);

    float diffuse=max(dot(N,lightDir),0.0);
    float ambient=0.20;
    float brightness=ambient+0.80*diffuse;

    FragColor=vec4(baseColor*brightness,1.0);
}
