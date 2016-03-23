#pragma once

#include <assert.h>
#include <EGL/egl.h>
#include <gles2/gl2.h>
class    ShaderId
{
public:
	ShaderId()
	{
		_shaderId = -1;
	}
	int _shaderId;
};


/**
*   ³ÌÐò
*/
class   ProgramId
{
public:
	int         _programId;
	ShaderId    _vertex;
	ShaderId    _fragment;
public:
	ProgramId()
	{
		_programId = -1;
	}
public:
	/**
	*   ¼ÓÔØº¯Êý
	*/
	bool    createProgram(const char* vertex, const char* fragment)
	{
		bool        error = false;
		do
		{
			if (vertex)
			{
				_vertex._shaderId = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(_vertex._shaderId, 1, &vertex, 0);
				glCompileShader(_vertex._shaderId);

				GLint   compileStatus;
				glGetShaderiv(_vertex._shaderId, GL_COMPILE_STATUS, &compileStatus);
				error = compileStatus == GL_FALSE;
				if (error)
				{
					GLchar messages[256];
					glGetShaderInfoLog(_vertex._shaderId, sizeof(messages), 0, messages);
					assert(messages && 0 != 0);
					break;
				}
			}
			if (fragment) //ÏñËØ×ÅÉ«Æ÷
			{
				_fragment._shaderId = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(_fragment._shaderId, 1, &fragment, 0);
				glCompileShader(_fragment._shaderId);

				GLint   compileStatus;
				glGetShaderiv(_fragment._shaderId, GL_COMPILE_STATUS, &compileStatus);
				error = compileStatus == GL_FALSE;

				if (error)
				{
					GLchar messages[256];
					glGetShaderInfoLog(_fragment._shaderId, sizeof(messages), 0, messages);
					assert(messages && 0 != 0);
					break;
				}
			}
			//½«vsºÍps¸½¼Óµ½ ×ÅÉ«Æ÷ ¶ÔÏóÉÏÈ¥
			_programId = glCreateProgram();

			if (_vertex._shaderId)
			{
				glAttachShader(_programId, _vertex._shaderId);
			}
			if (_fragment._shaderId)
			{
				glAttachShader(_programId, _fragment._shaderId);
			}

			glLinkProgram(_programId);

			GLint linkStatus;
			glGetProgramiv(_programId, GL_LINK_STATUS, &linkStatus);
			if (linkStatus == GL_FALSE)
			{
				GLchar messages[256];
				glGetProgramInfoLog(_programId, sizeof(messages), 0, messages);
				break;
			}
			glUseProgram(_programId);

		} while (false);
		//Ê¹ÓÃdo-while ½øÐÐÍ³Ò»µÄ´íÎó´¦Àí
		if (error)
		{
			if (_fragment._shaderId)
			{
				glDeleteShader(_fragment._shaderId);
				_fragment._shaderId = 0;
			}
			if (_vertex._shaderId)
			{
				glDeleteShader(_vertex._shaderId);
				_vertex._shaderId = 0;
			}
			if (_programId)
			{
				glDeleteProgram(_programId);
				_programId = 0;
			}
		}
		return  true;
	}

	/**
	*   Ê¹ÓÃ³ÌÐò
	*/
	virtual void    begin()
	{
		glUseProgram(_programId);

	}
	/**
	*   Ê¹ÓÃÍê³É
	*/
	virtual void    end()
	{
		glUseProgram(0);
	}
};

class   PROGRAM_P2_C4 :public ProgramId
{
public:
	typedef int attribute;
	typedef int uniform;
public:
	attribute   _position;
	uniform     _color;
	uniform     _MVP;
public:
	PROGRAM_P2_C4()
	{
		_position = -1;
		_color = -1;
		_MVP = -1;
	}
	~PROGRAM_P2_C4()
	{
	}

	/// ³õÊ¼»¯º¯Êý
	virtual bool    initialize()
	{
		const char* vs =
		{
			"precision lowp float; "
			"uniform   mat4 _MVP;"
			"attribute vec2 _position;"

			"void main()"
			"{"
			"   vec4    pos =   vec4(_position,0,1);"
			"   gl_Position =   _MVP * pos;"
			"}"
		};
		const char* ps =
		{
			"precision  lowp float; "
			"uniform    vec4 _color;"
			"void main()"
			"{"
			"   gl_FragColor   =   _color;"
			"}"
		};

		bool    res = createProgram(vs, ps);
		if (res)
		{
			_position = glGetAttribLocation(_programId, "_position");
			_color = glGetUniformLocation(_programId, "_color");
			_MVP = glGetUniformLocation(_programId, "_MVP");
		}
		return  res;
	}

