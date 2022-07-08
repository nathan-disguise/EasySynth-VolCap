// Copyright (c) 2022 YDrive Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "CameraRig/CameraRigData.h"


/**
 * Class containing interface methods for parsing and generating
 * camera rig yaml files.
 */
class FCameraRigYamlInterface
{
public:
	FCameraRigYamlInterface() {}

	/** Handles importing camera ring from a yaml file */
	FReply OnImportCameraRigClicked();

	/** Handles exporting camera rig into a yaml file */
	bool ExportCameraRig(const FString& OutputDir);
};
