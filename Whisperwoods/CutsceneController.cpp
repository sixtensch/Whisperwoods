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

bool CompareFrame(shared_ptr<CutsceneKey> i, shared_ptr<CutsceneKey> j)
{
    return (i->frame < j->frame);
}
bool CompareTime(shared_ptr<CutsceneKey> i, shared_ptr<CutsceneKey> j)
{
    return (i->time < j->time);
}

void CutsceneController::Update()
{
    // IMGUI timeline things.


    CutsceneCameraChannel* cameraChannel = nullptr;
    int camIndex = -1;
    if (activeCutscene)
    {
        for (int i = 0; i < activeCutscene->channels.Size(); i++)
        {
            if (static_cast<CutsceneCameraChannel*>(activeCutscene->channels[i].get()))
            {
                cameraChannel = static_cast<CutsceneCameraChannel*>(activeCutscene->channels[i].get());
                camIndex = i;
                break;
            }
        }
    }

#if WW_IMGUI 1
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

        if (cameraChannel)
        {
            Camera* cam = cameraChannel->targetCamera;
            Vec3 camPos = cam->GetPosition();
            Quaternion camRot = cam->GetRotation();
            ImGui::Text("Camera channel: %d ", camIndex);
            ImGui::Text("Camera Pos: %.3f %.3f %.3f \nCamera Rot %.3f %.3f %.3f %.3f ",
                camPos.x, camPos.y, camPos.z, camRot.x, camRot.y, camRot.z, camRot.w);
            if (ImGui::Button("Add Camera Keyframe"))
            {
                cameraChannel->AddKey(shared_ptr<CutsceneCameraKey>(new CutsceneCameraKey((float)currentFrame /(float)endFrame, camPos, camRot, 90, 1)));
                cameraChannel->keys[cameraChannel->keys.Size()-1]->frame = currentFrame;
                CutsceneCameraKey* debug = (CutsceneCameraKey*)cameraChannel->keys[cameraChannel->keys.Size() - 1].get();
                float time = debug->time;
                std::sort(&cameraChannel->keys.Front(), &cameraChannel->keys.Back() + 1, CompareFrame);
            }
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
#endif
}
