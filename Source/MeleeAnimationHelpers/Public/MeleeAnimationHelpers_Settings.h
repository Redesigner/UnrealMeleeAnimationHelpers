// Copyright (c) 2025 sleepysunset

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MeleeAnimationHelpers_Settings.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class MELEEANIMATIONHELPERS_API UMeleeAnimationHelpers_Settings : public UDeveloperSettings
{
	GENERATED_BODY()

	UMeleeAnimationHelpers_Settings(const FObjectInitializer& ObjectInitializer);

protected:
	/// Display a warning message when the Hitbox Component can't find a valid skeletal mesh to attach hitboxes to
	UPROPERTY(EditAnywhere)
	bool bWarnOnRootComponentFallback = true;

public:
	bool GetWarnOnRootComponentFallback() const { return bWarnOnRootComponentFallback; }
};
