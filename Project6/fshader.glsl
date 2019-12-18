#version 330
in vec2 UV; 
in vec3 Normal0;    
in vec4 Color;
in vec3 vPosition1;
in vec3 vPosition2;

out vec4 FragColor;   
//光照 
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
	//环境光
    vec4 AmbientColor =gDirectionalLight.AmbientIntensity * vec4(gDirectionalLight.Color, 1.0f) * vColor;                         
	
	//漫反射                                                                                   
    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);     
   
    vec4 lightDir = normalize(gDirectionalLight.lightPos - vec4(vPosition2,0.0));
	vec4 normal = normalize(vec4(Normal0, 0.0));
	float diffFactor = max(dot(lightDir, normal),0.0);
	vec4 DiffuseColor = diffFactor * vec4(gDirectionalLight.Color,1) * vColor;

	/*
		vec4 DiffuseColor;                                                                
		if (DiffuseFactor > 0) {                                                        
			DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) * gDirectionalLight.DiffuseIntensity *DiffuseFactor;                                               
		} else {                                                                          
			DiffuseColor = vec4(0, 0, 0, 0);                                            
		} 
	*/     
	if(isCube==1){//立方体
		FragColor = vColor*(AmbientColor + DiffuseColor);
		//FragColor = vec4(1,1,1,1);
		return;
	}else if(isCube == 2){//地面
		if(vPosition1.y<0.4){
			float x = UV.x;
			float y = UV.y;
			x= x+ test;
			if(x>1){x = x-1;}
			vec2 t = vec2(x,y);
			FragColor = texture2D(myTexture, t)*(AmbientColor + DiffuseColor);	
		}else{
			FragColor = texture2D(myTexture2, UV.xy)*(AmbientColor + DiffuseColor) ;
		} 	
	} else {//天空盒 0
		FragColor = texture2D(myTexture, UV.xy)*(AmbientColor + DiffuseColor) ;
	}
}