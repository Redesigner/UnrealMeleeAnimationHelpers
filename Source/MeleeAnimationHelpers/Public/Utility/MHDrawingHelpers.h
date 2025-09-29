#pragma once

class FPrimitiveDrawInterface;

class MHDrawingHelpers
{
public:
	static void DrawBox(FPrimitiveDrawInterface* PDI, const FVector& BoxDimensions,
		FLinearColor Color, const FMatrix& Transform);

	static void DrawSphereHitbox(FPrimitiveDrawInterface* PDI, float Radius,
		FLinearColor Color, const FMatrix& Transform);
	
	static void DrawCapsuleHitbox(FPrimitiveDrawInterface* PDI, float Radius, float HalfHeight,
		FLinearColor Color, const FMatrix& Transform);

	static void DrawHalfCircle(FPrimitiveDrawInterface* PDI, const FVector& Base, const FVector& X, const FVector& Y,
		const FLinearColor& Color, const float Radius, int32 NumSides);

	static void DrawCircle(FPrimitiveDrawInterface* PDI, const FVector& Base, const FVector& X, const FVector& Y,
		const FLinearColor& Color, const float Radius, int32 NumSides);
};
