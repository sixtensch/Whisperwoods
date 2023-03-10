#include "Core.h"
#include "CutsceneController.h"
#include <imgui.h>
#include <imgui_neo_sequencer.h>
#include "WWCBuilder.h"
#include "Input.h"
CutsceneController::CutsceneController()
    :
    m_cutSceneActive(false),
    m_isPlaying(false)
{
    activeCutscene = nullptr;
    transformOpen = true;
    m_time = 0;
    m_playbackRate = 0.2f;
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

bool CompareFrame(CutsceneKey i, CutsceneKey j)
{
    return (i.frame < j.frame);
}
bool CompareTime(CutsceneKey i, CutsceneKey j)
{
    return (i.time < j.time);
}

void CutsceneController::Update(float deltaTime)
{
    // IMGUI timeline things.


    CutsceneCameraChannel* cameraChannel = nullptr;
    // CutsceneAnimatorChannel* animatorChannel1 = nullptr;
    // CutsceneAnimatorChannel* animatorChannel2 = nullptr;

    cs::List<CutsceneAnimatorChannel*> animatorChannels;
    cs::List<CutsceneTransformChannel*> transformChannels;
    cs::List<CutsceneGUIChannel*> guiChannels;

    int camIndex = -1;
    // int animIndex1 = -1;
    // int animIndex2 = -1;

    if (activeCutscene)
    {
        for (int i = 0; i < activeCutscene->channels.Size(); i++)
        {
            if (dynamic_cast<CutsceneCameraChannel*>(activeCutscene->channels[i].get()))
                //if (activeCutscene->channels[i].get()->channelType == CutsceneTypeCamera)
            {
                cameraChannel = (CutsceneCameraChannel*)(activeCutscene->channels[i].get());
                camIndex = i;
                break;
            }
        }

        for (int i = 0; i < activeCutscene->channels.Size(); i++)
        {
                //if (activeCutscene->channels[i].get()->channelType == CutsceneTypeAnimator)
            if (dynamic_cast<CutsceneAnimatorChannel*>(activeCutscene->channels[i].get()))
            {
                animatorChannels.Add((CutsceneAnimatorChannel*)activeCutscene->channels[i].get());
            }
            else if (dynamic_cast<CutsceneTransformChannel*>(activeCutscene->channels[i].get()))
            {
                transformChannels.Add( (CutsceneTransformChannel*)activeCutscene->channels[i].get() );
            }
            else if (dynamic_cast<CutsceneGUIChannel*>(activeCutscene->channels[i].get()))
            {
                guiChannels.Add( (CutsceneGUIChannel*)activeCutscene->channels[i].get() );
            }
        }
    }

    if (m_cutSceneActive && activeCutscene)
    {
       
        for (int i = 0; i < activeCutscene->channels.Size(); i++)
        {
            activeCutscene->channels[i]->Update(m_time, endFrame);
        }
    }
    if (m_isPlaying)
    {
        m_time += deltaTime * 0.1f * m_playbackRate;
        if (m_time > 0.99f) m_time = 0;
        currentFrame = m_time * endFrame;
    }
    else
    {
        m_time = (float)currentFrame / (float)endFrame;
    }



	if (ImGui::Begin( "Cutscene controller" ))
	{
        ImGui::Checkbox("Cutscene Active", &m_cutSceneActive);
        ImGui::Checkbox("Cutscene Playing", &m_isPlaying);
        ImGui::DragFloat( "Playback speed", &m_playbackRate );
        ImGui::Text( "Current frame: %d \n Start Frame: %d \n End Frame: %d \n Transform open: %d \n selected %d", currentFrame, startFrame, endFrame, transformOpen, selected );


        bool removeSelected = false;
        if (ImGui::Button( "Remove Selected Keyframe" ))
        {
            removeSelected = true;
        }
        /*ImGui::Text( "KeyFrames: " );
        for (int i = 0; i < keys.size(); i++)
        {
            ImGui::Text( "%d  Key: %d", i, keys[i]);
        }      
        ImGui::DragInt( "Time to add", &keyTime );
        if (ImGui::Button( "Add Keyframe" ))
        {
            keys.push_back( keyTime );
        }*/

        if (ImGui::CollapsingHeader("Camera Key Adding", m_cameraKeyOpen))
        {
            if (cameraChannel != nullptr)
            {
                Camera* cam = cameraChannel->targetCamera;
                Vec3 camPos = cam->GetPosition();
                Quaternion camRot = cam->GetRotation();
                ImGui::Text("Camera channel: %d ", camIndex);
                ImGui::Text("Camera Pos: %.3f %.3f %.3f \nCamera Rot %.3f %.3f %.3f %.3f ",
                    camPos.x, camPos.y, camPos.z, camRot.x, camRot.y, camRot.z, camRot.w);
                if (ImGui::Button("Add Camera Keyframe"))
                {
                    cameraChannel->AddKey(CutsceneCameraKey((float)currentFrame / (float)endFrame, camPos, camRot, 90, 1));
                    cameraChannel->keys[cameraChannel->keys.Size() - 1].frame = currentFrame;
                    //CutsceneCameraKey* debug = (CutsceneCameraKey*)cameraChannel->keys[cameraChannel->keys.Size() - 1];
                    //float time = debug->time;
                    std::sort(&cameraChannel->keys.Front(), &cameraChannel->keys.Back() + 1, CompareFrame);
                }
            }
        }

        static int animationIndex;
        static float animationStartTime;
        static float animationPlaySpeed;
        static bool animationLoop;
        if (ImGui::CollapsingHeader("Animation Key Adding", m_animatorKeyOpen))
        {
            for (int i = 0; i < animatorChannels.Size(); i++)
            {
                CutsceneAnimatorChannel* channel = animatorChannels[i];
                std::string headerText = "Animator channel: " + channel->name;
                if (ImGui::TreeNode(headerText.c_str()))
                {
                    Animator* animator = channel->targetAnimator;
                    //ImGui::Text("Animator channel: %s ", channel->name.c_str());

                    if (animator->loadedAnimations.Size() > 0)
                    {
                        for (int i = 0; i < animator->loadedAnimations.Size(); i++)
                        {
                            //if (&animator->loadedAnimations[i] != nullptr)
                            ImGui::Text("Target animator anim %s ", animator->loadedAnimations[i].sourceAnimation->name.c_str());
                        }

                        ImGui::DragInt("Animation index to add", &animationIndex);
                        ImGui::DragFloat("Animation start time", &animationStartTime, 0.1f, 0.0f, 1.0f);
                        ImGui::DragFloat("Animation play speed", &animationPlaySpeed, 0.1f, 0.0f, 2.0f);
                        ImGui::Checkbox("Loop animation", &animationLoop);
                        if (ImGui::Button("Add Animation Trigger Keyframe"))
                        {
                            channel->AddKey(CutsceneAnimationTriggerKey(
                                    (float)currentFrame / (float)endFrame,
                                    animationIndex,
                                    animationStartTime,
                                    animationPlaySpeed, animationLoop));
                            channel->keys[channel->keys.Size() - 1].frame = currentFrame;
                            std::sort(&channel->keys.Front(), &channel->keys.Back() + 1, CompareFrame);
                        }
                    }
                    else
                    {

                    }
                    ImGui::TreePop();
                }
            }
        }

        static Vec3 posValue;
        static Vec3 rotValue;
        if (ImGui::CollapsingHeader( "Transform Key Adding", m_transformKeyOpen ))
        {
            for (int i = 0; i < transformChannels.Size(); i++)
            {
                CutsceneTransformChannel* channel = transformChannels[i];
                std::string headerText = "Transform channel: " + channel->name;
                if (ImGui::TreeNode( headerText.c_str() ))
                {
                    Transform* target = channel->targetTransform;
                    //ImGui::Text("Animator channel: %s ", channel->name.c_str());
                    ImGui::DragFloat3( "Key Position", (float*)&posValue, 0.1f);
                    ImGui::DragFloat3( "Key Euler Rot", (float*)&rotValue, 0.1f);
                    if (ImGui::Button( "Add Transform Keyframe" ))
                    {
                        channel->AddKey( CutsceneTransformKey(
                            (float)currentFrame / (float)endFrame, posValue, Quaternion::GetEuler(rotValue), Vec3(1,1,1)));
                        channel->keys[channel->keys.Size() - 1].frame = currentFrame;
                        std::sort( &channel->keys.Front(), &channel->keys.Back() + 1, CompareFrame );
                    }
                    ImGui::TreePop();
                }
            }
        }


        //static int targetIDVal;
        static float alphaVal;
        static bool activeVal;
        static Vec3 colorVal(1,1,1);
        if (ImGui::CollapsingHeader( "GUI Key Adding", m_uiKeyOpen ))
        {
            for (int i = 0; i < guiChannels.Size(); i++)
            {
                CutsceneGUIChannel* channel = guiChannels[i];
                std::string headerText = "GUI channel: " + channel->name;
                if (ImGui::TreeNode( headerText.c_str() ))
                {
                    GUI* target = channel->targetGUI;
                    //ImGui::Text("Animator channel: %s ", channel->name.c_str());
                    //ImGui::DragInt( "Key alpha", &targetIDVal, 0.1f );
                    ImGui::DragFloat( "Key alpha", &alphaVal, 0.1f );
                    ImGui::Checkbox( "Key element active", & activeVal );
                    ImGui::DragFloat3( "Key Color", (float*)&colorVal, 0.1f );
                    if (ImGui::Button( "Add GUI Keyframe" ))
                    {
                        channel->AddKey( CutsceneGUITriggerKey(
                            (float)currentFrame / (float)endFrame, 
                            alphaVal,
                            activeVal,
                            colorVal ) );
                        channel->keys[channel->keys.Size() - 1].frame = currentFrame;
                        std::sort( &channel->keys.Front(), &channel->keys.Back() + 1, CompareFrame );
                    }
                    ImGui::TreePop();
                }
            }
        }


        static std::string saveName;
        static char buf[128];
        if (ImGui::CollapsingHeader("WWC Saving", m_savingOpen))
        {
           // ImGui::InputText("Save Name", buf, 128);
            //activeCutscene->name = std::string(buf);
            if (ImGui::Button("Save current timeline"))
            {
                SaveWWC(activeCutscene);
            }
        }


        bool doDelete = false;
        if (ImGui::BeginNeoSequencer( "Sequencer", &currentFrame, &startFrame, &endFrame, {0,0}, ImGuiNeoSequencerFlags_EnableSelection | ImGuiNeoSequencerFlags_Selection_EnableDragging |
            ImGuiNeoSequencerFlags_Selection_EnableDeletion
        ))
        {

            if (activeCutscene != nullptr)
            {
                if (ImGui::BeginNeoGroup( activeCutscene->name.c_str(), &transformOpen ))
                {

                    for (int i = 0; i < activeCutscene->channels.Size(); i++)
                    {
                        if (ImGui::BeginNeoTimelineEx( activeCutscene->channels[i]->name.c_str(), &channelTabs[i]))
                        {
                            if (activeCutscene->channels[i]->channelType == CutsceneTypeAnimator)
                            {
                                CutsceneAnimatorChannel* channel = (CutsceneAnimatorChannel*)activeCutscene->channels[i].get();
                                for (auto&& v : channel->keys)
                                {
                                    ImGui::NeoKeyframe(&v.frame);

                                    if (ImGui::IsNeoKeyframeSelected())
                                    {
                                        selected = v.frame;
                                        //LOG_TRACE( "Animator key" );
                                        if( Input::Get().IsDXKeyPressed( DXKey::Delete ) || removeSelected)
                                        {
                                            removeSelected = false;
                                            int indexToRemove;
                                            for (int i = 0; i < channel->keys.Size(); i++)
                                            {
                                                if (channel->keys[i].frame == selected)
                                                {
                                                    indexToRemove = i;
                                                    break;
                                                }
                                            }
                                            channel->keys.Remove( indexToRemove );
                                        }
                                    }
                                    // Per keyframe code here

                                }
                            }
                            else if (activeCutscene->channels[i]->channelType == CutsceneTypeTransform)
                            {
                                CutsceneTransformChannel* channel = (CutsceneTransformChannel*)activeCutscene->channels[i].get();
                                for (auto&& v : channel->keys)
                                {
                                    ImGui::NeoKeyframe(&v.frame);

                                    if (ImGui::IsNeoKeyframeSelected())
                                    {
                                        selected = v.frame;
                                        if (Input::Get().IsDXKeyPressed( DXKey::Delete ) || removeSelected)
                                        {
                                            removeSelected = false;
                                            int indexToRemove;
                                            for (int i = 0; i < channel->keys.Size(); i++)
                                            {
                                                if (channel->keys[i].frame == selected)
                                                {
                                                    indexToRemove = i;
                                                    break;
                                                }
                                            }
                                            channel->keys.Remove( indexToRemove );
                                        }
                                    }
                                    // Per keyframe code here
                                }
                            }
                            else if (activeCutscene->channels[i]->channelType == CutsceneTypeCamera)
                            {
                                CutsceneCameraChannel* channel = (CutsceneCameraChannel*)activeCutscene->channels[i].get();
                                for (auto&& v : channel->keys)
                                {
                                    ImGui::NeoKeyframe(&v.frame);

                                    if (ImGui::IsNeoKeyframeSelected())
                                    {
                                        selected = v.frame;
                                        if (Input::Get().IsDXKeyPressed( DXKey::Delete ) || removeSelected)
                                        {
                                            removeSelected = false;
                                            int indexToRemove;
                                            for (int i = 0; i < channel->keys.Size(); i++)
                                            {
                                                if (channel->keys[i].frame == selected)
                                                {
                                                    indexToRemove = i;
                                                    break;
                                                }
                                            }
                                            channel->keys.Remove( indexToRemove );
                                        }
                                    }
                                    // Per keyframe code here
                                }
                            }
                            else if (activeCutscene->channels[i]->channelType == CutsceneTypeGUI)
                            {
                                CutsceneGUIChannel* channel = (CutsceneGUIChannel*)activeCutscene->channels[i].get();
                                for (auto&& v : channel->keys)
                                {
                                    ImGui::NeoKeyframe(&v.frame);

                                    if (ImGui::IsNeoKeyframeSelected())
                                    {
                                        selected = v.frame;
                                        if (Input::Get().IsDXKeyPressed( DXKey::Delete ) || removeSelected)
                                        {
                                            removeSelected = false;
                                            int indexToRemove;
                                            for (int i = 0; i < channel->keys.Size(); i++)
                                            {
                                                if (channel->keys[i].frame == selected)
                                                {
                                                    indexToRemove = i;
                                                    break;
                                                }
                                            }
                                            channel->keys.Remove( indexToRemove );
                                        }
                                    }
                                    // Per keyframe code here
                                }
                            }
                            else if (activeCutscene->channels[i]->channelType == CutsceneTypeText)
                            {
                                CutsceneTextChannel* channel = (CutsceneTextChannel*)activeCutscene->channels[i].get();
                                for (auto&& v : channel->keys)
                                {
                                    ImGui::NeoKeyframe(&v.frame);

                                    if (ImGui::IsNeoKeyframeSelected())
                                    {
                                        selected = v.frame;
                                        if (Input::Get().IsDXKeyPressed( DXKey::Delete ) || removeSelected)
                                        {
                                            removeSelected = false;
                                            int indexToRemove;
                                            for (int i = 0; i < channel->keys.Size(); i++)
                                            {
                                                if (channel->keys[i].frame == selected)
                                                {
                                                    indexToRemove = i;
                                                    break;
                                                }
                                            }
                                            channel->keys.Remove( indexToRemove );
                                        }
                                    }
                                    // Per keyframe code here
                                }
                            }
                            //for (int i = 0; i < activeCutscene->channels[i]->numKeys; i++)
                            //{
                            //    CutsceneAnimatorChannel* channel = (CutsceneAnimatorChannel*)&activeCutscene->channels[i];
                            //    for (auto&& v : channel->keys)
                            //    {
                            //        ImGui::NeoKeyframe(&v.frame);

                            //        if (ImGui::IsNeoKeyframeSelected())
                            //        {
                            //            selected = v.frame;
                            //        }
                            //        // Per keyframe code here
                            //    }

                            //}
                            
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
