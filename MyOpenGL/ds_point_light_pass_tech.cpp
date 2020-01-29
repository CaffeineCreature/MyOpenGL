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

#include "ds_point_light_pass_tech.h"
#include "ogldev_util.h"

DSPointLightPassTech::DSPointLightPassTech()
{
}

bool DSPointLightPassTech::Init()
{
	if (!Technique::Init()) {
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "light_pass.vs")) {
		DebugPrint("Shader failed: light_pass.vs\n");
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "point_light_pass.fs")) {
		DebugPrint("Shader failed: point_light_pass.fs\n");
		return false;
	}

	if (!Finalize()) {
		DebugPrint("Point Light Technique failed to finalize shaders\n");
		return false;
	}

	m_pointLightLocation.Color				= GetUniformLocation("gPointLight.Base.Color");
	m_pointLightLocation.AmbientIntensity	= GetUniformLocation("gPointLight.Base.AmbientIntensity");
	m_pointLightLocation.DiffuseIntensity	= GetUniformLocation("gPointLight.Base.DiffuseIntensity");
	m_pointLightLocation.Position			= GetUniformLocation("gPointLight.Position");
	m_pointLightLocation.Atten.Constant		= GetUniformLocation("gPointLight.Atten.Constant");
	m_pointLightLocation.Atten.Linear		= GetUniformLocation("gPointLight.Atten.Linear");
	m_pointLightLocation.Atten.Exp			= GetUniformLocation("gPointLight.Atten.Exp");

	if (m_pointLightLocation.Color				== INVALID_UNIFORM_LOCATION ||
		m_pointLightLocation.AmbientIntensity	== INVALID_UNIFORM_LOCATION ||
		m_pointLightLocation.DiffuseIntensity	== INVALID_UNIFORM_LOCATION ||
		m_pointLightLocation.Position			== INVALID_UNIFORM_LOCATION ||
		m_pointLightLocation.Atten.Constant		== INVALID_UNIFORM_LOCATION ||
		m_pointLightLocation.Atten.Linear		== INVALID_UNIFORM_LOCATION ||
		m_pointLightLocation.Atten.Exp			== INVALID_UNIFORM_LOCATION)
		return false;
	
	return DSLightPassTech::Init();
}

//uniform PointLight gPointLight
void DSPointLightPassTech::SetPointLight(const PointLight& Lights)
{
	glUniform3f(m_pointLightLocation.Color, Lights.Color.x, Lights.Color.y, Lights.Color.z);
	glUniform1f(m_pointLightLocation.AmbientIntensity, Lights.AmbientIntensity);
	glUniform1f(m_pointLightLocation.DiffuseIntensity, Lights.DiffuseIntensity);
	glUniform3f(m_pointLightLocation.Position, Lights.Position.x, Lights.Position.y, Lights.Position.z);
	glUniform1f(m_pointLightLocation.Atten.Constant, Lights.Attenuation.Constant);
	glUniform1f(m_pointLightLocation.Atten.Linear, Lights.Attenuation.Linear);
	glUniform1f(m_pointLightLocation.Atten.Exp, Lights.Attenuation.Exp);
}