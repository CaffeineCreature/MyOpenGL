#ifndef INTERMEDIATE_BUFFER_H
#define INTERMEDIATE_BUFFER_H

#include <GL/glew.h>

class IntermediateBuffer
{
public:

	IntermediateBuffer();

	~IntermediateBuffer();

	bool Init(uint32_t WindowWidth, uint32_t WindowHeight);

	void BindForWriting();

	void BindForReading();

private:

	GLuint m_fbo;
	GLuint m_colorBuffer;
	GLuint m_motionBuffer;
	GLuint m_depthBuffer;
};
#endif // !INTERMEDIATE_BUFFER_H