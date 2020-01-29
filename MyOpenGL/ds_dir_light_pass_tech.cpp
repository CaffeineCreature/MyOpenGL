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

#include "ds_dir_light_pass_tech.h"
#include "ogldev_util.h"

DSDirLightPassTech::DSDirLightPassTech()
{
}

bool DSDirLightPassTech::Init()
{
	if (!Technique::Init()) {
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "light_pass.vs")) {
		return false;
	}


	if (!AddShader(GL_FRAGMENT_SHADER, "dir_light_pass.fs")) {
		return false;
	}

	if (!Finalize()) {
		return false;
	}

	m_dirLightLocation.Color			= GetUniformLocation("gDirectionalLight.Base.Color");
	m_dirLightLocation.AmbientIntensity = GetUniformLocation("gDirectionalLight.Base.AmbientIntensity");
	m_dirLightLocation.DiffuseIntensity = GetUniformLocation("gDirectionalLight.Base.DiffuseIntensity");
	m_dirLightLocation.Direction		= GetUniformLocation("gDirectionalLight.Direction");

	if (m_dirLightLocation.Color			== GL_INVALID_INDEX ||
		m_dirLightLocation.AmbientIntensity == GL_INVALID_INDEX ||
		m_dirLightLocation.DiffuseIntensity == GL_INVALID_INDEX ||
		m_dirLightLocation.Direction		== GL_INVALID_INDEX)
		return false;

	return DSLightPassTech::Init();
}

//uniform DirectionalLight gDirectionalLight;
void DSDirLightPassTech::SetDirectionalLight(const DirectionalLight& Light)
{
	glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
	glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
	Vector3f Direction = Light.Direction;
	Direction.Normalize();
	glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
}