	/**
	*   Ê¹ÓÃ³ÌÐò
	*/
	virtual void    begin()
	{
		glUseProgram(_programId);
		glEnableVertexAttribArray(_position); //启用顶点数组

	}
	/**
	*   Ê¹ÓÃÍê³É
	*/
	virtual void    end()
	{
		glDisableVertexAttribArray(_position);
		glUseProgram(0);
	}
};


//display()使用
class PROGRAM_HeatMap :public ProgramId{
	typedef int attribute;
	typedef int uniform;
public:
	attribute _position;
	uniform _source;
public:
	virtual bool    initialize()
	{
		//heatmap中的着色器
		const char* vs =
		{
			"attribute vec4 position;"
			"varying vec2 texcoord; "
			"void main(){"
			"texcoord = position.xy*0.5 + 0.5;"  //取xy数值的向量
			"gl_Position = position;"
			"}"
		};
		const char* ps =
		{
			"precision mediump int;"
			"precision mediump float;"
			"uniform sampler2D source;"
			"varying vec2 texcoord; "
			"float linstep(float low, float high, float value){"
			"return clamp((value - low) / (high - low), 0.0, 1.0);"
			"}"

			"float fade(float low, float high, float value){"
			"float mid = (low + high)*0.5;"
			"float range = (high - low)*0.5;"
			"float x = 1.0 - clamp(abs(mid - value) / range, 0.0, 1.0);"
			"return smoothstep(0.0, 1.0, x);"
			"}"

			"vec3 getColor(float intensity){"
			"vec3 blue = vec3(0.0, 0.0, 1.0);"
			"vec3 cyan = vec3(0.0, 1.0, 1.0);"
			"vec3 green = vec3(0.0, 1.0, 0.0);"
			"vec3 yellow = vec3(1.0, 1.0, 0.0);"
			"vec3 red = vec3(1.0, 0.0, 0.0);"

			"vec3 color = ("
			"fade(-0.25, 0.25, intensity)*blue +"
			"fade(0.0, 0.5, intensity)*cyan +"
			"fade(0.25, 0.75, intensity)*green +"
			"fade(0.5, 1.0, intensity)*yellow +"
			"smoothstep(0.75, 1.0, intensity)*red"
			");"
			"return color;"
			"}"
			"vec4 alphaFun(vec3 color, float intensity){"
			"float alpha = smoothstep(0.00000000, 1.00000000, intensity);"
			"return vec4(color*alpha, alpha);"
			//"return vec4(color,0.8);"
			"}"

			"void main(){"
			"float intensity = smoothstep(0.0, 1.0, texture2D(source, texcoord).r);"
			"vec3 color = getColor(intensity);"
			//"gl_FragColor = vec4(1.0,0.0,0.0,1.0);"
			"if(intensity < 0.01)"
				"discard;"
			"gl_FragColor = alphaFun(color, intensity);"
			"}"
		};

		bool    res = createProgram(vs, ps);
		if (res)
		{
			//获取变量位置
			_position = glGetAttribLocation(_programId, "position");
			_source = glGetUniformLocation(_programId, "source");
		}
		return  res;
	}

	virtual void    begin()
	{
		glUseProgram(_programId);
		glEnableVertexAttribArray(_position);
	}

	virtual void    end()
	{
		glDisableVertexAttribArray(_position);
		glUseProgram(0);
	}
};

//update()使用
class PROGRAM_Heights :public ProgramId{
	typedef int attribute;
	typedef int uniform;
public:
	attribute _position, _intensity;
	uniform _viewport;
public:
	virtual bool initialize(){
		const char* vs =
		{
			"attribute vec4 position, intensity;"
			"uniform vec2 viewport;"
			"varying vec2 off, dim;"
			"varying float vIntensity;"
			"void main(){"
			"    dim = abs(position.zw);"
			"    off = position.zw;"
			"    vec2 pos = position.xy + position.zw;"
			"    vIntensity = intensity.x;"
			"    gl_Position = vec4((pos/viewport)*2.0-1.0, 0.0, 1.0);"
			"}"
		};
		const char* ps =
		{
			"precision mediump int;"
			"precision mediump float;"
			"varying vec2 off, dim;"
			"varying float vIntensity;"
			"void main(){"
			"float falloff = (1.0 - smoothstep(0.0, 1.0, length(off/dim)));"
			"float intensity = falloff*vIntensity;"
			"gl_FragColor = vec4(intensity);"
			//"gl_FragColor = vec4(1.0,.0,.0,.2);"
			"}"
		};

		bool    res = createProgram(vs, ps);
		if (res)
		{
			//获取变量位置
			_position = glGetAttribLocation(_programId, "position");
			_intensity = glGetAttribLocation(_programId, "intensity");
			_viewport = glGetUniformLocation(_programId, "viewport");
		}
		return  res;
	}

	virtual void begin(){
		glUseProgram(_programId);
		glEnableVertexAttribArray(_position);
		glEnableVertexAttribArray(_intensity);
	}
	virtual void end(){
		glDisableVertexAttribArray(_position);
		glDisableVertexAttribArray(_intensity);
		glUseProgram(0);
	}
};

