#version 330                                                                  
                                                                              
layout (location = 0) in vec3 vPosition;															  
layout (location = 1) in vec4 vColor;																  
out vec4 color;												
                                                                               
void main()                                                                     
{                                                                                
    gl_Position = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);				
	color = vColor;																
}