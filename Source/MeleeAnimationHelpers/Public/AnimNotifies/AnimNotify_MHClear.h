// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MHClear.generated.h"

/**
 * Clears the list of targets hit by hitboxes, optionally re-triggering any overlaps
 */
UCLASS(DisplayName = "Clear Hitboxes")
class MELEEANIMATIONHELPERS_API UAnimNotify_MHClear : public UAnimNotify
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual bool ShouldFireInEditor() override { return false; }
#endif

	/// If true, any current overlaps are retriggered when the Clear occurs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Retriggering, Meta = (AllowPrivateAccess))
	bool RetriggerOverlaps = true;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
