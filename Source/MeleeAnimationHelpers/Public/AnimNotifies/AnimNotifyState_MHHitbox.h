// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Hitboxes/MHHitboxParameters.h"
#include "AnimNotifyState_MHHitbox.generated.h"

/**
 * An Anim Notify that spawns a hitbox if the owning Actor has a MHHitboxComponent
 */
UCLASS(DisplayName = "Hitbox State")
class MELEEANIMATIONHELPERS_API UAnimNotifyState_MHHitbox : public UAnimNotifyState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shape, Meta = (AllowPrivateAccess))
	FMHHitboxParameters HitboxParameters;

#if WITH_EDITORONLY_DATA
	/// Color to draw the hitbox as in the engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Display, Meta = (AllowPrivateAccess))
	FColor HitboxDisplayColor = FColor::Red;

	/// Draw the origin as RGB axes?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Display, Meta = (AllowPrivateAccess))
	bool bShowOriginInEditor = true;
#endif
	
	// Cached version of pivot matrix, to avoid recalculating every frame
	UPROPERTY()
	FMatrix CachedPivotMatrix;
	
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
    	const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual bool ShouldFireInEditor() override;
	
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp,
		const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
	
	virtual void DrawCanvasInEditor(FCanvas& Canvas, FSceneView& View, USkeletalMeshComponent* MeshComp,
		const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;

	void DrawPreviewShape(FPrimitiveDrawInterface* PDI, const FVector& RootMotionOffset, const USkeletalMeshComponent* MeshComp) const;
#endif

	TMap<FObjectKey, TWeakObjectPtr<UShapeComponent>> SpawnedHitboxes;
};
