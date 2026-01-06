// Copyright (c) 2025 sleepysunset


#include "AnimNotifies/AnimNotify_MHClear.h"

#include "MeleeAnimationHelpers.h"
#include "Hitboxes/MHHitboxComponent.h"
#include "Logging/StructuredLog.h"

void UAnimNotify_MHClear::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
								 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

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

	HitboxComponent->ClearHitActors(RetriggerOverlaps);	
}
