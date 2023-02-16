#include "Core.h"
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
