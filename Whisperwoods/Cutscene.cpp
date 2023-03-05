#include "Core.h"
#include "Cutscene.h"

void Cutscene::AddChannel( shared_ptr<CutsceneChannel> newChannel )
{
	channels.Add( newChannel );
}

