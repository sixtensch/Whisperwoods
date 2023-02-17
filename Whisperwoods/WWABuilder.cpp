#include "Core.h"
#include "WWABuilder.h"
#include "FBXImporter.h"
#include "Resources.h"
#include "Debug.h"

void BuildWWA( std::string fbxPath )
{
	FBXImporter importer;
	AnimationResource animationsResourceWrite;
	importer.ImportFBXAnimations( fbxPath, &animationsResourceWrite );
	std::string output = importer.SaveWWA( &animationsResourceWrite, "Assets/Animations/" );
	LOG_TRACE( "WWA Build complete: %s - %s", fbxPath.c_str(), output.c_str() );
}
