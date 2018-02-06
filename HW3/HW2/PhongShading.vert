#version 150 compatibility

out vec3 EyeSpace;
out vec3 normal;
out vec4 TexCoord;

void main()
{
	EyeSpace = vec3(gl_ModelViewMatrix * gl_Vertex);
    normal = normalize(gl_NormalMatrix * gl_Normal);    	
	gl_Position =  gl_Vertex;
	TexCoord = gl_MultiTexCoord0;
}