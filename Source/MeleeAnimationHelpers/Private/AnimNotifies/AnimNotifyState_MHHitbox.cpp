// Copyright (c) 2025 sleepysunset

#include "AnimNotifies/AnimNotifyState_MHHitbox.h"

#include "MeleeAnimationHelpers.h"
#include "Animation/AnimSequenceHelpers.h"
#include "Hitboxes/MHHitboxComponent.h"
#include "Logging/StructuredLog.h"
#include "Utility/MHDrawingHelpers.h"
#include "Components/ShapeComponent.h"

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
			"Failed to spawn hitbox on Notify Begin for animation '{AnimationName}'. Actor '{ActorName}' did not have a valid MHHitboxComponent.",
			GetNameSafe(Animation), GetNameSafe(Owner));
		return;
	}

	SpawnedHitboxes.Add(MeshComp, MakeWeakObjectPtr(HitboxComponent->SpawnHitbox(HitboxParameters)));
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
			"Failed to destroy hitbox on Notify End for animation '{AnimationName}'. Actor '{ActorName}' did not have a valid MHHitboxComponent.",
			GetNameSafe(Animation), GetNameSafe(Owner));
		return;
	}

	const TWeakObjectPtr<UShapeComponent>* Data = SpawnedHitboxes.Find(MeshComp);
	if (!Data)
	{
		UE_LOGFMT(MeleeAnimationHelpers, Warning,
			"Failed to destroy hitbox on Notify End for animation '{AnimationName}'."
			"Could not find the associate notify instance data.",
			GetNameSafe(Animation));
		return;
	}
	
	TWeakObjectPtr<UShapeComponent> SpawnedHitbox = *Data;
	if (!SpawnedHitbox.IsValid())
	{
		return;
	}
	
	HitboxComponent->DestroyHitbox(SpawnedHitbox.Get());
	SpawnedHitbox.Reset();
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
	FVector CurrentPosition = FVector::ZeroVector;
	
	if (CurrentMontage)
	{
		CurrentPosition = UE::Anim::ExtractRootMotionFromAnimationAsset(CurrentMontage, nullptr, 0.0f, CurrentTime).GetLocation();
	}
	
	if (CurrentTime >= StartTime && CurrentTime <= EndTime)
	{
		DrawPreviewShape(PDI, CurrentPosition, MeshComp);
	}
}

void UAnimNotifyState_MHHitbox::DrawCanvasInEditor(FCanvas& Canvas, FSceneView& View, USkeletalMeshComponent* MeshComp,
	const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	Super::DrawCanvasInEditor(Canvas, View, MeshComp, Animation, NotifyEvent);
}

void UAnimNotifyState_MHHitbox::DrawPreviewShape(FPrimitiveDrawInterface* PDI, const FVector& RootMotionOffset, const USkeletalMeshComponent* MeshComp) const
{
	FMatrix HitboxTransform;
	FMatrix BoneTransform;
	bool FoundBone = false;
	if (!HitboxParameters.BoneAttachment.IsNone())
	{
		if (const int BoneIndex = MeshComp->GetBoneIndex(HitboxParameters.BoneAttachment); BoneIndex != INDEX_NONE)
		{
			BoneTransform = MeshComp->GetBoneTransform(BoneIndex).ToMatrixNoScale();
			HitboxTransform = CachedPivotMatrix * FTransform(RootMotionOffset + HitboxParameters.Position).ToMatrixNoScale() * BoneTransform;
			FoundBone = true;
		}
	}
	
	if (!FoundBone)
	{
		HitboxTransform = CachedPivotMatrix * FTransform(RootMotionOffset + HitboxParameters.Position).ToMatrixNoScale();
	}
	
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
		if (FoundBone)
		{
			// The math here is... not great, but should be okay for most use cases
			DrawCoordinateSystem(PDI,
				RootMotionOffset + BoneTransform.TransformPosition(HitboxParameters.Position + HitboxParameters.OriginOffset),
				(BoneTransform.Rotator().Quaternion() * HitboxParameters.Rotation.Quaternion()).Rotator(), 25.0f, SDPG_Foreground);	
		}
		else
		{
			DrawCoordinateSystem(PDI, RootMotionOffset + HitboxParameters.Position + HitboxParameters.OriginOffset, HitboxParameters.Rotation, 25.0f, SDPG_Foreground);
		}
	}
}
#endif
