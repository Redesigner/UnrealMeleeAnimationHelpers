// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "MHHitboxParameters.h"
#include "Components/ActorComponent.h"
#include "MHHitboxComponent.generated.h"


struct FMHHitboxParameters;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MELEEANIMATIONHELPERS_API UMHHitboxComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = true))
	FCollisionProfileName CollisionProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = true))
	bool bShowHitboxes = false;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHitboxOverlapped, UPrimitiveComponent*, OverlappedHitbox, UPrimitiveComponent*, OtherComponent, AActor*, OtherActor);

public:
	UPROPERTY(BlueprintAssignable)
	FOnHitboxOverlapped OnHitboxOverlapped;
	
	UFUNCTION(BlueprintNativeEvent)
	UShapeComponent* SpawnHitbox(const FMHHitboxParameters& HitboxParameters);

	void DestroyHitbox(const UShapeComponent* Hitbox);

	virtual UShapeComponent* SpawnHitbox_Implementation(const FMHHitboxParameters& HitboxParameters);

	static FMatrix CalculatePivotMatrix(const FMHHitboxParameters& HitboxParameters);
	
private:
	UMHHitboxComponent();

	UFUNCTION()
	void HandleHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	TArray<TWeakObjectPtr<UShapeComponent>> SpawnedHitboxes;

	TArray<FObjectKey> HitObjects;
};
