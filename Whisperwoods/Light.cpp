#include "Core.h"
#include "GameObject.h"
#include "Light.h"



void DirectionalLight::Update(float delta_time)
{
	transform.CalculateWorldMatrix();
	camera.SetPosition( transform.GetWorldPosition() );
	camera.SetRotation( transform.GetWorldRotation() );
	//camera.CopyTransform(transform);
	//camera.m_transform.parent = &transform;

	camera.SetValues(dx::XM_PI, 1, LIGHT_NEAR, 3000);
	camera.CalculateOrthoProjection(diameter, diameter);
	camera.Update();

	bufferData.intensity = (Vec3)color * intensity;
	bufferData.clipMatrix = camera.GetProjectionMatrix().Transpose() * camera.GetViewMatrix();
	//Vec4 temp = transform.worldMatrix/*.Conjugate()*/ * Vec4(0.0f, 1.0f, 0.0f, 0.0f);
	bufferData.direction = transform.worldMatrix * Vec4(0.0f, 0.0f, 1.0f, 0.0f);
	bufferData.direction = bufferData.direction.Normalized();
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