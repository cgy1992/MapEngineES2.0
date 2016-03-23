#pragma once
#include <EGL/egl.h>
#include <gles2/gl2.h>
#include <map>
class Node
{
public:
	Node(int width, int height):
		_width(width)
		, _height(height)
	{
		_textureId = createTexture(width, height);
		 _fbo = createFBO(width, height);
	}
	~Node()
	{
		glDeleteTextures(1, &_textureId);
	}
private:
	 unsigned    createTexture(int width, int height)
	{
		unsigned    textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(
			GL_TEXTURE_2D
			, 0
			, GL_RGBA
			, width
			, height
			, 0
			, GL_RGBA
			, GL_UNSIGNED_BYTE     //type
			, 0);
		return  textureId;
	}
	 std::pair<GLuint, GLuint>	 createFBO(int width, int height)
	 {
		 std::pair<GLuint, GLuint> buf;
		 glGenFramebuffers(1, &buf.first);
		 glBindFramebuffer(GL_FRAMEBUFFER, buf.first);

		 glGenRenderbuffers(1, &buf.second);
		 glBindRenderbuffer(GL_RENDERBUFFER, buf.second);
		 glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
		 glBindRenderbuffer(GL_RENDERBUFFER, 0);

		 glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buf.second);
	     glBindFramebuffer(GL_FRAMEBUFFER, 0);
		 return buf;
	 }
public:
	void bind(unsigned textureLevel)
	{
		glActiveTexture(GL_TEXTURE0 + textureLevel);
		glBindTexture(GL_TEXTURE_2D, _textureId);
	}
	void bindFBO()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo.first);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureId, 0);
	}
	void unbindFBO()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
public:
	int _width;
	int _height;
	GLuint _textureId;
	std::pair<GLuint, GLuint> _fbo;
};

