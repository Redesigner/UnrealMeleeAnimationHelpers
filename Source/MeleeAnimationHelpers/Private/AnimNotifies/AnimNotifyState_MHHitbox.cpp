// Copyright (c) 2025 sleepysunset

#include "AnimNotifies/AnimNotifyState_MHHitbox.h"

#include "MeleeAnimationHelpers.h"
#include "Animation/AnimSequenceHelpers.h"
#include "Hitboxes/MHHitboxComponent.h"
#include "Logging/StructuredLog.h"
#include "Utility/MHDrawingHelpers.h"

void UAnimNotifyState_MHHitbox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	const AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	UMHHitboxComponent* HitboxComponent = Owner->FindComponentByClass<UMHHitboxComponent>();
	if (!HitboxComponent)
	{
		UE_LOGFMT(MeleeAnimationHelpers, Warning,
			"Failed to spawn hitbox on Notify Begin for animation '{}'. Actor '{}' did not have a valid MHHitboxComponent.",
			GetNameSafe(Animation), GetNameSafe(Owner));
		return;
	}

	SpawnedHitbox = HitboxComponent->SpawnHitbox(HitboxParameters);
}

void UAnimNotifyState_MHHitbox::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
			"Failed to destroy hitbox on Notify End for animation '{}'. Actor '{}' did not have a valid MHHitboxComponent.",
			GetNameSafe(Animation), GetNameSafe(Owner));
		return;
	}

	if (!SpawnedHitbox.IsValid())
	{
		return;
	}
	
	HitboxComponent->DestroyHitbox(SpawnedHitbox.Get());
	SpawnedHitbox = nullptr;
}

#if WITH_EDITOR
bool UAnimNotifyState_MHHitbox::ShouldFireInEditor()
{
	return false;
}

void UAnimNotifyState_MHHitbox::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.PropertyChain.FindNode(StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UAnimNotifyState_MHHitbox, HitboxParameters))))
	{
		CachedPivotMatrix = UMHHitboxComponent::CalculatePivotMatrix(HitboxParameters);
	}
}

void UAnimNotifyState_MHHitbox::DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp,
	const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	Super::DrawInEditor(PDI, MeshComp, Animation, NotifyEvent);

	if (!bShouldFireInEditor)
	{
		return;
	}
	
	const float StartTime = NotifyEvent.GetTriggerTime();
	const float EndTime = NotifyEvent.GetEndTriggerTime();
	const UAnimMontage* CurrentMontage = Cast<UAnimMontage>(Animation);
	const float CurrentTime = CurrentMontage ? MeshComp->GetAnimInstance()->Montage_GetPosition(CurrentMontage) : -1.0f;
	FVector CurrentPosition = HitboxParameters.Position;
	
	if (CurrentMontage)
	{
		const FVector RootPositionOffset =  UE::Anim::ExtractRootMotionFromAnimationAsset(CurrentMontage, nullptr, 0.0f, CurrentTime).GetLocation();
		CurrentPosition += RootPositionOffset;
	}
	
	if (CurrentTime >= StartTime && CurrentTime <= EndTime)
	{
		DrawPreviewShape(PDI, CurrentPosition);
	}
}

void UAnimNotifyState_MHHitbox::DrawCanvasInEditor(FCanvas& Canvas, FSceneView& View, USkeletalMeshComponent* MeshComp,
	const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	Super::DrawCanvasInEditor(Canvas, View, MeshComp, Animation, NotifyEvent);
}

void UAnimNotifyState_MHHitbox::DrawPreviewShape(FPrimitiveDrawInterface* PDI, const FVector& RootMotionOffset) const
{
	const FMatrix HitboxTransform = CachedPivotMatrix * FTransform(HitboxParameters.Position + RootMotionOffset).ToMatrixNoScale();
	
	switch (HitboxParameters.Shape)
	{
	default:
		break;
	case Sphere:
		MHDrawingHelpers::DrawSphereHitbox(PDI, HitboxParameters.Radius, HitboxDisplayColor, HitboxTransform);
		break;
	case Box:
		MHDrawingHelpers::DrawBox(PDI, HitboxParameters.BoxExtents, HitboxDisplayColor, HitboxTransform);
		break;
	case Capsule:
		MHDrawingHelpers::DrawCapsuleHitbox(PDI, HitboxParameters.Radius, HitboxParameters.HalfHeight, HitboxDisplayColor, HitboxTransform);
		break;
	}
	
	if (bShowOriginInEditor)
	{
		DrawCoordinateSystem(PDI, RootMotionOffset + HitboxParameters.Position + HitboxParameters.OriginOffset, HitboxParameters.Rotation, 25.0f, SDPG_Foreground);
	}
}
#endif
