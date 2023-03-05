#include "Core.h"
#include "CutsceneController.h"
#include <imgui.h>
#include <imgui_neo_sequencer.h>

CutsceneController::CutsceneController()
{
}

void CutsceneController::AddCutscene( shared_ptr<Cutscene> cutscene )
{
}

bool CutsceneController::CutsceneActive()
{
	return false;
}

void CutsceneController::ActivateCutscene( int index )
{
}

void CutsceneController::Update()
{


    // IMGUI timeline things.
	if (ImGui::Begin( "Cutscene controller" ))
	{
        
        ImGui::Text( "Current frame: %d \n Start Frame: %d \n End Frame: %d \n Transform open: %d \n selected %d", currentFrame, startFrame, endFrame, transformOpen, selected );


        ImGui::Text( "KeyFrames: " );
        for (int i = 0; i < keys.size(); i++)
        {
            ImGui::Text( "%d  Key: %d", i, keys[i]);
        }
        
        ImGui::DragInt( "Time to add", &keyTime );

        if (ImGui::Button( "Add Keyframe" ))
        {
            keys.push_back( keyTime );
        }


        if (ImGui::BeginNeoSequencer( "Sequencer", &currentFrame, &startFrame, &endFrame, {0,0}, ImGuiNeoSequencerFlags_EnableSelection | ImGuiNeoSequencerFlags_Selection_EnableDragging ))
        {

            
            if (ImGui::BeginNeoGroup( "Transform", &transformOpen )) 
            {
                //std::vector<ImGui::FrameIndexType> keys = { 0, 10, 24 };
                if (ImGui::BeginNeoTimelineEx( "Position" )) 
                {
                    ImGui::Text( "Test" );
                    for (auto&& v : keys)
                    {
                        ImGui::NeoKeyframe( &v );

                        if (ImGui::IsNeoKeyframeSelected())
                        {
                            selected = v;
                        }          
                        // Per keyframe code here
                    }

                    ImGui::EndNeoTimeLine();
                }
                ImGui::EndNeoGroup();
            }

            ImGui::EndNeoSequencer();
        }
	}
	ImGui::End();
}
