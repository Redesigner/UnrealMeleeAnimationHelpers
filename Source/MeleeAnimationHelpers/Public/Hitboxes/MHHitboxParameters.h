// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "StructUtils/InstancedStruct.h"
#include "MHHitboxParameters.generated.h"

UENUM()
enum EMHHitboxShape
{
	Box,
	Sphere,
	Capsule
};

USTRUCT(BlueprintType)
struct MELEEANIMATIONHELPERS_API FMHHitboxParameters
{
	GENERATED_BODY()

	/// Shape of the hitbox
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EMHHitboxShape> Shape = Sphere;

	/// Position of the hitbox. Offset from the Skeletal mesh's origin
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	/// Pivot point for rotations. Offset from Hitbox Position
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OriginOffset = FVector::ZeroVector;

	/// Shape rotations. Rotates around OriginOffset
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	/// Sphere or Cap radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 1 || Shape == 2", EditConditionHides, ClampMin = 0.0f))
	float Radius = 0.0f;

	/// Capsule half height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 2", EditConditionHides, ClampMin = 0.0f))
	float HalfHeight = 0.0f;

	/// Box half extents
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 0", EditConditionHides, ClampMin = 0.0f))
	FVector BoxExtents = FVector::ZeroVector;
	
	/// Optional payload for including whatever you want
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInstancedStruct Payload;
};
