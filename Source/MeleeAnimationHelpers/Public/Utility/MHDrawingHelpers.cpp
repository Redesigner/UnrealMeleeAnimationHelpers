#include "MHDrawingHelpers.h"

void MHDrawingHelpers::DrawBox(FPrimitiveDrawInterface* PDI, const FVector& BoxDimensions,
                               const FLinearColor Color, const FMatrix& Transform)
{
	const FVector Brt = Transform.TransformPosition(FVector(BoxDimensions.X, BoxDimensions.Y, BoxDimensions.Z));
	const FVector Brd = Transform.TransformPosition(FVector(BoxDimensions.X, BoxDimensions.Y, -BoxDimensions.Z));
	const FVector Blt = Transform.TransformPosition(FVector(-BoxDimensions.X, BoxDimensions.Y, BoxDimensions.Z));
	const FVector Bld = Transform.TransformPosition(FVector(-BoxDimensions.X, BoxDimensions.Y, -BoxDimensions.Z));
	const FVector Frt = Transform.TransformPosition(FVector(BoxDimensions.X, -BoxDimensions.Y, BoxDimensions.Z));
	const FVector Frd = Transform.TransformPosition(FVector(BoxDimensions.X, -BoxDimensions.Y, -BoxDimensions.Z));
	const FVector Flt = Transform.TransformPosition(FVector(-BoxDimensions.X, -BoxDimensions.Y, BoxDimensions.Z));
	const FVector Fld = Transform.TransformPosition(FVector(-BoxDimensions.X, -BoxDimensions.Y, -BoxDimensions.Z));
	// Vertical Lines
	PDI->DrawLine(Brt, Brd, Color, SDPG_World);
	PDI->DrawLine(Blt, Bld, Color, SDPG_World);
	PDI->DrawLine(Frt, Frd, Color, SDPG_World);
	PDI->DrawLine(Flt, Fld, Color, SDPG_World);
	// Horizontal Lines
	PDI->DrawLine(Brt, Blt, Color, SDPG_World);
	PDI->DrawLine(Brd, Bld, Color, SDPG_World);
	PDI->DrawLine(Frt, Flt, Color, SDPG_World);
	PDI->DrawLine(Frd, Fld, Color, SDPG_World);
	// Forward Lines
	PDI->DrawLine(Brt, Frt, Color, SDPG_World);
	PDI->DrawLine(Blt, Flt, Color, SDPG_World);
	PDI->DrawLine(Brd, Frd, Color, SDPG_World);
	PDI->DrawLine(Bld, Fld, Color, SDPG_World);
}

void MHDrawingHelpers::DrawSphereHitbox(FPrimitiveDrawInterface* PDI, const float Radius, const FLinearColor Color,
	const FMatrix& Transform)
{
	const FVector Origin = Transform.GetOrigin();
	constexpr uint8 Segments = 16;

	constexpr float AngleInc = 2.f * UE_PI / Segments;
	int32 NumSegmentsY = 16;
	float Latitude = AngleInc;
	float SinY1 = 0.0f, CosY1 = 1.0f;

	while (NumSegmentsY--)
	{
		const float SinY2 = FMath::Sin(Latitude);
		const float CosY2 = FMath::Cos(Latitude);

		FVector Vertex1 = FVector(SinY1, 0.0f, CosY1) * Radius + Origin;
		FVector Vertex3 = FVector(SinY2, 0.0f, CosY2) * Radius + Origin;
		float Longitude = AngleInc;

		int32 NumSegmentsX = Segments;
		while (NumSegmentsX--)
		{
			const float SinX = FMath::Sin(Longitude);
			const float CosX = FMath::Cos(Longitude);

			const FVector Vertex2 = FVector((CosX * SinY1), (SinX * SinY1), CosY1) * Radius + Origin;
			const FVector Vertex4 = FVector((CosX * SinY2), (SinX * SinY2), CosY2) * Radius + Origin;

			PDI->DrawLine(Vertex1, Vertex2, Color, SDPG_World);
			PDI->DrawLine(Vertex1, Vertex3, Color, SDPG_World);

			Vertex1 = Vertex2;
			Vertex3 = Vertex4;
			Longitude += AngleInc;
		}
		SinY1 = SinY2;
		CosY1 = CosY2;
		Latitude += AngleInc;
	}
}

