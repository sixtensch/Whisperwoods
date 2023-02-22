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

void BuildWWM( std::string fbxPath, bool rigged, float pushAway )
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
		importer.ImportFBXStatic( fbxPath, &staticModelWrite, pushAway );
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


void BuildRoomWWM( int subdivisions, float radius, float height, std::string name )
{
	// Create the verticies.
	cs::List<VertexTextured> verticies;
	for (int i = 0; i < subdivisions; i++)
	{
		// Base vertex
		float angle = ((cs::c_pi * 2) / (float)subdivisions) * (float)i;

		Vec3 pos( cos( angle ), 0, sin( angle ) );
		pos = pos * radius;
		Vec3 nor = -pos;
		nor.Normalize();
		Vec3 tan( 0, 1, 0 );
		Vec3 bitan = nor.Cross( tan );
		// Wall Base vertex
		verticies.Add( VertexTextured( pos, nor, tan, bitan, Vec4( ((1.0f / subdivisions) * i), 1, 0, 0 ) ) );
		// Wall Top vertex
		verticies.Add( VertexTextured( pos + Vec3( 0, height, 0 ), nor, tan, bitan, Vec4( ((1.0f / subdivisions) * i), 0, 0, 0 ) ) );
		// Floor edge vertex 
		verticies.Add( VertexTextured(
			pos,
			{ 0,1,0 }, // Normal
			{ 1,0,0 }, // Tangent
			{ 0,0,1 }, // Bitangent
			Vec4(
				1.0f - (0.5f + (cos( angle ) * 0.5f)), // UV-X (Circle)
				1.0f-(0.5f + (sin( angle ) * 0.5f)), // UV-Y
				0,
				0
			) ) );
	}
	VertexTextured centerVertex( { 0,0,0 }, { 0,1,0 }, { 1,0,0 }, { 0,0,1 }, { 0.5f,0.5f,0,0 } );

	// Create the faces
	cs::List<int> indicies;
	for (int i = 0; i < subdivisions; i++)
	{
		// top1->base1->top2 - triangle 1
		// base2->top2->base1 - triangle 2
		int currentIndex = i*3;
		int nextIndex = (i == subdivisions -1) ? 0 : (currentIndex + 3);

		// Wall Triangle 1
		indicies.Add( currentIndex + 1 ); // top1
		VertexTextured debug1 = verticies[currentIndex + 1];
		indicies.Add( currentIndex ); // base1
		VertexTextured debug2 = verticies[currentIndex];
		indicies.Add( nextIndex + 1 ); // top2
		VertexTextured debug3 = verticies[nextIndex + 1];

		// Wall Triangle 2
		indicies.Add( nextIndex ); // base2
		indicies.Add( nextIndex + 1 ); // top2
		indicies.Add( currentIndex ); // base1
	}

	int floorStart = indicies.Size();

	for (int i = 0; i < subdivisions; i++)
	{
		int currentIndex = i*3;
		int nextIndex = (i == subdivisions - 1) ? 0 : (currentIndex + 3);

		// Floor Triangle
		indicies.Add( verticies.Size() ); // Centre
		indicies.Add( nextIndex + 2 ); // second floor base
		indicies.Add( currentIndex + 2 ); // First floor base
	}

	// Add last so that verticies.size() works for the centre vert.
	verticies.Add( centerVertex );


	// Do the writing, modified for two materials.
	ModelStaticResource staticModelWrite;
	for (int i = 0; i < verticies.Size(); i++)
	{
		staticModelWrite.verticies.Add( verticies[i] );
	}
	for (int i = 0; i < indicies.Size(); i++)
	{
		staticModelWrite.indicies.Add( indicies[i] );
	}

	staticModelWrite.startIndicies.Add( 0 );
	staticModelWrite.startIndicies.Add( floorStart );

	staticModelWrite.indexCounts.Add( floorStart );
	staticModelWrite.indexCounts.Add( indicies.Size() - floorStart );

	staticModelWrite.materialIndicies.Add( 0 );
	staticModelWrite.materialIndicies.Add( 1 );

	staticModelWrite.materialNames.Add( "wall" );
	staticModelWrite.materialNames.Add( "floor" );

	staticModelWrite.name = name;

	FBXImporter importer;
	std::string output = importer.SaveWMM( &staticModelWrite, "Assets/Models/Static/" );
	LOG_TRACE( "Static WWM Custom Build complete: %s", output.c_str() );
}