//聚合图使用
class   PROGRAM_POINT :public ProgramId
{
public:
	typedef int attribute;
	typedef int uniform;
public:
	attribute   _position;

	uniform    _MVP;
	uniform    _pointSize;
	uniform    _texture;
public:
	PROGRAM_POINT()
	{
		_position = -1;
		_MVP = -1;
		_pointSize = -1;
		_texture = -1;
	}
	virtual ~PROGRAM_POINT()
	{}

	virtual void    initialize()
	{
		const char* vs =
		{
			"precision  lowp float;\n"
			"uniform    mat4    _MVP;\n"
			"uniform    float   _pointSize;\n"
			"attribute  vec2    _position;\n"
			"void main()\n"
			"{"
			"   vec4    pos =   vec4(_position,0,1);\n"
			"   gl_Position =   _MVP * pos;\n"
			"   gl_PointSize=   _pointSize;\n"
			"}"
		};
		const char* ps =
		{
			"precision lowp float;\n"
			"uniform    sampler2D   _texture;\n"
			"void main()\n"
			"{\n"
			"   vec4    texColor=   texture2D( _texture, gl_PointCoord );\n" //内置的点的纹理坐标
			"   gl_FragColor    =   texColor;"
			"}"
		};
		createProgram(vs, ps);
		_position = glGetAttribLocation(_programId, "_position");
		_MVP = glGetUniformLocation(_programId, "_MVP");
		_texture = glGetUniformLocation(_programId, "_texture");
		_pointSize = glGetUniformLocation(_programId, "_pointSize");
	}

	virtual void    begin()
	{
		glUseProgram(_programId);
		glEnableVertexAttribArray(_position);
	}

	virtual void    end()
	{
		glDisableVertexAttribArray(_position);
		glUseProgram(0);
	}
};

//渲染文字
class   CELLUIProgram :public ProgramId
{
	typedef int     location;
protected:
	/**
	*   属性 attribute
	*/
	location    _position;
	location    _color;
	location    _uv;
	/**
	*   uniform
	*/
	location    _MVP;
	location    _texture;
public:
	CELLUIProgram()
	{
		_position = -1;
		_color = -1;
		_uv = -1;
		_texture = -1;
		_MVP = -1;
	}
	virtual ~CELLUIProgram()
	{}

	location    getPositionAttribute() const
	{
		return  _position;
	}
	location    getColorAttribute() const
	{
		return  _color;
	}
	location    getUV1Attribute() const
	{
		return  _uv;
	}
	location    getMVPUniform() const
	{
		return  _MVP;
	}

	location    getTexture1Uniform() const
	{
		return  _texture;
	}

	virtual void    initialize()
	{
		const char* vs =
		{
			"precision  lowp float; "
			"uniform    mat4 _MVP;"
			"attribute  vec3 _position;"
			"attribute  vec3 _uv;"
			"attribute  vec4 _color;"

			"varying    vec3 _outUV;"
			"varying    vec4 _outColor;"
			"void main()"
			"{"
			"   vec4    pos =   vec4(_position.x,_position.y,_position.z,1);"
			"   gl_Position =   _MVP * pos;"
			"   _outUV      =   _uv;"
			"   _outColor   =   _color;"
			"}"
		};
		const char* ps =
		{
			"precision  lowp float; "
			"uniform    sampler2D   _texture;"
			"varying    vec4        _outColor;"
			"varying    vec3        _outUV;"
			"void main()"
			"{"
			"   vec4   color   =   texture2D(_texture,vec2(_outUV.x,_outUV.y));"
			"  	if( _outUV.z > 0.0 )\n "
			"		gl_FragColor    =   vec4(_outColor.x,_outColor.y,_outColor.z,color.w * _outColor.w);\n"
			" 	else "
			"       gl_FragColor    =   color * _outColor;"
			"}"
		};

		bool    res = createProgram(vs, ps);
		if (res)
		{
			_position = glGetAttribLocation(_programId, "_position");
			_uv = glGetAttribLocation(_programId, "_uv");
			_color = glGetAttribLocation(_programId, "_color");

			_texture = glGetUniformLocation(_programId, "_texture");
			_MVP = glGetUniformLocation(_programId, "_MVP");
		}


	}
	/**
	*   使用程序
	*/
	virtual void    begin()
	{
		glUseProgram(_programId);
		glEnableVertexAttribArray(_position);
		glEnableVertexAttribArray(_uv);
		glEnableVertexAttribArray(_color);

	}
	/**
	*   使用完成
	*/
	virtual void    end()
	{
		glDisableVertexAttribArray(_position);
		glDisableVertexAttribArray(_uv);
		glDisableVertexAttribArray(_color);
		glUseProgram(0);
	}
};