#include "DataStruct.h"


LogFile	gLog(_T("C:\\Users\\Administrator\\Desktop\\log\\update.txt"));

CString UTF8ToGB2312(const char *str)  
{   
	CString result;   
	WCHAR *strSrc = NULL;   
	char *szRes = NULL;   
	int i;  // UTF8转换成Unicode    
	i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);   
	strSrc = new WCHAR[i+1];   
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);  // Unicode转换成GB2312    
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);   
	szRes = new char[i+1]; 
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);    
	result = szRes;   
	if (strSrc != NULL)   
	{    
		delete []strSrc; 
		strSrc = NULL;   
	}  
	if (szRes != NULL)   
	{    
		delete []szRes;    
		szRes = NULL;   
	}    
	return result;  
}  

std::wstring s2ws(const std::string& s) 
{ 
	int len; 
	int slength = (int)s.length() + 1; 
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);  
	wchar_t* buf = new wchar_t[len]; 
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len); 
	std::wstring r(buf); 
	delete[] buf; 
	return r; 
} 


char* WcharToChar(wchar_t* wc)
{
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}
wchar_t* CharToWchar(char* c)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

/*****************全局变量定义*************************/
GLuint g_programObj;
GLuint g_vertexShader;
GLuint g_fragmentShader;

GLuint g_programObj_Shp;
GLuint g_fragmentShader_Shp;
GLuint g_vertexShader_Shp;
GLuint g_positionLoc_Shp;
GLuint g_texCoordLoc_Shp;
GLuint g_colorLoc_Shp;
GLuint g_MVPLoc_Shp;

GLuint g_textureLoc = 0;
GLuint g_textureID = 0;

GLuint  g_positionLoc;
GLuint  g_texCoordLoc;
GLuint  g_colorLoc;
GLuint  g_MVPLoc;
GLuint  g_vboID;
GLuint  g_ibID;

GLuint g_CPvertexShader ;
GLuint g_CPfragmentShader;
GLuint g_programObj_CP;
GLuint g_positionLoc_CP;
GLuint g_colorLoc_CP;
GLuint g_MVPLoc_Cp;
GLuint g_texCoordLoc_CP;
//add by wangyuejiao
GLfloat g_mapTranslationMatrix[16];  
GLuint compileShader(GLenum type, const char *data) 
{
	const char *shaderStrings[1];
	shaderStrings[0] = data;
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, shaderStrings, NULL );
	glCompileShader(shader);
	GLint compiled;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infoLen);
		if (infoLen > 1)
		{
			char* infolog = new char(sizeof(char)*infoLen);
			glGetShaderInfoLog(shader,infoLen,NULL,infolog);
			delete[] infolog;
			infolog = NULL;
		}
		glDeleteShader(shader);
	}
	return shader;
}

FILE *tex_log = NULL;

