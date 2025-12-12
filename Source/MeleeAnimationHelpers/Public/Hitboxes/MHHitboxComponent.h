// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "MHHitboxParameters.h"
#include "Components/ActorComponent.h"
#include "MHHitboxComponent.generated.h"


struct FMHHitboxParameters;

UCLASS(ClassGroup=("Melee Animation Helpers"), meta=(BlueprintSpawnableComponent), DisplayName="Hitbox Component")
class MELEEANIMATIONHELPERS_API UMHHitboxComponent : public UActorComponent
{
	GENERATED_BODY()

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = true))
	FCollisionProfileName CollisionProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = true))
	bool bShowHitboxes = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHitboxOverlapped, UPrimitiveComponent*, OverlappedHitbox, UPrimitiveComponent*,
		OtherComponent, AActor*, OtherActor, const FInstancedStruct&, OptionalData);

public:
	/** Event called when a hitbox is overlapped by another component. This event will not fire
	 * if there is already a melee hit registered with this actor. Make sure to call ClearHitActors
	 * once an animation is done playing. If you aren't modifying the main functions,
	 * this is where you want to hook in your damage or other hit logic.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnHitboxOverlapped OnHitboxOverlapped;

	/**
	 * Spawns a hitbox with the given parameters.
	 * Override to change spawning behavior.
	 * @param HitboxParameters packed parameter struct.
	 * @return Expects a pointer to the hitbox that was just spawned
	 */
	UFUNCTION(BlueprintNativeEvent)
	UShapeComponent* SpawnHitbox(const FMHHitboxParameters& HitboxParameters);

	/**
	 * Clear our list of actors, so we can register new hits
	 * Will automatically check for any currently existing overlaps
	 */
	UFUNCTION(BlueprintCallable)
	void ClearHitActors();

	/**
	 * Remove a hitbox from the skeletal mesh. Only removes hitboxes
	 * that were created by this component
	 * @param Hitbox Pointer to the hitbox we are destroying
	 */
	UFUNCTION(BlueprintNativeEvent)
	void DestroyHitbox(UShapeComponent* Hitbox);

	/**
	 * Compute a matrix from given hitbox parameters. Transformation matrix is rotated around the given
	 * origin offset, in local space.
	 * @param HitboxParameters Hitbox parameters to convert into a matrix.
	 * @return Matrix transformed around the new origin
	 */
	static FMatrix CalculatePivotMatrix(const FMHHitboxParameters& HitboxParameters);
	
	/**
	 * Determines if we have any live hitboxes in play
	 * @return true if we have any live hitboxes, false if not
	 */
	UFUNCTION(BlueprintCallable)
	bool HasLiveHitboxes();
	
protected:
	UMHHitboxComponent();

	virtual void BeginPlay() override;

	/**
	 * Hook for the individual shape component overlap function.
	 * By default, checks to see if the other actor has been hit before calling OnHitboxOverlapped,
	 * and looks up the relevant optional parameters from the optional data map.
	 * @param OverlappedComponent Owned component that was overlapped. Will always be a spawned hitbox.
	 * @param OtherActor Actor that was overlapped. Will not fire if this has already been hit.
	 * @param OtherComp Required delegate param
	 * @param OtherBodyIndex Required delegate param
	 * @param bFromSweep Required delegate param
	 * @param SweepResult Required delegate param
	 */
	UFUNCTION()
	virtual void HandleHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual UShapeComponent* SpawnHitbox_Implementation(const FMHHitboxParameters& HitboxParameters);
	virtual void DestroyHitbox_Implementation(UShapeComponent* Hitbox);

	/**
	 * Determine which scene component to use as the attachment parent for the hitboxes.
	 * In order of priority, selects the Skeletal Mesh of the character (if the parent actor is a character),
	 * then the first found Skeletal Mesh,
	 * then the root component.
	 * Override this if you want to change the way the Hitbox Component selects the attachment.
	 * By default, this is called once in BeginPlay.
	 * @return The scene component to use as the hitbox attachment
	 */
	virtual USceneComponent* SelectHitboxAttachment() const;

	/**
	 * The hitboxes that are currently "owned" by this component. Stored
	 * as weak pointers, because there is no guarantee that they won't be destroyed by
	 * something else
	 */
	TArray<TWeakObjectPtr<UShapeComponent>> SpawnedHitboxes;

	/**
	 * Map associating individual hitboxes with their optional data.
	 * Data is stored here instead of in the components themselves, so they
	 * can still be base primitive components.
	 */
	TMap<FObjectKey, FInstancedStruct> HitboxOptionalData;

	/** 
	 * Cached value for storing our calculated hitbox attachment.
	 * This is normally set in BeginPlay, by calling SelectHitboxAttachment.
	 */
	TWeakObjectPtr<USceneComponent> HitboxOrigin;

	/**
	 * Array of all objects that have been hit by an owned hitbox.
	 * Usually, this list is cleared by either reaching the end of
	 * an Attack Anim Notify state, or a clear hit Anim Notify
	 */
	TArray<FObjectKey> HitObjects;
};
