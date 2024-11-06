// Copyright (c) 2022 YDrive Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Structure that contains intrinsic and extrinsic information
 * for each camera inside the camera rig and the camera rig itself.
 */
struct FCameraRigData
{
	/**
	 * Structures that contains information on a single camera. 
	 */
    struct FCameraPoses 
    {
        FTransform Transform;
        double timestamp;
        FFrameNumber frameNumber;
    };

	struct FCameraData
	{
		FString CameraName;
		double FocalLength;
        double FocalLengthX;
        double FocalLengthY;
		double PrincipalPointX;
		double PrincipalPointY;
        double vFov;
        double hFov;
		FIntPoint SensorSize;
        FRotator  rotation;
        FVector  location;
		FTransform Transform;
        TArray<FCameraPoses> Poses;
	};

	/**
	 * Array of cameras inside the camera rig
	 */
	TArray<FCameraData> Cameras;

    /**
        * Array of rig poses if avalible.
     */
    TArray<FCameraPoses> RigPoses;

    /**
     * Frame ranges in the sequence.
     */
    FFrameNumber frameStart;
    FFrameNumber frameEnd;
    FString sequenceName;
};