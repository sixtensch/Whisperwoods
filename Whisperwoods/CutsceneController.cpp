#include "Core.h"
#include "CutsceneController.h"
#include <imgui.h>
#include <imgui_neo_sequencer.h>
#include "WWCBuilder.h"

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
            if (dynamic_cast<CutsceneAnimatorChannel*>(activeCutscene->channels[i].get()))
                //if (activeCutscene->channels[i].get()->channelType == CutsceneTypeAnimator)
            {
                animatorChannels.Add((CutsceneAnimatorChannel*)activeCutscene->channels[i].get());


                //if (animIndex1 < 0)
                //{
                //    animatorChannel1 = (CutsceneAnimatorChannel*)activeCutscene->channels[i].get();
                //    animIndex1 = i;
                //    //break;
                //}
                //else
                //{
                //    animatorChannel2 = static_cast<CutsceneAnimatorChannel*>(activeCutscene->channels[i].get());
                //    animIndex2 = i;
                //    break;
                //}
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
        m_time += deltaTime *0.25f;
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
        ImGui::Text( "Current frame: %d \n Start Frame: %d \n End Frame: %d \n Transform open: %d \n selected %d", currentFrame, startFrame, endFrame, transformOpen, selected );

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
                    ImGui::TreePop();
                }
            }
            //if (animatorChannel1 != nullptr)
            //{
            //    shared_ptr<Animator> animator = animatorChannel1->targetAnimator;
            //    ImGui::Text("Animator channel 1: %d ", animIndex1);

            //    if (animator->loadedAnimations.Size() > 0)
            //    {
            //        for (int i = 0; i < animator->loadedAnimations.Size(); i++)
            //        {
            //            //if (&animator->loadedAnimations[i] != nullptr)
            //            ImGui::Text("Target animator anim %d ", i);
            //        }

            //        ImGui::DragInt("Animation index to add", &animationIndex);
            //        ImGui::DragFloat("Animation start time", &animationStartTime, 0.1f, 0.0f, 1.0f);
            //        ImGui::DragFloat("Animation play speed", &animationPlaySpeed, 0.1f, 0.0f, 2.0f);
            //        ImGui::Checkbox("Loop animation", &animationLoop);
            //        if (ImGui::Button("Add Animation Trigger Keyframe"))
            //        {
            //            animatorChannel1->AddKey(shared_ptr<CutsceneAnimationTriggerKey>(
            //                new CutsceneAnimationTriggerKey(
            //                    (float)currentFrame / (float)endFrame,
            //                    animationIndex,
            //                    animationStartTime,
            //                    animationPlaySpeed, animationLoop)));
            //            animatorChannel1->keys[animatorChannel1->keys.Size() - 1]->frame = currentFrame;
            //            std::sort(&animatorChannel1->keys.Front(), &animatorChannel1->keys.Back() + 1, CompareFrame);
            //        }
            //    }
            //}
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
                            if (activeCutscene->channels[i]->channelType == CutsceneTypeAnimator)
                            {
                                CutsceneAnimatorChannel* channel = (CutsceneAnimatorChannel*)activeCutscene->channels[i].get();
                                for (auto&& v : channel->keys)
                                {
                                    ImGui::NeoKeyframe(&v.frame);

                                    if (ImGui::IsNeoKeyframeSelected())
                                    {
                                        selected = v.frame;
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
