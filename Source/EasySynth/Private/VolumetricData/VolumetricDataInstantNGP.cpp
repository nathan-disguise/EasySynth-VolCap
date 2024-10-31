#include "VolumetricData\VolumetricDataInstantNGP.h"

#include "SequenceRendererTargetOptions.h"

#include "Kismet/KismetMathLibrary.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool VolumetricDataInstantNGP::ExportVolumetricData(const FString& OutputDir, const FCameraRigData& RigCameras, const FRendererTargetOptions& RenderTargetOptions)
{
    // If were not exporting the color image, then we can't export the volumetric data.
    if (!RenderTargetOptions.TargetSelected(FRendererTargetOptions::COLOR_IMAGE))
    {
        UE_LOG(LogEasySynth, Error, TEXT("%s: Cannot export volumetric data without color image. Please select color image export."), *FString(__FUNCTION__));
        return false;
    }

    // Convert our RigCamera data to UStructs.
    FString fileContents;
    TSharedPtr<FJsonObject> exportData = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> frames;
    for (auto& Camera : RigCameras.Cameras)
    {
        if (Camera.Poses.Num() == 0)
        {
            for (FFrameNumber frameID = RigCameras.frameStart; frameID < RigCameras.frameEnd; frameID++)
            {
                TSharedPtr<FJsonObject> CameraEntry = MakeShareable(new FJsonObject);
                CameraEntry->SetStringField(TEXT("file_path"), Camera.CameraName / TEXT("/ColorImage/") / RigCameras.sequenceName + TEXT(".") + FString::Printf(TEXT("%04d"), frameID.Value) + getFileExtenstionString(RenderTargetOptions.OutputFormat(FRendererTargetOptions::COLOR_IMAGE)));
                CameraEntry->SetNumberField(TEXT("camera_angle_x"), Camera.hFov);
                CameraEntry->SetNumberField(TEXT("camera_angle_y"), Camera.vFov);
                CameraEntry->SetNumberField(TEXT("fl_x"), Camera.FocalLengthX);
                CameraEntry->SetNumberField(TEXT("fl_y"), Camera.FocalLengthY);
                //CameraEntry->SetNumberField(TEXT("cx"), Camera.PrincipalPointX);
                //CameraEntry->SetNumberField(TEXT("cy"), Camera.PrincipalPointY);
                CameraEntry->SetNumberField(TEXT("w"), Camera.SensorSize.X);
                CameraEntry->SetNumberField(TEXT("h"), Camera.SensorSize.Y);

                // TODO: Should add support for distortion parameters.
                CameraEntry->SetNumberField(TEXT("k1"), 0.0f);
                CameraEntry->SetNumberField(TEXT("k2"), 0.0f);
                CameraEntry->SetNumberField(TEXT("p1"), 0.0f);
                CameraEntry->SetNumberField(TEXT("p2"), 0.0f);

                // Treat this as a singular frame. With a singular pose.
                FTransform initTransform = Camera.Transform;
                initTransform.SetLocation(initTransform.GetLocation() * 0.01f);
                FMatrix cameraTransform = initTransform.ToMatrixNoScale().GetTransposed();
                cameraTransform = cameraTransform * FMatrix(
                    FPlane(-1, 0, 0, 0),
                    FPlane(0, 0, 1, 0),
                    FPlane(0, 1, 0, 0),
                    FPlane(0, 0, 0, 1)
                     ) * FMatrix(
                        FPlane(0, 0, 1, 0),
                        FPlane(1, 0, 0, 0),
                        FPlane(0, 1, 0, 0),
                        FPlane(0, 0, 0, 1)
                         ) * FQuat::MakeFromEuler(FVector(0.0f, 0.0f, 90.0f)).ToMatrix();
                TArray<TSharedPtr<FJsonValue>> transform;
                for (int i = 0; i < 4; i++)
                {
                    TArray<TSharedPtr<FJsonValue>> row;
                    for (int j = 0; j < 4; j++)
                    {
                        row.Add(MakeShareable(new FJsonValueNumber(cameraTransform.M[i][j])));
                    }
                    transform.Add(MakeShareable(new FJsonValueArray(row)));
                }
                CameraEntry->SetArrayField(TEXT("transform_matrix"), transform);
                frames.Add(MakeShareable(new FJsonValueObject(CameraEntry)));
            }
        }
        // TODO - Export but account for the different camera poses.
    }
    exportData->SetArrayField(TEXT("frames"), frames);

    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&fileContents);
    if (!FJsonSerializer::Serialize(exportData.ToSharedRef(), Writer)) 
    {
        UE_LOG(LogEasySynth, Error, TEXT("%s: Failed to construct JSON file."), *FString(__FUNCTION__));
        return false;
    }

    if (!FFileHelper::SaveStringToFile(fileContents, *FPaths::Combine(OutputDir, TEXT("transforms.json"))))
    {
        UE_LOG(LogEasySynth, Error, TEXT("%s: Failed to save JSON file to disk."), *FString(__FUNCTION__));
        return false;
    }

    return true;
}

FString VolumetricDataInstantNGP::getFileExtenstionString(EImageFormat imageFormat)
{
    switch (imageFormat) 
    {
        case EImageFormat::JPEG:
            return TEXT(".jpeg");
            break;
        case EImageFormat::PNG:
            return TEXT(".png");
            break;
        case EImageFormat::EXR:
            return TEXT(".exr");
            break;
    }

    return FString();
}
