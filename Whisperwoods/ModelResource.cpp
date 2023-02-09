#include "Core.h"
#include "ModelResource.h"
#include "Debug.h"

UINT ModelStaticResource::GetVertexByteWidth() const
{
	return sizeof(VertexTextured) * verticies.Size();
}

UINT ModelRiggedResource::GetVertexByteWidth() const
{
	return sizeof(VertexRigged) * verticies.Size();
}
