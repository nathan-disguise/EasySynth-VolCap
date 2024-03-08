
#pragma once

#include "VolumetricData\VolumetricDataInterface.h"

/**
 * Exports Volumetric Data as InstantNGP Directory.
 */
class EASYSYNTH_API VolumetricDataInstantNGP : public VolumentricDataInterface
{
public:
    VolumetricDataInstantNGP() = default;
    ~VolumetricDataInstantNGP() = default;

    virtual bool ExportVolumetricData(const FString& OutputDir,
               const FCameraRigData& RigCameras) override;
};