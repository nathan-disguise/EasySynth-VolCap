#pragma once

#include "CoreMinimal.h"

#include "RendererTargets/RendererTarget.h"

/**
 * Class Tracking which renderer targets are requested to be rendered
*/
class FRendererTargetOptions
{
public:
    /** The enum containing all supported rendering targets */
    enum TargetType { COLOR_IMAGE, DEPTH_IMAGE, NORMAL_IMAGE, OPTICAL_FLOW_IMAGE, SEMANTIC_IMAGE, COUNT };
    enum EVolumetricFormat { NONE, INSTANT_NGP };

    FRendererTargetOptions();

    /** Select a rendering target */
    void SetSelectedTarget(const int TargetType, const bool Selected) { SelectedTargets[TargetType] = Selected; }

    /** Check if a rendering target is selected */
    bool TargetSelected(const int TargetType) const { return SelectedTargets[TargetType]; }

    /** Checks if any of the available options is selected */
    bool AnyOptionSelected() const;

    /** Set output format for the target */
    void SetOutputFormat(const int TargetType, const EImageFormat Selected) { OutputFormats[TargetType] = Selected; }

    /** Get selected output format for the target */
    EImageFormat OutputFormat(const int TargetType) const { return OutputFormats[TargetType]; }

    /** Updates should camera poses be exported */
    void SetExportCameraPoses(const bool bValue) { bExportCameraPoses = bValue; }

    /** Return should camera poses be exported */
    bool ExportCameraPoses() const { return bExportCameraPoses; }

    /** DepthRangeMetersValue getter */
    void SetDepthRangeMeters(const float DepthRangeMeters) { DepthRangeMetersValue = DepthRangeMeters; }

    /** DepthRangeMetersValue setter */
    float DepthRangeMeters() const { return DepthRangeMetersValue; }

    /** OpticalFlowScaleValue getter */
    void SetOpticalFlowScale(const float OpticalFlowScale) { OpticalFlowScaleValue = OpticalFlowScale; }

    /** OpticalFlowScaleValue setter */
    float OpticalFlowScale() const { return OpticalFlowScaleValue; }

    /** Populate provided queue with selected renderer targets */
    void GetSelectedTargets(
        UTextureStyleManager* TextureStyleManager,
        TQueue<TSharedPtr<FRendererTarget>>& OutTargetsQueue) const;

private:
    /** Get the renderer target object from the target type id */
    TSharedPtr<FRendererTarget> RendererTarget(const int TargetType, UTextureStyleManager* TextureStyleManager) const;

    /** Is the default color image rendering requested */
    TArray<bool> SelectedTargets;

    /** Selected output formats for each target */
    TArray<EImageFormat> OutputFormats;

    /** Selected volumetric output format */
    EVolumetricFormat VolumetricOutputFormat;

    /** Whether to export camera poses */
    bool bExportCameraPoses;

    /**
     * The clipping range when rendering the depth target
     * Larger values provide the longer range, but also the lower granularity
    */
    float DepthRangeMetersValue;

    /**
     * Multiplying coefficient for optical flow
     * Larger values increase color intensity, but also increase the chance of clipping
    */
    float OpticalFlowScaleValue;

    /** Default value for the depth range */
    static const float DefaultDepthRangeMetersValue;

    /** Default value for the optical flow scale */
    static const float DefaultOpticalFlowScaleValue;
};

