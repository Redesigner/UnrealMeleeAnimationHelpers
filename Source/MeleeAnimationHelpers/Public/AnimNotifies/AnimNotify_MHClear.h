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

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
