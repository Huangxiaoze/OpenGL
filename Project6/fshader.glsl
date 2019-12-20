#version 330
in vec2 UV; 
in vec3 Normal0;    
in vec4 Color;
in vec3 vPosition1;
in vec3 lightPos_new;

out vec4 FragColor;   
//���� 
struct DirectionalLight                                                             
{                                                                                   
    vec3 Color;                                                                     
    float AmbientIntensity;                                                         
    float DiffuseIntensity;                                                         
    vec3 Direction; 
	vec4 lightPos;                                                            
};                                                                                   
uniform DirectionalLight gDirectionalLight;       
uniform sampler2D myTexture; 
uniform sampler2D myTexture2;
uniform int isCube;
uniform float depth;
uniform float test;

void main()
{  
	vec4 vColor;
	
	if(isCube!=1){
		vColor = vec4(1,1,1,1);
	}else{
		vColor=Color;
	}
	
	//////////////////////////////////////////////////
	//����

	// TODO ������ά����Ĳ�������
	vec3 ambiColor = vec3(0.2, 0.2, 0.2);
	vec3 diffColor = vec3(0.5, 0.5, 0.5);
	vec3 specColor = vec3(0.3, 0.3, 0.3);

	
	// ������ɷ�
	float   ambientStrength = 0.1f;
	vec3    ambient = ambientStrength * gDirectionalLight.Color * vColor.xyz;
	
	
	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	vec3    lightDir = normalize(lightPos_new - vPosition1);
	vec3    normal = normalize(Normal0);
	float   diffFactor = max(dot(lightDir, normal), 0.0);
	vec3    diffuse = diffFactor * gDirectionalLight.Color * vColor.xyz;


	//FragColor = vec4(1,0,0,1);return;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if(isCube==1){//������
		FragColor = vColor*vec4(ambient+diffuse,1.0);
	}else if(isCube == 2){//����
		if(vPosition1.y<depth-0.01){
			float x = UV.x;
			float y = UV.y;
			x= x+ test;
			if(x>1){x = x-1;}
			vec2 t = vec2(x,y);
			float alph = y;
			FragColor = texture2D(myTexture, t)*vec4(ambient*1.2+diffuse*1.2,1.0)*alph+texture2D(myTexture2, UV.xy)*vec4(ambient+diffuse,1.0)*(1-alph);
		}else{
			FragColor = texture2D(myTexture2, UV.xy)*vec4(ambient+diffuse,1.0);
		} 	
	} else {//��պ� 0
		FragColor =  texture2D(myTexture, UV.xy)*vec4(ambient+diffuse,1.0);
	}
}