// Copyright (c) 2025 sleepysunset


#include "Hitboxes/MHHitboxPriorityComponent.h"

#include "Components/ShapeComponent.h"

UMHHitboxPriorityComponent::UMHHitboxPriorityComponent(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

UShapeComponent* UMHHitboxPriorityComponent::SpawnHitbox_Implementation(const FMHHitboxParameters& HitboxParameters)
{
	UShapeComponent* NewHitbox = Super::SpawnHitbox_Implementation(HitboxParameters);
	HitboxPriority.Add(NewHitbox, HitboxParameters.Priority);
	return NewHitbox;
}

void UMHHitboxPriorityComponent::DestroyHitbox_Implementation(UShapeComponent* Hitbox)
{
	Super::DestroyHitbox_Implementation(Hitbox);
	HitboxPriority.Remove(Hitbox);
}

void UMHHitboxPriorityComponent::HandleHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const FObjectKey HitActor = FObjectKey(OtherActor); HitObjects.Contains(HitActor))
	{
		return;
	}

	const uint8 Priority = *HitboxPriority.Find(OverlappedComponent);
	
	FPendingHitboxOverlap OverlapEvent;
	
	if (const FInstancedStruct* OptionalData = HitboxOptionalData.Find(OverlappedComponent))
	{
		OverlapEvent = FPendingHitboxOverlap(Priority, OverlappedComponent, OtherComp, OtherActor, *OptionalData);
	}
	else
	{
		OverlapEvent = FPendingHitboxOverlap(Priority, OverlappedComponent, OtherComp, OtherActor, FInstancedStruct());
	}
	
	// Insert sort our pending overlaps
	for (int i = 0; i < PendingHitboxOverlaps.Num(); ++i)
	{
		if (Priority > PendingHitboxOverlaps[i].Priority)
		{
			PendingHitboxOverlaps.Insert(OverlapEvent, i);
			return;
		}
	}
	PendingHitboxOverlaps.Add(OverlapEvent);
}

void UMHHitboxPriorityComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	ProcessPendingOverlaps();
}

void UMHHitboxPriorityComponent::ProcessPendingOverlaps()
{
	for (const FPendingHitboxOverlap& PendingOverlap : PendingHitboxOverlaps)
	{
		if (HitObjects.Contains(PendingOverlap.OtherActor.Get()))
		{
			break;
		}
		
		UPrimitiveComponent* OverlappedHitbox = PendingOverlap.OverlappedHitbox.Get();
		if (!OverlappedHitbox)
		{
			break;
		}
		
		UPrimitiveComponent* OtherComponent = PendingOverlap.OtherComponent.Get();
		if (!OtherComponent)
		{
			break;
		}
		
		AActor* OtherActor = PendingOverlap.OtherActor.Get();
		if (!OtherActor)
		{
			break;
		}
		
		OnHitboxOverlapped.Broadcast(OverlappedHitbox, OtherComponent, OtherActor, PendingOverlap.OptionalData);
		HitObjects.Add(OtherActor);
	}
	PendingHitboxOverlaps.Reset();
}
