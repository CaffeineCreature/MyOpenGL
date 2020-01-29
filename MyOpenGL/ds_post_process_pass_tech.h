// Caffeine Creature
// Christopher Pearson
// 2018

#ifndef DS_POST_PROCESS_PASS_TECH_H
#define DS_POST_PROCESS_PASS_TECH_H

#include "technique.h"
#include "ogldev_math_3d.h"

class DSPostProcessPassTech : public Technique {
public:
	DSPostProcessPassTech();

	virtual bool Init();

	void SetFinalTextureUnit(GLuint textureUnit);
	void SetScreenSize(uint32_t width, uint32_t height);
	void SetWVP(const Matrix4f& WVP);

private:
	GLuint m_FramebufferTextureUnitLocation;
	GLuint m_ScreenSizeLocation;
	GLuint m_WVPLocation;
};

#endif // DS_POST_PROCESS_PASS_TECH_H