void initcode()
{
	//栅格顶点着色器
	g_VShaderData = "uniform mat4 a_MVP;"
		"attribute vec2 a_texCoord;"
		//"attribute vec3 a_color;"
		"attribute vec4 a_position;"
		//"varying  vec3 v_color;"
		"varying  vec2 v_texCoord;" //顶点shader的输出
		"void main()" 
		"{"
		"gl_PointSize = 1.0;"
		"gl_Position = a_MVP*a_position;"
		//"v_color = a_color;"
		"v_texCoord = a_texCoord;"
		"}";
	//栅格片段着色器
	g_FShaderData = "precision mediump float;"
		"varying vec3 v_color;"
		"varying vec2 v_texCoord;"
		"uniform sampler2D s_texture;"
		//"uniform float uAlpha;"
		"void main()"
		"{"
		//"gl_FragColor = texture2D( s_texture, vec2(v_texCoord.x, v_texCoord.y) )+ vec4(v_color.x*0.0,v_color.y*0.0,v_color.z*0.0,0);"
		//"gl_FragColor = vec4(1.0,0.5,0.5,1.0);"
		"gl_FragColor = texture2D( s_texture, vec2(v_texCoord.x, v_texCoord.y) );"
		"}";

	//矢量的片元着色
	//矢量的定点着色
	g_VShaderShpData = "uniform mat4 a_MVP;"
		//"attribute vec2 a_texCoord;"
		"attribute vec4 a_color;"
		"attribute vec4 a_position;"
		"varying  vec4 v_color;"             //输出值作为FragmentShader的输入
		//"varying  vec2 v_texCoord;"
		"void main()" 
		"{"
		"gl_PointSize = 1.0;"
		"gl_Position = a_MVP*a_position;"
		"v_color = a_color;"
		//"v_texCoord = a_texCoord;"
		"}";

	g_FShaderShpData = "precision mediump float;"   //设置显示模式
		"varying vec4 v_color;"
		//"varying vec2 v_texCoord;"
		//"uniform sampler2D s_texture;"
		"void main()"
		"{ "
		//"gl_FragColor = texture2D( s_texture, vec2(v_texCoord.x, v_texCoord.y) );"
		//"gl_FragColor = vec4(v_color.x,v_color.y,v_color.z,1.0);"
		"gl_FragColor = v_color;"
		"}";


	//圆点着色器:vertex 与 片段
	g_VShaderCirclePointData = "uniform mat4 a_MVP;"
		//"attribute vec2 a_texCoord;"
		"attribute vec4 a_color;"
		"attribute vec4 a_position;"
		"attribute float pointsize;"    //attribute 表示着色器的input
		"varying  vec4 v_color;"
		//"varying  vec2 v_texCoord;"
		"void main()" 
		"{"
		//"gl_PointSize = 10.0;"
		"gl_PointSize = pointsize;"  // 原来这里的是写死了，现在改用变量，解决了自由绘笔 画线 任意宽度的问题
		"gl_Position = a_MVP*a_position;"
		"v_color = a_color;"
		//"v_texCoord = a_texCoord;"
		"}";

	g_FShaderCirclePointData = "precision mediump float;"
		"varying vec4 v_color;"
		//"varying vec2 v_texCoord;"
		//"uniform sampler2D s_texture;"
		//"uniform float uAlpha;"
		//"vec4 c_color;"
		"void main()"
		"{ "
		"gl_FragColor = v_color;"
		"if(length(gl_PointCoord-vec2(0.5)) > 0.5)"
		"discard;"
		"}";
}

void InitProgram( void )
{
	//glSetCurrentContextPPAPI(g_context);

	/************************part 1:*********************************************************/
	g_vertexShader = compileShader(GL_VERTEX_SHADER, g_VShaderData);//栅格的顶点着色器
	g_fragmentShader = compileShader(GL_FRAGMENT_SHADER, g_FShaderData);
	//初始化着色器：纹理
	g_programObj = glCreateProgram();
	glAttachShader(g_programObj, g_vertexShader);
	glAttachShader(g_programObj, g_fragmentShader);
	glLinkProgram(g_programObj);

	//判断是否连接上
	GLint link;
	glGetProgramiv(g_programObj,GL_LINK_STATUS,&link);
	if (!link)
	{
		glDeleteProgram(g_programObj);
		g_programObj = 0;
		return;
	}
	g_positionLoc = glGetAttribLocation(g_programObj, "a_position");
	g_texCoordLoc = glGetAttribLocation(g_programObj, "a_texCoord");
	//g_colorLoc = glGetAttribLocation(g_programObj, "a_color");
	g_MVPLoc = glGetUniformLocation(g_programObj, "a_MVP");


	/************************part 2:**********************************************/
	//初始化着色器：矢量
	g_vertexShader_Shp = compileShader(GL_VERTEX_SHADER, g_VShaderShpData);//矢量的顶点着色器
	g_fragmentShader_Shp = compileShader(GL_FRAGMENT_SHADER,g_FShaderShpData);
	g_programObj_Shp = glCreateProgram();
	glAttachShader(g_programObj_Shp, g_vertexShader_Shp);
	glAttachShader(g_programObj_Shp, g_fragmentShader_Shp);
	glLinkProgram(g_programObj_Shp);

	//在这里是不是应该加入是否连接上的判断  GLint link

	g_positionLoc_Shp = glGetAttribLocation(g_programObj_Shp, "a_position");
	g_colorLoc_Shp = glGetAttribLocation(g_programObj_Shp, "a_color");
	g_MVPLoc_Shp = glGetUniformLocation(g_programObj_Shp, "a_MVP");
}

HANDLE g_hRenderLineEvent;
HANDLE g_hThreadLineEvent;
HANDLE g_hAddLineEvent;
bool   g_UpdateLine = false;
bool g_qiepian = false;


	