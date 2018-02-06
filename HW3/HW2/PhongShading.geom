#version 150 compatibility

layout(triangles) in;
layout(triangle_strip, max_vertices=500) out;

uniform int lightNumber;
uniform int level;
uniform float Radius = 1.0f;

in vec3 EyeSpace[];
in vec3 normal[];
in vec4 TexCoord[];

vec3 v0, v01, v02;
vec4 avg_coord;
vec4 Centroid = vec4(0,1,0,1);

out vec3 vertexEyeSpace;
out vec3 norm;
out vec4 texcoord;

void vertex_coord(float s, float t)
{
	if( s == 0 )
		avg_coord = (1 - t) * TexCoord[0] + t * TexCoord[2];			
	else if( t == 0 )
		avg_coord = (1 - s) * TexCoord[0] + s * TexCoord[1];	
	else if( s + t == 1 )
		avg_coord = s * TexCoord[1] + t * TexCoord[2];	
	else
		avg_coord = (1 - s - t) * TexCoord[0] + s * TexCoord[1] + t * TexCoord[2];
}

void ProduceVertex( float s, float t )
{
	vec3 v = v0 + s*v01 + t*v02;
	
	vertex_coord(s,t);	
	//_texcoord = t0 + s*t1 + t*t2;
		
	norm = normalize( v - (Centroid).xyz);
	vertexEyeSpace = norm + (gl_ModelViewMatrix * Centroid).xyz;;
	
	//vec3 n = v;	
	//vec4 ECposition = gl_ModelViewMatrix * vec4( (Radius*v), 1. );

	gl_Position = gl_ProjectionMatrix * vec4(vertexEyeSpace,1.0f);
	//vertexEyeSpace = ECposition.xyz;
	
	texcoord = avg_coord;
	
	EmitVertex( );
}

void main(){
    
	v01 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
	v02 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;
	v0 = gl_in[0].gl_Position.xyz;
	
	int numlayer = 1 << level;
	float dt = 1.0 / float(numlayer);
	float t_top = 1.0;	
	
	for(int k = 0; k < numlayer; k++)
	{
		float t_bot = t_top - dt;
		float smax_top = 1.0 - t_top;
		float smax_bot = 1.0 - t_bot;
		
		int nums = k + 1;
		float ds_top = smax_top / float(nums - 1);
		float ds_bot = smax_bot / float(nums);
		
		float s_top = 0;
		float s_bot = 0;
		
		for(int x = 0; x < nums; x++)
		{
			ProduceVertex(s_bot, t_bot);
			ProduceVertex(s_top, t_top);
			
			
			//new_vertex(s_bot, t_bot);
			//new_vertex(s_top, t_top);
			s_top += ds_top;
			s_bot += ds_bot;
		}
		ProduceVertex(s_bot, t_bot);
		
		//new_vertex(s_bot, t_bot);
		EndPrimitive();
		
		t_top = t_bot;
	}	
}
