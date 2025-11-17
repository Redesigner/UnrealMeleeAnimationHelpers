// Copyright (c) 2025 sleepysunset

#include "Hitboxes/HitboxComponentDetails.h"

TSharedRef<IPropertyTypeCustomization> FHitboxComponentDetails::MakeInstance()
{
	return MakeShareable( new FHitboxComponentDetails() );
}

void FHitboxComponentDetails::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FHitboxComponentDetails::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}
