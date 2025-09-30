#pragma once

class FPrimitiveDrawInterface;

class MHDrawingHelpers
{
public:
	/*
	 * A lot of this code, particularly the logic for drawing the caps and spheres
	 * is adapted from DebugDraw. I just cleaned it up a little!
	 */

	/**
	 * Draw a Box with lines
	 * @param PDI Primitive Draw Interface to draw into
	 * @param BoxDimensions Box half extents
	 * @param Color Shape Color
	 * @param Transform Shape transformation Matrix
	 */
	static void DrawBox(FPrimitiveDrawInterface* PDI, const FVector& BoxDimensions,
	                    FLinearColor Color, const FMatrix& Transform);
	/**
	 * Draw a Sphere with lines
	 * @param PDI Primitive Draw Interface to draw into
	 * @param Radius Sphere radius
	 * @param Color Shape Color
	 * @param Transform Shape transformation Matrix
	 */
	static void DrawSphereHitbox(FPrimitiveDrawInterface* PDI, float Radius,
		FLinearColor Color, const FMatrix& Transform);

	/**
	 * Draw a Capsule with lines
	 * @param PDI Primitive Draw Interface to draw into
	 * @param Radius Cap radius
	 * @param HalfHeight Capsule half height
	 * @param Color Shape Color
	 * @param Transform Shape transformation Matrix
	 */
	static void DrawCapsuleHitbox(FPrimitiveDrawInterface* PDI, float Radius, float HalfHeight,
		FLinearColor Color, const FMatrix& Transform);

	static void DrawHalfCircle(FPrimitiveDrawInterface* PDI, const FVector& Base, const FVector& X, const FVector& Y,
		const FLinearColor& Color, const float Radius, int32 NumSides);

	static void DrawCircle(FPrimitiveDrawInterface* PDI, const FVector& Base, const FVector& X, const FVector& Y,
		const FLinearColor& Color, const float Radius, int32 NumSides);
};
