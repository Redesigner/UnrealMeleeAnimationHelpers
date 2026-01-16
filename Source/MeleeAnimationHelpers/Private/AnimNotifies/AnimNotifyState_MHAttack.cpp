// Copyright (c) 2025 sleepysunset

#include "AnimNotifies/AnimNotifyState_MHAttack.h"

#include "MeleeAnimationHelpers.h"
#include "Hitboxes/MHHitboxComponent.h"
#include "Logging/StructuredLog.h"

void UAnimNotifyState_MHAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	const AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	UMHHitboxComponent* HitboxComponent = Owner->FindComponentByClass<UMHHitboxComponent>();
	if (!HitboxComponent)
	{
		UE_LOGFMT(MeleeAnimationHelpers, Warning,
			"Failed to clear hitboxes on Notify for animation '{AnimationName}'. Actor '{ActorName}' did not have a valid MHHitboxComponent.",
			GetNameSafe(Animation), GetNameSafe(Owner));
		return;
	}

	HitboxComponent->ClearHitActors(false);	
}
