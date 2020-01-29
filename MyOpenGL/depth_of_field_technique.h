#ifndef DEPTH_OF_FIELD_TECHNIQUE_H
#define DEPTH_OF_FIELD_TECHNIQUE_H

#include "technique.h"
#include "ogldev_math_3d.h"

struct CameraOptics{
	float fov;
	float fStops;
	float min_fStops;
	float max_fStops;
	float focalLength;
	float focalDistance;
	float sensorWidth;
	float aspectRatio;
	float apertureRotation;
	float apertureDiameter;
	uint32_t apertureBlades;
	
	CameraOptics()
	{
		// Default camera setup
		min_fStops = 2.8f;
		max_fStops = 22.f;
		fStops = 4.f;

		// 35mm film, 36 x 24mm
		sensorWidth = 0.036f;
		aspectRatio = 1024.f / 1280.f;

		// focus at 1 meter
		focalDistance = 1.f;
		fov = ToRadian(60.f);
		focalLength = 0.5f * sensorWidth / tan(fov / 2.f);

		// heptagonal apature - default postion
		apertureRotation = 0.f;
		apertureBlades = 7;
		apertureDiameter = focalLength / fStops;
	}

	CameraOptics(const PersProjInfo& PersP)
	{
		// Default camera setup
		min_fStops = 2.8f;
		max_fStops = 22.f;
		fStops = 4.f;

		// 35mm film, 36 x 24mm
		sensorWidth = 0.036f;
		aspectRatio = PersP.Height / PersP.Width;

		// focus at 1 meter
		focalDistance = 1.f;
		fov = ToRadian(PersP.FOV);
		focalLength = 0.5f * sensorWidth / tan(fov / 2);

		// heptagonal apature - default postion
		apertureRotation = 0.f;
		apertureBlades = 7;
		apertureDiameter = focalLength / fStops;
	}

	CameraOptics(const PersProjInfo& PersP, float _focalDistance)
	{
		// Default camera setup
		min_fStops = 2.8f;
		max_fStops = 22.f;
		fStops = 4.f;

		// 35mm film, 36 x 24mm
		sensorWidth = 0.036f;
		aspectRatio = PersP.Height / PersP.Width;

		// focus at 1 meter
		focalDistance = _focalDistance;
		fov = ToRadian(PersP.FOV);
		focalLength = 0.5f * sensorWidth / tan(fov / 2);

		// heptagonal apature - default postion
		apertureRotation = 0.f;
		apertureBlades = 7;
		apertureDiameter = focalLength / fStops;
	}
};

class DofTechnique : public Technique
{
	DofTechnique();

	virtual bool Init();

	void SetColorTextureUnit(GLuint TextureUnit);

	void SetCameraOptics(const CameraOptics&);

	void SetDoFTaps(GLuint Taps);

private:
	Vector2f sampleGrid;
	GLuint m_colorTextureLocation;
	GLuint m_sampleGridLocation;

};

#endif // !DEPTH_OF_FIELD_TECHNIQUE_H