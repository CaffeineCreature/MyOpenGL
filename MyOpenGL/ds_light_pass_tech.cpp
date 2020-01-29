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

#include <limits.h>
#include <string.h>

#include "ds_light_pass_tech.h"
#include "ogldev_util.h"

DSLightPassTech::DSLightPassTech()
{
}

bool DSLightPassTech::Init()
{
	m_WVPLocation					= GetUniformLocation("gWVP");
	m_posTextureUnitLocation		= GetUniformLocation("gPositionMap");
	m_colorTextureUnitLocation		= GetUniformLocation("gColorMap");
	m_specularTextureUnitLocation	= GetUniformLocation("gSpecularMap");
	m_normalTextureUnitLocation		= GetUniformLocation("gNormalMap");
	m_eyeWorldPosLocation			= GetUniformLocation("gEyeWorldPos");
	//m_matSpecularIntensityLocation	= GetUniformLocation("gMatSpecularIntensity");
	m_matSpecularPowerLocation		= GetUniformLocation("gSpecularPower");
	m_screenSizeLocation			= GetUniformLocation("gScreenSize");

	if (m_WVPLocation					== GL_INVALID_INDEX ||
		m_posTextureUnitLocation		== GL_INVALID_INDEX ||
		m_colorTextureUnitLocation		== GL_INVALID_INDEX ||
		m_specularTextureUnitLocation	== GL_INVALID_INDEX ||
		m_normalTextureUnitLocation		== GL_INVALID_INDEX ||
		m_eyeWorldPosLocation			== GL_INVALID_INDEX ||
		//m_matSpecularIntensityLocation	== GL_INVALID_INDEX ||
		m_matSpecularPowerLocation		== GL_INVALID_INDEX ||
		m_screenSizeLocation			== GL_INVALID_INDEX) {
		DebugPrint("Failed to intialize light pass technique uniform variables\n");
		return false;
	}

	return true;
}

// * uniform mat4 gWVP
void DSLightPassTech::SetWVP(const Matrix4f& WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

// * uniform sampler2D gPositionMap
void DSLightPassTech::SetPositionTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_posTextureUnitLocation, TextureUnit);
}

// * uniform sampler2D gColorMap
void DSLightPassTech::SetColorTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_colorTextureUnitLocation, TextureUnit);
}

// * uniform sampler2D gSpecularMap;
void DSLightPassTech::SetSpecularTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_specularTextureUnitLocation, TextureUnit);
}

// * uniform sampler2D gNormalMap
void DSLightPassTech::SetNormalTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_normalTextureUnitLocation, TextureUnit);
}

// * uniform vec3 gEyeWorldPos
void DSLightPassTech::SetEyeWorldPos(const Vector3f& EyeWorldPos)
{
	glUniform3fv(m_eyeWorldPosLocation, 1, EyeWorldPos);
}

// * uniform float gMatSpecularIntensity
void DSLightPassTech::SetMatSpecularIntensity(float Intensity)
{
	glUniform1f(m_matSpecularIntensityLocation, Intensity);
}

// * uniform float gSpecularPower
void DSLightPassTech::SetMatSpecularPower(float Power)
{
	glUniform1f(m_matSpecularPowerLocation, Power);
}

// * uniform vec2 gScreenSize
void DSLightPassTech::SetScreenSize(unsigned int Width, unsigned int Height)
{
	glUniform2f(m_screenSizeLocation,(GLfloat) Width, (GLfloat) Height);
}