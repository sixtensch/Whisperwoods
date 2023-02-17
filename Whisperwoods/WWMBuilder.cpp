#include "Core.h"
#include "Vertex.h"
#include "WWMBuilder.h"
#include "FBXImporter.h"
#include "Resources.h"
#include "Debug.h"


void BuildWWM( std::string fbxPath, bool rigged )
{
	FBXImporter importer;
	if (rigged)
	{
		ModelRiggedResource riggedModelWrite;
		importer.ImportFBXRigged( fbxPath, &riggedModelWrite );
		std::string output = importer.SaveWMM( &riggedModelWrite, "Assets/Models/Rigged/" );
		LOG_TRACE( "Rigged WWM Build complete for: %s - %s", fbxPath.c_str(), output.c_str() );
	}
	else
	{
		ModelStaticResource staticModelWrite;
		importer.ImportFBXStatic( fbxPath, &staticModelWrite );
		std::string output = importer.SaveWMM( &staticModelWrite, "Assets/Models/Static/" );
		LOG_TRACE( "Static WWM Build complete: %s - %s", fbxPath.c_str(), output.c_str() );
	}
}

void BuildWWM(cs::List<VertexTextured> verticies, cs::List<int> indicies, std::string name)
{
	ModelStaticResource staticModelWrite;
	for (int i = 0; i < verticies.Size(); i++)
	{
		staticModelWrite.verticies.Add(verticies[i]);
	}
	for (int i = 0; i < indicies.Size(); i++)
	{
		staticModelWrite.indicies.Add(indicies[i]);
	}

	staticModelWrite.indexCounts.Add(indicies.Size());
	staticModelWrite.materialIndicies.Add(0);
	staticModelWrite.name = name;
	staticModelWrite.startIndicies.Add(0);
	staticModelWrite.materialNames.Add(name);

	FBXImporter importer;
	std::string output = importer.SaveWMM(&staticModelWrite, "Assets/Models/Static/");
	LOG_TRACE("Static WWM Custom Build complete: %s", output.c_str());
}
