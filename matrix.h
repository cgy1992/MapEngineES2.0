#ifndef EXAMPLES_HELLO_WORLD_GLES_MATRIX_H
#define EXAMPLES_HELLO_WORLD_GLES_MATRIX_H

/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

/** @file matrix.cc
* Implements simple matrix manipulation functions.
*/

//-----------------------------------------------------------------------------
#define _USE_MATH_DEFINES 1
#include <limits.h>
#include <math.h>
#include <GLES2/gl2.h>

typedef GLfloat Matrix_t[16];

/// Since GLES2 doesn't have all the nifty matrix transform functions that GL
/// has, we emulate some of them here for the sake of sanity from:
/// http://www.opengl.org/wiki/GluPerspective_code
void glhFrustumf2(Matrix_t mat, GLfloat left, GLfloat right, GLfloat bottom,
	GLfloat top, GLfloat znear, GLfloat zfar);

void glhPerspectivef2(Matrix_t mat, GLfloat fovyInDegrees,
	GLfloat aspectRatio, GLfloat znear, GLfloat zfar);


void identity_matrix(Matrix_t mat);
void multiply_matrix(const Matrix_t a, const Matrix_t b, Matrix_t mat);
void rotate_matrix(GLfloat x_deg, GLfloat y_deg, GLfloat z_deg, Matrix_t mat);
void translate_matrix(GLfloat x, GLfloat y, GLfloat z, Matrix_t mat);
//add by wangyuejiao
void translateMatrix(GLfloat x, GLfloat y, GLfloat z, Matrix_t mat);

void glhOrtho2(Matrix_t mat, GLfloat left, GLfloat right, GLfloat bottom,GLfloat top, GLfloat znear, GLfloat zfar);
void scale_matrix(GLfloat x,GLfloat y,GLfloat z,Matrix_t mat); 

struct Vector3
{
	float x;
	float y;
	float z;
};

void glmNormalize(Vector3& v)
{
	GLfloat l;
	l = (GLfloat)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x /= l;
	v.y /= l;
	v.z /= l;
}

void glmCross(Vector3& u, Vector3& v, Vector3& n)
{
	n.x = u.y * v.z - u.z * v.y;
	n.y = u.z * v.x - u.x * v.y;
	n.z = u.x * v.y - u.y * v.x;
}
void GetLookAtMatrix(const Vector3& eye, const Vector3& at, Vector3& up, Matrix_t mat)
{
	Vector3 forward, side;       
	forward.x = at.x - eye.x;
	forward.y = at.y - eye.y;
	forward.z = at.z - eye.z;
	glmNormalize(forward);
	glmCross(forward, up, side);
	glmNormalize(side);
	//glmCross(side, forward, up);

	mat[0] = side.x;
	mat[1] = up.x;
	mat[2] = -forward.x;
	mat[3] = 0;

	mat[4] = side.y;
	mat[5] = up.y;
	mat[6] = -forward.y;
	mat[7] = 0;

	mat[8] = side.z;
	mat[9] = up.z;
	mat[10] = -forward.z;
	mat[11] = 0;

	mat[12] = 0;
	mat[13] = 0;
	mat[14] = 0;
	mat[15] = 1;

	translate_matrix(-eye.x, -eye.y, -eye.z, mat);
}

#endif  // EXAMPLES_HELLO_WORLD_GLES_MATRIX_H

