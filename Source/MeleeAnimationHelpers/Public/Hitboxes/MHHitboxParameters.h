// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EMHHitboxShape> Shape = Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OriginOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 1 || Shape == 2", EditConditionHides, ClampMin = 0.0f))
	float Radius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 2", EditConditionHides, ClampMin = 0.0f))
	float HalfHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 0", EditConditionHides, ClampMin = 0.0f))
	FVector BoxExtents = FVector::ZeroVector;
};
