#pragma once
#include "CELLShader.hpp"
#include <vector>
#include "Node.h"


class Heights
{
public:
	Heights(int width, int height) :_width(width), _height(height), vertexSize(8), maxPointCount(1024 * 10)
		, bufferIndex(0)
		, pointCount(0)
		, nodeBack(width, height)
		, nodeFront(width,height)
		, vertexBuffer(0)
	{
		_shader.initialize();
		vertexBufferData = new GLfloat[maxPointCount * vertexSize * 6];
		memset(vertexBufferData,0,(maxPointCount * vertexSize * 6)*sizeof(GLfloat));
		glGenBuffers(1, &vertexBuffer);
		
		int i, _i, _ref;
		for (i = _i = 0, _ref = maxPointCount; 0 <= _ref ? _i < _ref : _i > _ref; i = 0 <= _ref ? ++_i : --_i){
			vertexBufferViews.push_back(i*vertexSize * 6);
		}
	}
	~Heights()
	{
		delete[] vertexBufferData;
		vertexBufferData = NULL;
		glDeleteBuffers(1,&vertexBuffer);
		vertexBuffer = 0;
	};

public:
	void update()
	{
		if (pointCount > 0) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			//成成 并 使用FBO
			nodeFront.bindFBO();
			//生成 并 使用VBO
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, vertexBufferViews[pointCount] * 4, &vertexBufferData[0], GL_STREAM_DRAW);
			_shader.begin(); 
			{
				glUniform2f(_shader._viewport, _width, _height);
				glVertexAttribPointer(_shader._position, 4, GL_FLOAT, false, 8 * 4, (void *)(0 * 4));
				glVertexAttribPointer(_shader._intensity, 4, GL_FLOAT, false, 8 * 4, (void *)(4 * 4));
				//gLog.Log(_T( "pass 1"));
				//CString info;
				//info.Format(_T("Data length--> %d, pointCount --> %d"),vertexBufferViews[pointCount] * 4,pointCount);
				//gLog.Log(info);
				glDrawArrays(GL_TRIANGLES, 0, pointCount * 6);
				//gLog.Log(_T("pass 2"));
			}
			_shader.end();
			pointCount = 0;
			bufferIndex = 0;
			nodeFront.unbindFBO();
			glDisable(GL_BLEND);

		}
	}
	// pointCount
	void addPoint(int x,int y,int size,double intensity)
	{
		int s;
		if (size == 0) {
			size = 50;
		}
		if (intensity == 0) {
			intensity = 0.2;
		}
		if (pointCount >= maxPointCount - 1) {
			update();
		}
		y = _height - y;
		s = size / 2;
		addVertex(x, y, -s, -s, intensity);
		addVertex(x, y, +s, -s, intensity);
		addVertex(x, y, -s, +s, intensity);
		addVertex(x, y, -s, +s, intensity);
		addVertex(x, y, +s, -s, intensity);
		addVertex(x, y, +s, +s, intensity);
		pointCount += 1;
	}

	//清空热力图
	void clear()
	{
		nodeFront.bindFBO();
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);
		nodeFront.unbindFBO();
		return;
	}

private:
	//bufferIndex , 是放在着色器里面 计算顶点的坐标
	void addVertex(int x, int y, int xs, int ys, double intensity)
	{
		vertexBufferData[bufferIndex++] = x;
		vertexBufferData[bufferIndex++] = y;
		vertexBufferData[bufferIndex++] = xs;
		vertexBufferData[bufferIndex++] = ys;
		vertexBufferData[bufferIndex++] = intensity;
		vertexBufferData[bufferIndex++] = intensity;
		vertexBufferData[bufferIndex++] = intensity;
		vertexBufferData[bufferIndex++] = intensity;
	}
public:
	PROGRAM_Heights _shader;
	int _width;
	int _height;
	GLuint vertexBuffer;  //VBO ID
	int vertexSize;
	int maxPointCount;
	GLfloat *vertexBufferData;
	int bufferIndex;
	int pointCount;
	std::vector<int> vertexBufferViews; //
	Node nodeBack;
	Node nodeFront;
};

