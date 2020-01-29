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

#include "ds_geom_pass_tech.h"
#include "ogldev_util.h"


DSGeomPassTech::DSGeomPassTech()
{
}

bool DSGeomPassTech::Init()
{
	if (!Technique::Init()) {
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "geometry_pass.vs")) {
		return false;
	}


	if (!AddShader(GL_FRAGMENT_SHADER, "geometry_pass.fs")) {
		return false;
	}

	if (!Finalize()) {
		return false;
	}

	m_WVPLocation = GetUniformLocation("gWVP");
	m_WorldMatrixLocation = GetUniformLocation("gWorld");
	m_colorTextureUnitLocation = GetUniformLocation("gColorMap");
	m_colorTextureUnitLocation = GetUniformLocation("gNormalMap");

	if (m_WVPLocation == INVALID_UNIFORM_LOCATION ||
		m_WorldMatrixLocation == INVALID_UNIFORM_LOCATION ||
		m_colorTextureUnitLocation == INVALID_UNIFORM_LOCATION ||
		m_normalTextureUnitLocation == INVALID_UNIFORM_LOCATION) {
		return false;
	}

	return true;
}

// * uniform mat4 gWVP
void DSGeomPassTech::SetWVP(const Matrix4f& WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

// * uniform mat4 gWorld
void DSGeomPassTech::SetWorldMatrix(const Matrix4f& WorldInverse)
{
	glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_TRUE, (const GLfloat*)WorldInverse.m);
}

// * uniform sampler2d gColorMap
void DSGeomPassTech::SetColorTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_colorTextureUnitLocation, TextureUnit);
}

// * uniform sampler2d gNormalMap
void DSGeomPassTech::SetNormalTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_normalTextureUnitLocation, TextureUnit);
}