// Caffeine Creature
// Christopher Pearson
// 2018

#include "ds_post_process_pass_tech.h"
#include "ogldev_util.h"

#include <limits.h>
#include <string.h>

DSPostProcessPassTech::DSPostProcessPassTech()
{
}

bool DSPostProcessPassTech::Init()
{
	if (!Technique::Init())
		return false;
	
	if (!AddShader(GL_VERTEX_SHADER, "post_process.vs")) 
		return false;
	
	if (!AddShader(GL_FRAGMENT_SHADER, "post_process.fs")) 
		return false;

	if (!Finalize())
		return false;

	m_FramebufferTextureUnitLocation	= GetUniformLocation("gFrameBuffer");
	m_ScreenSizeLocation				= GetUniformLocation("gScreenSize");
	m_WVPLocation						= GetUniformLocation("gWVP");

	if (m_FramebufferTextureUnitLocation	== INVALID_UNIFORM_LOCATION ||
		m_ScreenSizeLocation				== INVALID_UNIFORM_LOCATION ||
		m_WVPLocation						== INVALID_UNIFORM_LOCATION)
		return false;

	return true;
}

void DSPostProcessPassTech::SetFinalTextureUnit(GLuint textureUnit)
{
	glUniform1i(m_FramebufferTextureUnitLocation, textureUnit);
}

void DSPostProcessPassTech::SetScreenSize(uint32_t width, uint32_t height)
{
	glUniform2f(m_ScreenSizeLocation, width, height);
}

void DSPostProcessPassTech::SetWVP(const Matrix4f& WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (GLfloat*)WVP.m);
}