#pragma once
#include "Cutscene.h"

std::string SaveWWC( Cutscene* cutScene );

bool LoadWWC(Cutscene* outScene, std::string filePath);