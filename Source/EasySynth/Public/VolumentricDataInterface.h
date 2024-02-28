// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CameraRig/CameraRigData.h"

/**
 * Class containing interface methods for generating
 * Volumetric Data Files for export. (e.g.InstantNGP)
 */
class EASYSYNTH_API VolumentricDataInterface
{
public:
	VolumentricDataInterface() = default;
	virtual ~VolumentricDataInterface() = default;

    virtual bool exportVolumetricData(const FString& OutputDir,
        const FCameraRigData& RigCameras,
        const FIntPoint& SensorSize) = 0;
};