// Copyright (c) 2025 sleepysunset


#include "Hitboxes/MHHitboxPriorityComponent.h"

#include "Components/ShapeComponent.h"

UMHHitboxPriorityComponent::UMHHitboxPriorityComponent(const FObjectInitializer&)
{
	// Tick is not absolutely necessary, but something needs to call ProcessPendingOverlaps
	PrimaryComponentTick.bCanEverTick = true;
}

UShapeComponent* UMHHitboxPriorityComponent::SpawnHitbox_Implementation(const FMHHitboxParameters& HitboxParameters)
{
	/*
	Interrupt the hitbox's spawning logic and record its priority in the list,
	because the default behavior for spawning hitboxes discards the hitbox
	parameters. This might be better as a default behavior, but I don't want the
	extra overhead for this in the regular component.
	*/
	UShapeComponent* NewHitbox = Super::SpawnHitbox_Implementation(HitboxParameters);
	HitboxPriority.Add(FObjectKey(NewHitbox), HitboxParameters.Priority);
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
	// Still check that an actor isn't in the list, so we don't have to deal with sorting it later
	if (const FObjectKey HitActor = FObjectKey(OtherActor); HitObjects.Contains(HitActor))
	{
		return;
	}

	const uint8* PriorityRaw = HitboxPriority.Find(OverlappedComponent);
	const FInstancedStruct* OptionalData = HitboxOptionalData.Find(OverlappedComponent);
	check(PriorityRaw && OptionalData);
	
	const uint8 Priority = *PriorityRaw;
	const FPendingHitboxOverlap OverlapEvent = FPendingHitboxOverlap(Priority, OverlappedComponent, OtherComp, OtherActor, *OptionalData);
	
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
		// Check first, because an early entry in this list might have registered a new hit
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
