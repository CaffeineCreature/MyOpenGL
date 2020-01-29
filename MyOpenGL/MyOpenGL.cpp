/*

Copyright 2011 Etay Meiriq

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See theqq
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>
#include <GL/glew.h>
#include <string>
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif
#include <sys/types.h>

#include "ogldev_app.h"
#include "ogldev_util.h"
#include "ogldev_pipeline.h"
#include "ogldev_camera.h"
#include "ssao_tech.h"
#include "geom_pass_tech.h"
#include "blur_tech.h"
#include "lighting_technique.h"
#include "ogldev_backend.h"
#include "ogldev_camera.h"
#include "mesh.h"
#include "ogldev_io_buffer.h"

#pragma region MeshLocations
constexpr char* MESH_BOX	 = "../Content/box.obj";
constexpr char* MESH_BUDDHA	 = "../Content/buddha.obj";
constexpr char* MESH_BUNNY	 = "../Content/bunny.obj";
constexpr char* MESH_DRAGON	 = "../Content/dragon.obj";
constexpr char* MESH_HHELI	 = "../Content/hheli.obj";
constexpr char* MESH_JEEP	 = "../Content/jeep.obj";
constexpr char* MESH_MONKEY	 = "../Content/monkey.obj";
constexpr char* MESH_PHOENIX = "../Content/phoenix_ugv.md2";
constexpr char* MESH_SPHERE	 = "../Content/sphere.obj";
constexpr char* MESH_QUAD	 = "../Content/quad.obj";
constexpr char* MESH_SPONZA  = "../Content/crytek_sponza/crytek_sponza.obj";

constexpr char* MESH_SHROOM_01 = "C:/Dev/Assets/ArtByRens/Mushrooms/Mushroom 01.obj";

constexpr char* SKINNED_MESH_GUARD = "../Content/boblampclean.md5mesh";

constexpr char* MESH_FACE			= "C:/Dev/Assets/MIT/head-poses/head-reference.obj";
constexpr char* MESH_FACE_ANGER		= "C:/Dev/Assets/MIT/head-poses/head-01-anger.obj";
constexpr char* MESH_FACE_CRY		= "C:/Dev/Assets/MIT/head-poses/head-02-cry.obj";
constexpr char* MESH_FACE_FURY		= "C:/Dev/Assets/MIT/head-poses/head-03-fury.obj";
constexpr char* MESH_FACE_GRIN		= "C:/Dev/Assets/MIT/head-poses/head-04-grin.obj";
constexpr char* MESH_FACE_LAUGH		= "C:/Dev/Assets/MIT/head-poses/head-05-laugh.obj";
constexpr char* MESH_FACE_RAGE		= "C:/Dev/Assets/MIT/head-poses/head-06-rage.obj";
constexpr char* MESH_FACE_SAD		= "C:/Dev/Assets/MIT/head-poses/head-07-sad.obj";
constexpr char* MESH_FACE_SMILE		= "C:/Dev/Assets/MIT/head-poses/head-08-smile.obj";
constexpr char* MESH_FACE_SUPRISE	= "C:/Dev/Assets/MIT/head-poses/head-09-surprise.obj";
#pragma endregion

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

class MyOpenGL : public ICallbacks, public OgldevApp
{
public:

	MyOpenGL()
	{
		m_pGameCamera = NULL;

		m_persProjInfo.FOV = 60.0f;
		m_persProjInfo.Height = WINDOW_HEIGHT;
		m_persProjInfo.Width = WINDOW_WIDTH;
		m_persProjInfo.zNear = 0.01f;
		m_persProjInfo.zFar = 1000.0f;

		m_pipeline.SetPerspectiveProj(m_persProjInfo);

		m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
		m_directionalLight.AmbientIntensity = 0.3f;
		m_directionalLight.DiffuseIntensity = 1.0f;
		m_directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

		m_shaderType = 0;
	}

	~MyOpenGL()
	{
		SAFE_DELETE(m_pGameCamera);
	}

	bool Init()
	{
		Vector3f Pos(0.0f, 24.0f, -38.0f);
		Vector3f Target(0.0f, -0.5f, 1.0f);
		Vector3f Up(0.0, 1.0f, 0.0f);

		m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

		if (!m_geomPassTech.Init()) {
			OGLDEV_ERROR0("Error initializing the geometry pass technique\n");
			return false;
		}

		if (!m_SSAOTech.Init()) {
			OGLDEV_ERROR0("Error initializing the SSAO technique\n");
			return false;
		}

		m_SSAOTech.Enable();
		m_SSAOTech.SetSampleRadius(1.5f);
		Matrix4f PersProjTrans;
		PersProjTrans.InitPersProjTransform(m_persProjInfo);
		m_SSAOTech.SetProjMatrix(PersProjTrans);

		if (!m_lightingTech.Init()) {
			OGLDEV_ERROR0("Error initializing the lighting technique\n");
			return false;
		}

		m_lightingTech.Enable();
		m_lightingTech.SetDirectionalLight(m_directionalLight);
		m_lightingTech.SetScreenSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		m_lightingTech.SetShaderType(0);

		if (!m_blurTech.Init()) {
			OGLDEV_ERROR0("Error initializing the blur technique\n");
			return false;
		}

		if (!m_mesh.LoadMesh(MESH_DRAGON)) {
			return false;
		}

		m_mesh.GetOrientation().m_scale = Vector3f(1.f);
		m_mesh.GetOrientation().m_pos = Vector3f(0.0f, 0.0f, 0.0f);
		m_mesh.GetOrientation().m_rotation = Vector3f(0.0f, 180.0f, 0.0f);

		if (!m_quad.LoadMesh("../Content/quad.obj")) {
			return false;
		}
		
		if (!m_gBuffer.Init(WINDOW_WIDTH, WINDOW_HEIGHT, true, GL_RGB32F)) {
			return false;
		}
		
		if (!m_aoBuffer.Init(WINDOW_WIDTH, WINDOW_HEIGHT, false, GL_R32F)) {
			return false;
		}

		if (!m_blurBuffer.Init(WINDOW_WIDTH, WINDOW_HEIGHT, false, GL_R32F)) {
			return false;
		}

		return true;
	}

	void Run()
	{
		OgldevBackendRun(this);

	}

	virtual void RenderSceneCB()
	{
		m_pGameCamera->OnRender();

		m_pipeline.SetCamera(*m_pGameCamera);

		GeometryPass();

		SSAOPass();

		BlurPass();

		LightingPass();

		RenderFPS();

		CalcFPS();

		OgldevBackendSwapBuffers();
	}

	void GeometryPass()
	{
		m_geomPassTech.Enable();

		m_gBuffer.BindForWriting();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_pipeline.Orient(m_mesh.GetOrientation());
		m_geomPassTech.SetWVP(m_pipeline.GetWVPTrans());
		m_geomPassTech.SetWVMatrix(m_pipeline.GetWVTrans());
		m_mesh.Render();
	}

	void SSAOPass()
	{
		m_SSAOTech.Enable();
		m_SSAOTech.BindPositionBuffer(m_gBuffer);

		m_aoBuffer.BindForWriting();

		glClear(GL_COLOR_BUFFER_BIT);

		m_quad.Render();
	}

	void BlurPass()
	{
		m_blurTech.Enable();

		m_blurTech.BindInputBuffer(m_aoBuffer);

		m_blurBuffer.BindForWriting();

		glClear(GL_COLOR_BUFFER_BIT);

		m_quad.Render();
	}

	void LightingPass()
	{
		m_lightingTech.Enable();
		m_lightingTech.SetShaderType(m_shaderType);
		m_lightingTech.BindAOBuffer(m_blurBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_pipeline.Orient(m_mesh.GetOrientation());
		m_lightingTech.SetWVP(m_pipeline.GetWVPTrans());
		m_lightingTech.SetWorldMatrix(m_pipeline.GetWorldTrans());
		m_mesh.Render();
	}

	virtual void KeyboardCB(OGLDEV_KEY OgldevKey, OGLDEV_KEY_STATE State)
	{
		switch (OgldevKey) {
		case OGLDEV_KEY_ESCAPE:
		case OGLDEV_KEY_q:
			OgldevBackendLeaveMainLoop();
			break;
		case OGLDEV_KEY_A:
			m_shaderType++;
			m_shaderType = m_shaderType % 3;
			break;
		default:
			m_pGameCamera->OnKeyboard(OgldevKey);
		}
	}


	virtual void PassiveMouseCB(int x, int y)
	{
		m_pGameCamera->OnMouse(x, y);
	}

private:

	SSAOTechnique m_SSAOTech;
	GeomPassTech m_geomPassTech;
	LightingTechnique m_lightingTech;
	BlurTech m_blurTech;
	Camera* m_pGameCamera;
	Mesh m_mesh;
	Mesh m_quad;
	PersProjInfo m_persProjInfo;
	Pipeline m_pipeline;
	IOBuffer m_gBuffer;
	IOBuffer m_aoBuffer;
	IOBuffer m_blurBuffer;
	DirectionalLight m_directionalLight;
	int m_shaderType;
};

int main(int argc, char** argv)
{
	OgldevBackendInit(OGLDEV_BACKEND_TYPE_GLFW, argc, argv, true, false);

	if (!OgldevBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "MyOpenGL")) {
		OgldevBackendTerminate();
		return 1;
	}

	SRANDOM;

	MyOpenGL* pApp = new MyOpenGL();

	if (!pApp->Init()) {
		delete pApp;
		OgldevBackendTerminate();
		return 1;
	}

	pApp->Run();

	OgldevBackendTerminate();

	return 0;
}