// Copyright (c) 2025 sleepysunset


#include "Hitboxes/MHHitboxComponent.h"

#include "MeleeAnimationHelpers.h"
#include "MeleeAnimationHelpers_Settings.h"
#include "Hitboxes/MHHitboxParameters.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Logging/StructuredLog.h"


UMHHitboxComponent::UMHHitboxComponent()
{
}

void UMHHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

	HitboxOrigin = SelectHitboxAttachment();
}

void UMHHitboxComponent::HandleHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FObjectKey HitActor = FObjectKey(OtherActor);
	if (HitObjects.Contains(HitActor))
	{
		return;
	}
	
	HitObjects.Add(HitActor);
	const FInstancedStruct* OptionalData = HitboxOptionalData.Find(OverlappedComponent);
	check(OptionalData);
	OnHitboxOverlapped.Broadcast(OverlappedComponent, OtherComp, OtherActor, *OptionalData);
	
}

void UMHHitboxComponent::ClearHitActors(bool UpdateOverlaps)
{
	HitObjects.Reset();

	if (UpdateOverlaps)
	{
		for (const auto& Hitbox : SpawnedHitboxes)
		{
			TArray<UPrimitiveComponent*> OverlappingComponents;
			Hitbox->GetOverlappingComponents(OverlappingComponents);
			for (const auto& OverlappingComponent : OverlappingComponents)
			{
				HandleHitboxOverlap(Hitbox.Get(), OverlappingComponent->GetOwner(), OverlappingComponent, 0, false, FHitResult());
			}
		}
	}
}

void UMHHitboxComponent::DestroyHitbox_Implementation(UShapeComponent* Hitbox)
{
	if (!Hitbox)
	{
		return;
	}

	bool OwnsHitbox = false;
	for (int i = SpawnedHitboxes.Num() - 1; i >= 0; --i)
	{
		if (UShapeComponent* SpawnedHitbox = SpawnedHitboxes[i].Get(); SpawnedHitbox == Hitbox)
		{
			OwnsHitbox = true;
			SpawnedHitboxes.RemoveAt(i);
			HitboxOptionalData.Remove(SpawnedHitbox);
			break;
		}
	}

	if (!OwnsHitbox)
	{
		return;
	}
	
	Hitbox->DestroyComponent();
}

USceneComponent* UMHHitboxComponent::SelectHitboxAttachment() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOGFMT(MeleeAnimationHelpers, Error, "Melee hitbox component '{HitboxComponentName}': does not have a valid owner.", GetNameSafe(this));
		return nullptr;
	}
	
	if (const ACharacter* Character = Cast<ACharacter>(Owner))
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh())
		{
			return SkeletalMeshComponent;	
		}
	}

	if (USkeletalMeshComponent* FirstFoundSkeletalMesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
	{
		return FirstFoundSkeletalMesh;
	}

	if (GetDefault<UMeleeAnimationHelpers_Settings>()->GetWarnOnRootComponentFallback())
	{
		UE_LOGFMT(MeleeAnimationHelpers, Warning, "Melee hitbox component '{HitboxComponentName}': could not find a valid skeletal mesh in '{OwnerActorName}',"
			" and OverrideAttachment was either not set, or was invalid. Falling back to Root Component.",
			GetNameSafe(this), GetNameSafe(Owner));
	}
	
	if (USceneComponent* RootComponent = Owner->GetRootComponent())
	{
		return RootComponent;
	}

	UE_LOGFMT(MeleeAnimationHelpers, Error, "Melee hitbox component '{HitboxComponentName}': Owning actor '{OwnerActorName}'"
		" does not have a valid root component.",
		GetNameSafe(this), GetNameSafe(Owner));
	return nullptr;
}

UShapeComponent* UMHHitboxComponent::SpawnHitbox_Implementation(const FMHHitboxParameters& HitboxParameters)
{	
	if (!HitboxOrigin.IsValid())
	{
		UE_LOGFMT(MeleeAnimationHelpers, Error, "Melee hitbox component '{HitboxComponentName}: Could not find a valid origin. "
			"The hitbox will not be spawned.'", GetNameSafe(this));
		return nullptr;
	}
	
	UShapeComponent* SpawnedShape;
	
	switch (HitboxParameters.Shape)
	{
	case Box:
		{
			UBoxComponent* Box = NewObject<UBoxComponent>(this);
			Box->SetBoxExtent(HitboxParameters.BoxExtents);
			SpawnedShape = Box;
			break;
		}
	case Capsule:
		{
			UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(this);
			Capsule->SetCapsuleRadius(HitboxParameters.Radius);
			Capsule->SetCapsuleHalfHeight(HitboxParameters.HalfHeight);
			SpawnedShape = Capsule;
			break;
		}
	default:
	case Sphere:
		{
			USphereComponent* Sphere = NewObject<USphereComponent>(this);
			Sphere->SetSphereRadius(HitboxParameters.Radius);
			SpawnedShape = Sphere;
			break;
		}
	}
	SpawnedShape->SetGenerateOverlapEvents(true);
	const FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	SpawnedShape->AttachToComponent(HitboxOrigin.Get(), AttachmentTransformRules, HitboxParameters.BoneAttachment);
	const FMatrix HitboxTransformMatrix = CalculatePivotMatrix(HitboxParameters);
	SpawnedShape->SetRelativeLocationAndRotation(HitboxTransformMatrix.TransformPosition(FVector::ZeroVector) + HitboxParameters.Position, HitboxTransformMatrix.Rotator());
	
	// Set the collision type
	SpawnedShape->SetCollisionProfileName(CollisionProfile.Name);
	SpawnedShape->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleHitboxOverlap);
	
	// Even though this ignores moving, it also works for the registration function later
	SpawnedShape->IgnoreActorWhenMoving(GetOwner(), true);
	SpawnedShape->SetHiddenInGame(false);
	SpawnedShape->SetVisibility(bShowHitboxes);

	// Overlaps are automatically updated here
	SpawnedShape->UpdateOverlaps();
	SpawnedShape->RegisterComponentWithWorld(GetWorld());

	SpawnedHitboxes.AddUnique(SpawnedShape);
	HitboxOptionalData.Add(SpawnedShape, HitboxParameters.Payload);
	
	return SpawnedShape;
}

FMatrix UMHHitboxComponent::CalculatePivotMatrix(const FMHHitboxParameters& HitboxParameters)
{
	const FMatrix PivotTransform = FTransform(-HitboxParameters.OriginOffset).ToMatrixNoScale();
	const FMatrix InversePivotTransform = FTransform(HitboxParameters.OriginOffset).ToMatrixNoScale();
	return PivotTransform * HitboxParameters.Rotation.Quaternion().ToMatrix() * InversePivotTransform;
}

bool UMHHitboxComponent::HasLiveHitboxes()
{
	return SpawnedHitboxes.Num() > 0;
}
