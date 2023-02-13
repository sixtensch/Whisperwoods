#include "Core.h"
#include "GameObject.h"
#include "Light.h"



void DirectionalLight::Update(float delta_time)
{
	camera.CopyTransform(transform);
	camera.SetValues(0, 0, LIGHT_NEAR, LIGHT_FAR);
	camera.CalculateOrthoProjection(diameter, diameter);
	camera.Update();

	bufferData.intensity = (Vec3)color * intensity;
	bufferData.clipMatrix = camera.GetProjectionMatrix().Transpose() * camera.GetViewMatrix();
	bufferData.direction = transform.GetWorldRotation() * Vec3(0, 0, 1.0f);
}

void PointLight::Update(float delta_time)
{
	bufferData.intensity = (Vec3)color * intensity;
	bufferData.position = transform.GetWorldPosition();
	bufferData.range = range;
}

void SpotLight::Update(float delta_time)
{
	camera.CopyTransform(transform);
	camera.SetValues(fovOuter, 1, LIGHT_NEAR, range);
	camera.CalculatePerspectiveProjection();
	camera.Update();

	bufferData.intensity = (Vec3)color * intensity;
	bufferData.clipMatrix = camera.GetProjectionMatrix().Transpose() * camera.GetViewMatrix();
	bufferData.direction = transform.GetWorldRotation() * Vec3(0, 0, 1.0f);
	bufferData.range = range;
	bufferData.position = transform.GetWorldPosition();
	bufferData.cosInner = std::cosf(fovInner);
	bufferData.cosOuter = std::cosf(fovOuter);
}