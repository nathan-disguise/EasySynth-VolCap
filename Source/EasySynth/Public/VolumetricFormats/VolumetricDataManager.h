#pragma once

#include "CoreMinimal.h"

class ULevelSequense;

enum class EVolumetricFormat : uint8
{
    NONE,
    INSTANTNGP
};

class VolumetricDataManager 
{
public:
    VolumetricDataManager() = default;

    // Export formats.
    EVolumetricFormat GetExportFormat() { return m_exportFormat; }
    FString GetExportFormatString();
    void SetExportFormat(EVolumetricFormat format) { m_exportFormat = format; }

    bool exportVolumetricDataFromSequense(ULevelSequence* sequense);

private:
    EVolumetricFormat m_exportFormat = EVolumetricFormat::NONE;
};