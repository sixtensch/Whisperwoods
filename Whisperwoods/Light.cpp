#include "Core.h"
#include "GameObject.h"
#include "Light.h"


void Light::FillBasicBufferData(LightBufferData* outData, Transform* inTransform)
{
	outData->position = inTransform->GetWorldPosition();
	Quaternion worldRot = inTransform->GetWorldRotation();
	Quaternion direction = worldRot * Quaternion(0, 0, 1, 0) * worldRot.Inverse(); // Quaternion rotation magic.
	outData->direction = Vec3(direction.x, direction.y, direction.z);
	outData->color = color;
}


void PointLight::Update()
{
	FillBasicBufferData(&bufferData, &transform);
	bufferData.intensity.x = intensity;
}

void SpotLight::Update()
{
	FillBasicBufferData(&bufferData, &transform);
	bufferData.intensityInnerOuter.x = intensity;
	bufferData.intensityInnerOuter.y = fovInner;
	bufferData.intensityInnerOuter.z = fovOuter;
}

void DirectionalLight::Update()
{
	FillBasicBufferData(&bufferData, &transform);
	bufferData.intensity.x = intensity;
}
