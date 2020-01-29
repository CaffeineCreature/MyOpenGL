/*
Copyright 2011 Etay Meiri

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GBUFFER_H
#define	GBUFFER_H

#include <GL/glew.h>

class GBuffer
{
public:

	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_SPECULAR,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_NUM_TEXTURES,
		GBUFFER_TEXTURE_TYPE_FINAL = GBUFFER_NUM_TEXTURES,
		GBUFFER_TEXTURE_TYPE_ALT
	};
	
	enum GBUFFER_COLOR_ATTATCHMENT {
		GBUFFER_POSITION_ATTATCHMENT = GL_COLOR_ATTACHMENT0,
		GBUFFER_DIFFUSE_ATTATCHMENT,
		GBUFFER_SPECULAR_ATTATCHMENT,
		GBUFFER_NORMAL_ATTATCHMENT,
		GBUFFER_FINAL_ATTATCHMENT,
		GBUFFER_ALT_ATTATCHMENT
	};

	GBuffer();

	~GBuffer();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight, GLuint samplesPerPixel);

	void StartFrame();
	void BindForGeomPass();
	void BindForStencilPass();
	void BindForLightPass();
	void BindForPostProcess();
	void BindForFinalPass();
	void BindSwapActive();

	GLuint GetActiveTextureUnit();

private:
	// active read texture attatchment
	GLuint m_pingPongFlag;
	GLuint m_fbo;
	GLuint m_textures[GBUFFER_NUM_TEXTURES];
	GLuint m_depthTexture;
	GLuint m_finalTexture;
	GLuint m_altTexture;
	GLuint m_sampleCount;
};

#endif GBUFFER_H