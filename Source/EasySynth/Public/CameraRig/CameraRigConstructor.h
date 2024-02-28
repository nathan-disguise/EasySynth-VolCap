// Copyright (c) 2022 YDrive Inc. All rights reserved. 
// Modified by Nathan Butt

#pragma once

#include "CameraRigData.h"
#include "SequencerWrapper.h"
#include "CineCameraComponent.h"

class CameraRigHelpers
{
public:
    static FCameraRigData constructCameraRig(ULevelSequence* RenderingSequense, TArray<UCameraComponent*> CameraRig, FIntPoint SensorSize);

private:
    static TArray<FCameraRigData::FCameraPoses> getCameraPoses(ULevelSequence* RenderingSequense, UCameraComponent* TargetCamera);
};