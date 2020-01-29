#include "motion_blur_technique.h"
#include "ogldev_util.h"

MotionBlurTechnique::MotionBlurTechnique() 
{

}

bool MotionBlurTechnique::Init() 
{
	if (!Technique::Init()) {
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "motion_blur.vs")) {
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "motion_blur.fs")) {
		return false;
	}

	if (!Finalize()) {
		return false;
	}

	m_colorTextureLocation = GetUniformLocation("gColorTexture");
	m_motionTextureLocation = GetUniformLocation("gMotionTexture");

	if (m_motionTextureLocation == INVALID_UNIFORM_LOCATION ||
		m_colorTextureLocation == INVALID_UNIFORM_LOCATION) {
		return false;
	}

	return true;
}

void MotionBlurTechnique::SetColorTextureUnit(GLuint textureUnit) 
{
	glUniform1i(m_colorTextureLocation, textureUnit);
}

void MotionBlurTechnique::SetMotionTextureUnit(GLuint textureUnit) 
{
	glUniform1i(m_motionTextureLocation, textureUnit);
}