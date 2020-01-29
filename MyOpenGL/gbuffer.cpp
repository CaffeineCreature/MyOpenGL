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

#include <stdio.h>

#include "ogldev_util.h"
#include "gbuffer.h"
#include "ogldev_texture.h"

GBuffer::GBuffer()
{
	m_fbo = 0;
	m_depthTexture = 0;
	m_finalTexture = 0;
	ZERO_MEM(m_textures);
}

GBuffer::~GBuffer()
{
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
	}

	if (m_textures[0] != 0) {
		glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
	}

	if (m_depthTexture != 0) {
		glDeleteTextures(1, &m_depthTexture);
	}

	if (m_finalTexture != 0) {
		glDeleteTextures(1, &m_finalTexture);
	}

	if (m_altTexture != 0) {
		glDeleteTextures(1, &m_altTexture);
	}
}

bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight, GLuint samplesPerPixel)
{
	// Create the FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	// Create the gbuffer textures
	glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);

	glGenTextures(1, &m_depthTexture);

	glGenTextures(1, &m_finalTexture);

	glGenTextures(1, &m_altTexture);

	m_sampleCount = samplesPerPixel;

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
	}

	// depth
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, WindowWidth, WindowHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

	// final
	glBindTexture(GL_TEXTURE_2D, m_finalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GBUFFER_FINAL_ATTATCHMENT, GL_TEXTURE_2D, m_finalTexture, 0);

	// alt
	glBindTexture(GL_TEXTURE_2D, m_altTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GBUFFER_ALT_ATTATCHMENT, GL_TEXTURE_2D, m_altTexture, 0);

	// Final check
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		DebugPrint("FB error, status: 0x%x\n", Status);
		return false;
	}
	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	GLCheckErrorDebug();

	return true;
}

void GBuffer::StartFrame()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glDrawBuffer(GBUFFER_FINAL_ATTATCHMENT);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pingPongFlag = GBUFFER_FINAL_ATTATCHMENT;
}

void GBuffer::BindForGeomPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	GLenum DrawBuffers[] = {	GBUFFER_POSITION_ATTATCHMENT,
								GBUFFER_DIFFUSE_ATTATCHMENT,
								GBUFFER_SPECULAR_ATTATCHMENT,
								GBUFFER_NORMAL_ATTATCHMENT};


	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
}

void GBuffer::BindForStencilPass()
{
	// must disable the draw buffers 
	glDrawBuffer(GL_NONE);
}

void GBuffer::BindForLightPass()
{
	glDrawBuffer(GBUFFER_FINAL_ATTATCHMENT);

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
	}
}

// Call before post proccess stage
void GBuffer::BindForPostProcess()
{
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glDrawBuffer(GBUFFER_ALT_ATTATCHMENT);
	glClear(GL_COLOR_BUFFER_BIT);

	for (uint32_t i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
	}

	glActiveTexture(GL_TEXTURE0 + GBUFFER_TEXTURE_TYPE_FINAL);
	glBindTexture(GL_TEXTURE_2D, m_finalTexture);

	m_pingPongFlag = GBUFFER_FINAL_ATTATCHMENT;
}

// Call between post proccess draw-calls
void GBuffer::BindSwapActive()
{
	glDrawBuffer(m_pingPongFlag);
	glClear(GL_COLOR_BUFFER_BIT);

	// rebind main buffers
	for (uint32_t i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
	}

	// swap ping pong binding
	m_pingPongFlag = m_pingPongFlag == GBUFFER_FINAL_ATTATCHMENT ? GBUFFER_ALT_ATTATCHMENT : GBUFFER_FINAL_ATTATCHMENT;

	// bind last rendered texture to be read
	glActiveTexture(GL_TEXTURE0 + GetActiveTextureUnit());
	glBindTexture(GL_TEXTURE_2D, m_pingPongFlag == GBUFFER_FINAL_ATTATCHMENT ? m_finalTexture : m_altTexture);
}

// Return the texture unit for the color buffer last writen
GLuint GBuffer::GetActiveTextureUnit()
{
	return m_pingPongFlag == GBUFFER_FINAL_ATTATCHMENT ? GBUFFER_TEXTURE_TYPE_FINAL : GBUFFER_TEXTURE_TYPE_ALT;
}

void GBuffer::BindForFinalPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glReadBuffer(m_pingPongFlag == GBUFFER_FINAL_ATTATCHMENT ? GBUFFER_ALT_ATTATCHMENT : GBUFFER_FINAL_ATTATCHMENT);
}
