#include "Core.h"
#include "CutsceneController.h"
#include <imgui.h>
#include <imgui_neo_sequencer.h>

CutsceneController::CutsceneController()
{
    activeCutscene = nullptr;
    transformOpen = true;
    m_time = 0;
}

void CutsceneController::AddCutscene( shared_ptr<Cutscene> cutscene )
{
    m_cutscenes.Add(cutscene);
}

bool CutsceneController::CutsceneActive()
{
	return m_cutSceneActive;
}

void CutsceneController::ActivateCutscene( int index )
{
    activeCutscene = m_cutscenes[index].get();
    channelTabs.Clear();
    for (int i = 0; i < activeCutscene->channels.Size(); i++)
    {
        channelTabs.Add( true );
    }
}

void CutsceneController::Update()
{


    // IMGUI timeline things.
	if (ImGui::Begin( "Cutscene controller" ))
	{
        ImGui::Checkbox("Cutscene Active", &m_cutSceneActive);

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

            if (activeCutscene != nullptr)
            {
                if (ImGui::BeginNeoGroup( activeCutscene->name.c_str(), &transformOpen ))
                {

                    for (int i = 0; i < activeCutscene->channels.Size(); i++)
                    {
                        if (ImGui::BeginNeoTimelineEx( activeCutscene->channels[i]->name.c_str(), &channelTabs[i]))
                        {
                            for (auto&& v : activeCutscene->channels[i]->keys)
                            {
                                ImGui::NeoKeyframe( &v->frame);

                                if (ImGui::IsNeoKeyframeSelected())
                                {
                                    selected = v->frame;
                                }
                                // Per keyframe code here
                            }

                            ImGui::EndNeoTimeLine();
                        }
                    }

                ImGui::EndNeoGroup();
                }
            }



            //if (ImGui::BeginNeoGroup( "Test group", &transformOpen )) 
            //{
            //    //std::vector<ImGui::FrameIndexType> keys = { 0, 10, 24 };
            //    if (ImGui::BeginNeoTimelineEx( "Test track" )) 
            //    {

            //        for (auto&& v : keys)
            //        {
            //            ImGui::NeoKeyframe( &v );

            //            if (ImGui::IsNeoKeyframeSelected())
            //            {
            //                selected = v;
            //            }          
            //            // Per keyframe code here
            //        }

            //        ImGui::EndNeoTimeLine();
            //    }
            //    ImGui::EndNeoGroup();
            //}

            ImGui::EndNeoSequencer();
        }
	}
	ImGui::End();
}
