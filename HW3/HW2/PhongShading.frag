#version 150 compatibility

in vec3 vertexEyeSpace;
in vec3 norm;
//in vec2 _texcoord;
in vec4 texcoord;
uniform sampler2D textureUnit;


void main(){
    vec4 Ia, Id, Is;
    vec4 finalFragColor = vec4(0.0f);
    vec3 N = normalize(norm);
    vec3 V = normalize(-vertexEyeSpace); // in eye space, eye position is (0, 0, 0).

    for(int i = 0; i < 2; ++i){
        vec3 L = normalize(gl_LightSource[i].position.xyz - vertexEyeSpace);
        vec3 R = normalize(reflect(-L, N));

        // ambient = Ia * Ka
        Ia = gl_LightSource[i].ambient * gl_FrontMaterial.ambient;

        // diffuse = Id * Kd * (N * L)
        Id = gl_LightSource[i].diffuse * gl_FrontMaterial.diffuse * max(dot(N, L), 0.0f);

        // specular = Is * Ks * (R * V)^ns
        Is = gl_LightSource[i].specular * gl_FrontMaterial.specular * pow(max(dot(R, V), 0.0f), gl_FrontMaterial.shininess);

        finalFragColor += Ia + Id + Is;
    }

    gl_FragColor = vec4(finalFragColor) * texture(textureUnit,texcoord.xy);
}
