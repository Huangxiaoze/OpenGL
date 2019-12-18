#version 330 
in vec3 vPosition;
in vec2 vertexUV; 
in vec3 Normal;
in int vtype;
in vec4 color;

uniform mat4 matrix; 
uniform mat4 gWorld;

out vec2 UV; 
out vec3 Normal0;
out vec4 Color;
out vec3 vPosition1;
out vec3 vPosition2;
void main()
{
    gl_Position = matrix * vec4(vPosition, 1.0);
    UV = vertexUV;
	vPosition2 = (gWorld * vec4(vPosition, 1.0)).xyz;
	vPosition1 = vPosition;	
	Color = color;
	Normal0 = (transpose(inverse(gWorld))*vec4(Normal,0.0)).xyz;
}
