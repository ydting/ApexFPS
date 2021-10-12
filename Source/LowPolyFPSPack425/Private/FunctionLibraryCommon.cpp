// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraryCommon.h"

#include "Engine/Engine.h"

void UFunctionLibraryCommon::DebugMessage(EDebugType DebugType, const FString& Message, float Duration /*= 1.0f*/)
{
	FColor DebugColor = FColor::Red;

	switch (DebugType)
	{
	case EDebugType::Warning:
		DebugColor = FColor::Yellow;
		break;
	case EDebugType::Error:
		DebugColor = FColor::Red;
		break;
	case EDebugType::Debug:
		DebugColor = FColor::Cyan;
		break;
	default:
		break;
	}

	GEngine->AddOnScreenDebugMessage(-1, Duration, DebugColor, Message);
}

void UFunctionLibraryCommon::ClampPlayerYawViewAngle(AController* PlayerController ,const float AvailableAngleAmount)
{
	if (APlayerController* PC = Cast<APlayerController>(PlayerController))
	{
		if (APlayerCameraManager* CamManager = PC->PlayerCameraManager)
		{
			const float Average = AvailableAngleAmount / 2;
			const float NewMin = PC->GetControlRotation().Yaw - Average;
			const float NewMax = PC->GetControlRotation().Yaw + Average;
			CamManager->ViewYawMin = NewMin;
			CamManager->ViewYawMax = NewMax;
		}
	}
}

void UFunctionLibraryCommon::UnClampPlayerYawView(AController* PlayerController)
{
	if (APlayerController* PC = Cast<APlayerController>(PlayerController))
	{
		if (APlayerCameraManager* CamManager = PC->PlayerCameraManager)
		{
			CamManager->ViewYawMin = 0;
			CamManager->ViewYawMax = 359.9f;
		}
	}
}
