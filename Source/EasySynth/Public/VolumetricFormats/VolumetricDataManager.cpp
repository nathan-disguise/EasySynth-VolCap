#include "VolumetricDataManager.h"

#include "LevelSequense.h"

FString VolumetricDataManager::GetExportFormatString()
{
    if (m_exportFormat == EVolumetricFormat::INSTANTNGP)
        return TEXT("Instant NGP");
    return TEXT("Pick a Volumetric Data Format for Export.");
}

bool VolumetricDataManager::exportVolumetricDataFromSequense(ULevelSequence* sequense)
{
    // Check the cameras in the sequense and obtain information for the Lens and then export.
    return true;
}
