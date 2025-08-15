#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f);

		DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter);
		DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter);
		DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter);
		DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter);
		DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter);
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter)
{
	if (Texture)
	{
		const float TextureWidth = Texture->GetSizeX();
		const float TextureHeight = Texture->GetSizeY();
		const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth * 0.5f), ViewportCenter.Y - (TextureHeight * 0.5f));

		DrawTexture(
			Texture,
			TextureDrawPoint.X,
			TextureDrawPoint.Y,
			TextureWidth,
			TextureHeight,
			0.f,
			0.f,
			1.f,
			1.f,
			FLinearColor::White
		);
	}
}
