// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeleeAnimationHelpersEditor.h"

#include "Hitboxes/HitboxComponentDetails.h"
#include "Hitboxes/MHHitboxOverlapParametersOverride.h"

#define LOCTEXT_NAMESPACE "FMeleeAnimationHelpersEditorModule"
void FMeleeAnimationHelpersEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout
	(
		FMHHitboxOverlapParametersOverride::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FHitboxComponentDetails::MakeInstance)
	);

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FMeleeAnimationHelpersEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		// unregister properties when the module is shutdown
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FMHHitboxOverlapParametersOverride::StaticStruct()->GetFName());
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMeleeAnimationHelpersEditorModule, MeleeAnimationHelpersEditor)

DEFINE_LOG_CATEGORY(MeleeAnimationHelpersEditor)