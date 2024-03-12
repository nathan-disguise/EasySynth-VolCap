#include "CameraRig/CameraRigConstructor.h"

#include "EasySynth.h"
#include "EntitySystem/Interrogation/MovieSceneInterrogationLinker.h"
#include "EntitySystem/MovieSceneEntitySystemTypes.h"
#include "ILevelSequenceEditorToolkit.h"
#include "ISequencer.h"
#include "Kismet/KismetMathLibrary.h"
#include "LevelSequence.h"
#include "MovieScene.h"
#include "MovieSceneObjectBindingID.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Tracks/MovieScene3DTransformTrack.h"


FCameraRigData CameraRigHelpers::constructCameraRig(ULevelSequence* RenderingSequense, TArray<UCameraComponent*> CameraRig, FIntPoint SensorSize)
{
    FCameraRigData rigData;
    
    for (auto& entry : CameraRig) 
    {
        FCameraRigData::FCameraData cameraDataEntry;
        cameraDataEntry.CameraName = entry->GetName();
        cameraDataEntry.Transform = entry->GetRelativeTransform();
        cameraDataEntry.SensorSize = SensorSize;
        cameraDataEntry.PrincipalPointX = SensorSize.X / 2.0f;
        cameraDataEntry.PrincipalPointY = SensorSize.Y / 2.0f;
        float hFov = entry->FieldOfView;
        float vFov = 2 * UKismetMathLibrary::DegAtan(UKismetMathLibrary::DegTan(hFov / 2.0f) * entry->AspectRatio);
        cameraDataEntry.hFov = hFov;
        cameraDataEntry.vFov = vFov;
        cameraDataEntry.FocalLengthX = SensorSize.X / UKismetMathLibrary::DegTan(hFov / 2.0f) / 2.0f;
        cameraDataEntry.FocalLength = cameraDataEntry.FocalLengthX;
        cameraDataEntry.FocalLengthY = SensorSize.Y / UKismetMathLibrary::DegTan(vFov / 2.0f) / 2.0f;
        cameraDataEntry.Poses = getCameraPoses(RenderingSequense, entry);
        rigData.Cameras.Add(cameraDataEntry);
    }

    return rigData;
}

TArray<FCameraRigData::FCameraPoses> CameraRigHelpers::getCameraPoses(ULevelSequence* RenderingSequense, UCameraComponent* TargetCamera)
{
    // Get level sequence fps
    TArray<FCameraRigData::FCameraPoses> Poses;
    FSequencerWrapper SequencerWrapper;
    if (!SequencerWrapper.OpenSequence(RenderingSequense))
    {
        UE_LOG(LogEasySynth, Error, TEXT("%s: Sequencer wrapper opening failed."), *FString(__FUNCTION__))
        return TArray<FCameraRigData::FCameraPoses>();
    }

    const FFrameRate DisplayRate = SequencerWrapper.GetMovieScene()->GetDisplayRate();
    const double FrameTime = 1.0f / DisplayRate.AsDecimal();
    double AccumulatedFrameTime = 0.0f;

    // Get level sequence ticks per second
    // Engine likes to update much more often than the video frame rate,
    // so this is needed to calculate engine ticks that correspond to frames
    const FFrameRate TickResolutions = SequencerWrapper.GetMovieScene()->GetTickResolution();

    // Calculate ticks per frame
    const int TicksPerFrame = TickResolutions.AsDecimal() / DisplayRate.AsDecimal();

    // Get the camera poses from each cut section
    TArray<UMovieSceneCameraCutSection*>& CutSections = SequencerWrapper.GetMovieSceneCutSections();
    for (auto CutSection : CutSections)
    {
        // Get the camera component
        UCameraComponent* Camera = CutSection->GetFirstCamera(
            *SequencerWrapper.GetSequencer(),
            SequencerWrapper.GetSequencer()->GetFocusedTemplateID());
        if (Camera != TargetCamera)
        {
            UE_LOG(LogEasySynth, Error, TEXT("%s: Cut section camera component is null"), *FString(__FUNCTION__))
            continue;
        }

        // Get the current cut section camera binding id
        const FMovieSceneObjectBindingID& CameraBindingID = CutSection->GetCameraBindingID();

        // Find the track inside the level sequence that corresponds to the
        // pose transformation of the camera
        UMovieScene3DTransformTrack* CameraTransformTrack = nullptr;
        for (const FMovieSceneBinding& Binding : SequencerWrapper.GetMovieScene()->GetBindings())
        {
            if (Binding.GetObjectGuid() == CameraBindingID.GetGuid())
            {
                for (UMovieSceneTrack* Track : Binding.GetTracks())
                {
                    CameraTransformTrack = Cast<UMovieScene3DTransformTrack>(Track);
                    if (CameraTransformTrack != nullptr)
                    {
                        break;
                    }
                }
            }
        }
        if (CameraTransformTrack == nullptr)
        {
            UE_LOG(LogEasySynth, Error, TEXT("%s: Could not find camera transform track"), *FString(__FUNCTION__))
            continue;
        }

        // Interrogator object that queries the transformation track for camera poses
        UE::MovieScene::FSystemInterrogator Interrogator;

        // Inclusive lower bound of the movie scene ticks that belong to this cut section
        FFrameNumber StartTickNumber = CutSection->GetTrueRange().GetLowerBoundValue();
        // Exclusive upper bound of the movie scene ticks that belong to this cut section
        FFrameNumber EndTickNumber = CutSection->GetTrueRange().GetUpperBoundValue();
        for (FFrameNumber TickNumber = StartTickNumber; TickNumber < EndTickNumber; TickNumber += TicksPerFrame)
        {
            // Reinitialize the interrogator for each frame
            Interrogator.Reset();
            TGuardValue<UE::MovieScene::FEntityManager*> DebugVizGuard(
                UE::MovieScene::GEntityManagerForDebuggingVisualizers, &Interrogator.GetLinker()->EntityManager);
            Interrogator.ImportTrack(CameraTransformTrack, UE::MovieScene::FInterrogationChannel::Default());

            // Add frame interrogation
            if (Interrogator.AddInterrogation(TickNumber) == INDEX_NONE)
            {
                UE_LOG(LogEasySynth, Error, TEXT("%s: Adding interrogation failed"), *FString(__FUNCTION__))
                break;
            }
            Interrogator.Update();

            // Get the camera pose transform for the frame
            // Engine crashes in case multiple interrogations are added at once
            TArray<FTransform> TempTransforms;
            Interrogator.QueryWorldSpaceTransforms(UE::MovieScene::FInterrogationChannel::Default(), TempTransforms);
            if (TempTransforms.Num() == 0)
            {
                UE_LOG(LogEasySynth, Error, TEXT("%s: No camera transforms found"), *FString(__FUNCTION__))
                break;
            }

            for (FTransform& Transform : TempTransforms)
            {

                AccumulatedFrameTime += FrameTime;
                Poses.Add(FCameraRigData::FCameraPoses{ Transform, AccumulatedFrameTime });
            }
        }
    }

    return Poses;
}