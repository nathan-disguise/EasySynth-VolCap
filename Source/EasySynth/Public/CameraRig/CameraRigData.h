// Copyright (c) 2022 YDrive Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Structure that contains intrinsic and extrinsic information
 * for each camera inside the camera rig
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
		FTransform Transform;
        TArray<FCameraPoses> Poses;
	};

	/**
	 * Array of cameras inside the camera rig
	 */
	TArray<FCameraData> Cameras;
};