void MHDrawingHelpers::DrawCapsuleHitbox(FPrimitiveDrawInterface* PDI, const float Radius,
	const float HalfHeight, const FLinearColor Color, const FMatrix& Transform)
{
	constexpr int32 DrawCollisionSides = 16;
	const FVector Origin = Transform.GetOrigin();
	FVector XAxis, YAxis, ZAxis;
	Transform.GetUnitAxes(XAxis, YAxis, ZAxis);

	// Draw top and bottom circles
	const float HalfAxis = FMath::Max<float>(HalfHeight - Radius, 1.f);
	const FVector TopEnd = Origin + HalfAxis*ZAxis;
	const FVector BottomEnd = Origin - HalfAxis*ZAxis;

	DrawCircle(PDI, TopEnd, XAxis, YAxis, Color, Radius, DrawCollisionSides);
	DrawCircle(PDI, BottomEnd, XAxis, YAxis, Color, Radius, DrawCollisionSides);

	// Draw domed caps
	DrawHalfCircle(PDI, TopEnd, YAxis, ZAxis, Color, Radius, DrawCollisionSides);
	DrawHalfCircle(PDI, TopEnd, XAxis, ZAxis, Color, Radius, DrawCollisionSides);

	const FVector NegZAxis = -ZAxis;

	DrawHalfCircle(PDI, BottomEnd, YAxis, NegZAxis, Color, Radius, DrawCollisionSides);
	DrawHalfCircle(PDI, BottomEnd, XAxis, NegZAxis, Color, Radius, DrawCollisionSides);

	// Draw connected lines
	PDI->DrawLine(TopEnd + Radius*XAxis, BottomEnd + Radius*XAxis, Color, SDPG_World);
	PDI->DrawLine(TopEnd - Radius*XAxis, BottomEnd - Radius*XAxis, Color, SDPG_World);
	PDI->DrawLine(TopEnd + Radius*YAxis, BottomEnd + Radius*YAxis, Color, SDPG_World);
	PDI->DrawLine(TopEnd - Radius*YAxis, BottomEnd - Radius*YAxis, Color, SDPG_World);
}

void MHDrawingHelpers::DrawHalfCircle(FPrimitiveDrawInterface* PDI, const FVector& Base, const FVector& X,
	const FVector& Y, const FLinearColor& Color, const float Radius, int32 NumSides)
{
	NumSides = FMath::Max(NumSides, 2);
	const float AngleDelta = 2.0f * UE_PI / NumSides;
	FVector	LastVertex = Base + X * Radius;

	for( int32 SideIndex = 0; SideIndex < (NumSides/2); SideIndex++)
	{
		FVector	Vertex = Base + (X * FMath::Cos(AngleDelta * (SideIndex + 1)) + Y * FMath::Sin(AngleDelta * (SideIndex + 1))) * Radius;
		PDI->DrawLine(LastVertex, Vertex, Color, ESceneDepthPriorityGroup::SDPG_World);
		LastVertex = Vertex;
	}
}

void MHDrawingHelpers::DrawCircle(FPrimitiveDrawInterface* PDI, const FVector& Base, const FVector& X, const FVector& Y,
	const FLinearColor& Color, const float Radius, int32 NumSides)
{
	NumSides = FMath::Max(NumSides, 2);
	const float	AngleDelta = 2.0f * UE_PI / NumSides;
	FVector	LastVertex = Base + X * Radius;

	for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
	{
		const FVector Vertex = Base + (X * FMath::Cos(AngleDelta * (SideIndex + 1)) + Y * FMath::Sin(AngleDelta * (SideIndex + 1))) * Radius;
		PDI->DrawLine(LastVertex, Vertex, Color, SDPG_World);
		LastVertex = Vertex;
	}
}
