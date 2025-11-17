// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_MHAttack.generated.h"

/**
 * 
 */
UCLASS()
class MELEEANIMATIONHELPERS_API UAnimNotifyState_MHAttack : public UAnimNotifyState
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual bool ShouldFireInEditor() override { return false; }
#endif

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
