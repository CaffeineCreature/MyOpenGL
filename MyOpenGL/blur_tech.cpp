#include <limits.h>
#include <string.h>

#include "blur_tech.h"
#include "ogldev_util.h"

#define INPUT_TEXTURE_UNIT                 GL_TEXTURE0
#define INPUT_TEXTURE_UNIT_INDEX           0

BlurTech::BlurTech()
{
}

bool BlurTech::Init()
{
	if (!Technique::Init()) {
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "blur.vs")) {
		return false;
	}


	if (!AddShader(GL_FRAGMENT_SHADER, "blur.fs")) {
		return false;
	}

	if (!Finalize()) {
		return false;
	}

	m_inputTextureUnitLocation = GetUniformLocation("gColorMap");

	if (m_inputTextureUnitLocation == INVALID_UNIFORM_LOCATION) {
		return false;
	}

	Enable();

	glUniform1i(m_inputTextureUnitLocation, INPUT_TEXTURE_UNIT_INDEX);

	return true;
}

void BlurTech::BindInputBuffer(IOBuffer & inputBuf)
{
	inputBuf.BindForReading(INPUT_TEXTURE_UNIT);
}
