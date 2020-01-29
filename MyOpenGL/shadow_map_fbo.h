#ifndef SHADOW_MAP_FBO_H
#define SHADOW_MAP_FBO_H

#include <GL/glew.h>

class ShadowMapFBO
{
public:
	ShadowMapFBO();

	~ShadowMapFBO();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

	void BindForWriting();

	void BindForReading(GLenum TextureUnit);

private:
	GLuint m_fbo;
	GLuint m_shadowMap;
};

#endif // !SHADOW_MAP_FBO_H