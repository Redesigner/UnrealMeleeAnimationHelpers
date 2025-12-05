// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "Hitboxes/MHHitboxComponent.h"
#include "MHHitboxPriorityComponent.generated.h"

// Internal struct for stacking events
// Matches the FOnHitboxOverlapped delegate signature, but
// using TWeakObjectPtr, in case these objects expire
// before the queue can be cleared
struct FPendingHitboxOverlap
{
	uint8 Priority;
	TWeakObjectPtr<UPrimitiveComponent> OverlappedHitbox;
	TWeakObjectPtr<UPrimitiveComponent> OtherComponent;
	TWeakObjectPtr<AActor> OtherActor;
	FInstancedStruct OptionalData;
};

/**
 * @brief A hitbox component that handles priority for when multiple collisions occur at the same time
 */
UCLASS()
class MELEEANIMATIONHELPERS_API UMHHitboxPriorityComponent : public UMHHitboxComponent
{
	GENERATED_BODY()
	
	UMHHitboxPriorityComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual UShapeComponent* SpawnHitbox_Implementation(const FMHHitboxParameters& HitboxParameters) override;
	virtual void DestroyHitbox_Implementation(UShapeComponent* Hitbox) override;
	
	virtual void HandleHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Process the overlaps that have occurred, first sorting them by priority
	 */
	void ProcessPendingOverlaps();
	
	TArray<FPendingHitboxOverlap> PendingHitboxOverlaps;
	
	TMap<FObjectKey, uint8> HitboxPriority;
};
