#include "stdafxppapi.h" 
typedef struct 
{ 
	// Handle to a program object 
	GLuint programObject; 
} UserData; 

// Create a shader object, load the shader source, and 
// compile the shader. 
// 
GLuint LoadShader(const char *shaderSrc, GLenum type) 
{ 
	GLuint shader; 
	GLint compiled; 

	// Create the shader object 
	shader = glCreateShader(type); 
	if(shader == 0) 
		return 0; 
	// Load the shader source 
	glShaderSource(shader, 1, &shaderSrc, NULL); 

	// Compile the shader 
	glCompileShader(shader); 
	// Check the compile status 
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled); 
	if(!compiled)   
	{ 
		glDeleteShader(shader); 
		return 0; 
	} 
	return shader; 
} 
/// 
// Initialize the shader and program object 
// 
int Init() 
{ 
	UserData *userData = esContext->userData; 
	GLbyte vShaderStr[] =    
		"attribute vec4 vPosition;    \n"  
		"void main()                  \n" 
		"{                            \n" 
		"   gl_Position = vPosition; \n" 
		"}                            \n"; 

	GLbyte fShaderStr[] =    
		"precision mediump float;                    \n" 
		"void main()                                 \n" 
		"{                                           \n" 
		"  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n" 
		"}                                           \n"; 
	GLuint vertexShader; 
	GLuint fragmentShader; 
	GLuint programObject; 
	GLint linked; 
	// Load the vertex/fragment shaders 
	vertexShader = LoadShader(vShaderStr, GL_VERTEX_SHADER); 
	fragmentShader = LoadShader(fShaderStr, GL_FRAGMENT_SHADER); 
	// Create the program object 
	programObject = glCreateProgram(); 
	if(programObject == 0) 
		return 0; 
	glAttachShader(programObject, vertexShader); 
	glAttachShader(programObject, fragmentShader); 
	// Bind vPosition to attribute 0     
	glBindAttribLocation(programObject, 0, "vPosition"); 
	// Link the program 
	glLinkProgram(programObject); 
	// Check the link status 
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked); 
	if(!linked)   
	{ 
		glDeleteProgram(programObject);  
		return -1; 
	} 
	// Store the program object 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
	return programObject; 
} 
/// 
// Draw a triangle using the shader pair created in Init() 
// 
void Draw(GLuint programObject) 
{ 
	GLfloat vVertices[ ] = {0.0f,  0.5f, 0.0f,   
		-0.5f, -0.5f, 0.0f, 
		0.5f, -0.5f,  0.0f}; 

	// Set the viewport 
	glViewport(0, 0, 1000, 1000); 

	// Clear the color buffer 
	glClear(GL_COLOR_BUFFER_BIT); 
	// Use the program object 
	glUseProgram(userData->programObject); 
	// Load the vertex data 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices); 
	glEnableVertexAttribArray(0); 
	glDrawArrays(GL_TRIANGLES, 0, 3); 
	eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface); 
} 
int main(int argc, char *argv[ ]) 
{ 
	ESContext esContext; 
	UserData  userData; 
	esInitialize(&esContext); 
	esContext.userData = &userData; 
	esCreateWindow(&esContext, "Hello Triangle", 320, 240, 
		ES_WINDOW_RGB); 

	if(!Init(&esContext)) 
		return 0; 
	esRegisterDrawFunc(&esContext, Draw); 

	esMainLoop(&esContext);  
	翻译：  江湖游侠  QQ；86864472   email：mazhaoyang2005@gmail.com 
		本翻译花费作者 3 个月有余，能力有限，错误在所难免，欢迎交流、指出错误，转载请注明出处， 
}