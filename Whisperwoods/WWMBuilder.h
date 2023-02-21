#pragma once

void BuildWWM( std::string fbxPath, bool rigged );

void BuildWWM( std::string fbxPath, bool rigged, float pushAway );

void BuildWWM(cs::List<VertexTextured> verticies, cs::List<int> indicies, std::string name);

void BuildRoomWWM( int subdivisions, float radius, float height, std::string name );