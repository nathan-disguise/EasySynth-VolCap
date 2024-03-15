// Copyright (c) 2022 YDrive Inc. All rights reserved.

#include "RendererTargets/CameraPoseExporter.h"

#include "Camera/CameraComponent.h"
#include "EntitySystem/Interrogation/MovieSceneInterrogationLinker.h"
#include "EntitySystem/MovieSceneEntitySystemTypes.h"
#include "ILevelSequenceEditorToolkit.h"
#include "ISequencer.h"
#include "Kismet/KismetMathLibrary.h"
#include "LevelSequence.h"
#include "Misc/FileHelper.h"
#include "MovieScene.h"
#include "MovieSceneObjectBindingID.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Tracks/MovieScene3DTransformTrack.h"

bool FCameraPoseExporter::ExportCameraRigPoses(const FString& RootFilePath, const FCameraRigData& rigData)
{
    // Create the file content
    TArray<FString> Lines;
    Lines.Reserve(rigData.RigPoses.Num() + 1);
    Lines.Add("id,tx,ty,tz,qx,qy,qz,qw,t,f");

    for (int i = 0; i < rigData.RigPoses.Num(); i++)
    {
        // Remove the scaling that makes no impact on camera functionality,
        // but my be used to scale the camera placeholder mesh as user desires
        const FVector Translation = rigData.RigPoses[i].Transform.GetTranslation();
        const FQuat Rotation = rigData.RigPoses[i].Transform.GetRotation();

        Lines.Add(FString::Printf(TEXT("%d,%f,%f,%f,%f,%f,%f,%f,%f,%d"),
            i,
            Translation.X, Translation.Y, Translation.Z,
            Rotation.X, Rotation.Y, Rotation.Z, Rotation.W,
            rigData.RigPoses[i].timestamp, rigData.RigPoses[i].frameNumber.Value));
    }

    // Save the file
    FString filePath = RootFilePath  / "CameraRigPoses.csv";
    if (!FFileHelper::SaveStringArrayToFile(
        Lines,
        *filePath,
        FFileHelper::EEncodingOptions::AutoDetect,
        &IFileManager::Get(),
        EFileWrite::FILEWRITE_None))
    {
        UE_LOG(LogEasySynth, Error, TEXT("%s: Failed while saving the file %s"), *FString(__FUNCTION__), *filePath)
            return false;
    }

    // Save the poses for each camera.
    for (const auto& CamEntry : rigData.Cameras) 
    {
        Lines.Empty();
        Lines.Add("id,tx,ty,tz,qx,qy,qz,qw,t,f");
        for (int i = 0; i < CamEntry.Poses.Num(); i++)
        {
            // Remove the scaling that makes no impact on camera functionality,
            // but my be used to scale the camera placeholder mesh as user desires
            const FVector Translation = CamEntry.Poses[i].Transform.GetTranslation();
            const FQuat Rotation = CamEntry.Poses[i].Transform.GetRotation();

            Lines.Add(FString::Printf(TEXT("%d,%f,%f,%f,%f,%f,%f,%f,%f,%d"),
                i,
                Translation.X, Translation.Y, Translation.Z,
                Rotation.X, Rotation.Y, Rotation.Z, Rotation.W,
                CamEntry.Poses[i].timestamp, CamEntry.Poses[i].frameNumber.Value));
        }

        // Save the file
        filePath = RootFilePath / CamEntry.CameraName / "CameraPosesNeo.csv";
        if (!FFileHelper::SaveStringArrayToFile(
            Lines,
            *filePath,
            FFileHelper::EEncodingOptions::AutoDetect,
            &IFileManager::Get(),
            EFileWrite::FILEWRITE_None))
        {
            UE_LOG(LogEasySynth, Error, TEXT("%s: Failed while saving the file %s"), *FString(__FUNCTION__), *filePath)
                return false;
        }
    }

    return true;
}