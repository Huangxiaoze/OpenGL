#version 330 
in vec3 vPosition;
in vec2 vertexUV; 
in vec3 Normal;
in int vtype;
in vec4 color;

out vec2 UV; 
out vec3 Normal0;
out vec4 Color;
out vec3 vPosition1;
out vec3 lightPos_new;

uniform vec3 lightPos;
uniform mat4 matrix; 
uniform mat4 gWorld;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	
  // gl_Position = matrix * vec4(vPosition, 1.0);
   
   mat4 modelViewProj = projMatrix*viewMatrix* modelMatrix;

   gl_Position = modelViewProj*vec4(vPosition,1.0);

    UV = vertexUV;
	vPosition1 = vPosition;	
	Color = color;
    Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;


	vPosition1 = (modelMatrix * vec4(vPosition, 1.0)).xyz; // 在世界坐标系中指定
	mat4 normalMatrix = mat4(transpose(inverse(modelMatrix)));
	Normal0 = (normalMatrix * vec4(Normal,0.0)).xyz; // 计算法向量经过模型变换后值

	lightPos_new = lightPos;

}
