#pragma once
#include "CELLShader.hpp"
#include <EGL/egl.h>
#include <gles2/gl2.h>
#include "Heights.h"
#include "DataStruct.h"
class DrawEngine;
class HeatMap
{
public:
	HeatMap(int width, int height,DrawEngine *den = NULL) :
	  _quad(0)
	, _width(width)
	, _height(height)
	, den(den)
	{
		_shader.initialize();
		glViewport(0, 0, _width, _height);
		glGenBuffers(1, &_quad);
		GLfloat quad_data[24] = { -1, -1, 0, 1, 1, -1, 0, 1, -1, 1, 0, 1, -1, 1, 0, 1, 1, -1, 0, 1, 1, 1, 0, 1 };
		glBindBuffer(GL_ARRAY_BUFFER, _quad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		_heights = new Heights(_width,_height);
	};
	~HeatMap(){ delete _heights;}
public:
	void display()
	{
		//glViewport(0, 0, _width, _height);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //融合到目标
		//
		glBindBuffer(GL_ARRAY_BUFFER, _quad); //使用VBO
		_heights->nodeFront.bind(0); //激活第一阶段纹理，并绑定纹理ID
		_shader.begin(); 
		{
			glUniform1i(_shader._source, 0); //使用第一阶段纹理
			//glUniformMatrix4fv(_shader._MVP, 1, false, MVP.data()); 全局矩阵没有
			//UV坐标也没有
			//最后两个参数应该是 sizeof(vertex),及各个数据成员的偏移,0表示数据紧靠在一起,在着色器中对顶点数据做了处理
			glVertexAttribPointer(_shader._position, 4, GL_FLOAT,false,0,0); 
			
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		_shader.end();
		glDisable(GL_BLEND);
	}
	void update(){ _heights->update();}
	void clear(){ _heights->clear();}
	//针对地理坐标
	void addPoint(const HeatPoint &point);
	void addPoints(const std::vector<HeatPoint> &pointSet);
	//针对像素坐标
	void addPoint(int x, int y, int size, double intensity){ _heights->addPoint(x, y, size, intensity);}
private:
public:
	PROGRAM_HeatMap _shader;
	GLuint			_quad;
	int				_width;
	int				_height;
	Heights			*_heights;
	DrawEngine		*den;
};

