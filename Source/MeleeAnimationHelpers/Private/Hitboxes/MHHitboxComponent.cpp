// Copyright (c) 2025 sleepysunset


#include "Hitboxes/MHHitboxComponent.h"

#include "Hitboxes/MHHitboxParameters.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"


UMHHitboxComponent::UMHHitboxComponent()
{
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
	OnHitboxOverlapped.Broadcast(OverlappedComponent, OtherComp, OtherActor);
}

void UMHHitboxComponent::ClearHitActors()
{
	HitObjects.Reset();
}

void UMHHitboxComponent::DestroyHitbox(UShapeComponent* Hitbox)
{
	if (!Hitbox)
	{
		return;
	}

	bool OwnsHitbox = false;
	for (int i = SpawnedHitboxes.Num() - 1; i >= 0; --i)
	{
		if (SpawnedHitboxes[i].Get() == Hitbox)
		{
			OwnsHitbox = true;
			SpawnedHitboxes.RemoveAt(i);
			break;
		}
	}

	if (!OwnsHitbox)
	{
		return;
	}
	
	Hitbox->DestroyComponent();
}

UShapeComponent* UMHHitboxComponent::SpawnHitbox_Implementation(const FMHHitboxParameters& HitboxParameters)
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}
	
	USkeletalMeshComponent* SkeletalMesh;
	if (const ACharacter* Character = Cast<ACharacter>(Owner))
	{
		SkeletalMesh = Character->GetMesh();
	}
	else
	{
		SkeletalMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	}
	
	if (!SkeletalMesh)
	{
		return nullptr;
	}
	
	UShapeComponent* SpawnedShape;
	
	switch (HitboxParameters.Shape)
	{
	case Box:
		{
			UBoxComponent* Box = NewObject<UBoxComponent>(this, "Hitbox");
			Box->SetBoxExtent(HitboxParameters.BoxExtents);
			SpawnedShape = Box;
			break;
		}
	case Capsule:
		{
			UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(this, "Hitbox");
			Capsule->SetCapsuleRadius(HitboxParameters.Radius);
			Capsule->SetCapsuleHalfHeight(HitboxParameters.HalfHeight);
			SpawnedShape = Capsule;
			break;
		}
	default:
	case Sphere:
		{
			USphereComponent* Sphere = NewObject<USphereComponent>(this, "Hitbox");
			Sphere->SetSphereRadius(HitboxParameters.Radius);
			SpawnedShape = Sphere;
			break;
		}
	}
	SpawnedShape->SetGenerateOverlapEvents(true);
	const FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	SpawnedShape->AttachToComponent(SkeletalMesh, AttachmentTransformRules);
	FMatrix HitboxTransformMatrix = CalculatePivotMatrix(HitboxParameters);
	SpawnedShape->SetRelativeLocationAndRotation(HitboxTransformMatrix.TransformPosition(FVector::ZeroVector) + HitboxParameters.Position, HitboxTransformMatrix.Rotator());
	
	// Set the collision type, but make the object type dynamic
	SpawnedShape->SetCollisionProfileName(CollisionProfile.Name);
	SpawnedShape->SetCollisionObjectType(ECC_WorldDynamic);
	SpawnedShape->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleHitboxOverlap);
	
	// Even though this ignores moving, it also works for the registration function later
	SpawnedShape->IgnoreActorWhenMoving(GetOwner(), true);
	SpawnedShape->SetHiddenInGame(false);
	SpawnedShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpawnedShape->SetVisibility(bShowHitboxes);

	// Overlaps are automatically updated here
	SpawnedShape->RegisterComponentWithWorld(GetWorld());

	SpawnedHitboxes.AddUnique(SpawnedShape);
	return SpawnedShape;
}

FMatrix UMHHitboxComponent::CalculatePivotMatrix(const FMHHitboxParameters& HitboxParameters)
{
	const FMatrix PivotTransform = FTransform(-HitboxParameters.OriginOffset).ToMatrixNoScale();
	const FMatrix InversePivotTransform = FTransform(HitboxParameters.OriginOffset).ToMatrixNoScale();
	return PivotTransform * HitboxParameters.Rotation.Quaternion().ToMatrix() * InversePivotTransform;
}
