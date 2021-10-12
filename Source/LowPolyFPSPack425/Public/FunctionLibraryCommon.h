// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibraryCommon.generated.h"

UENUM(BlueprintType)
enum class EDebugType : uint8
{
	Warning,
	Error,
	Debug
};


/**
 * 
 */
UCLASS()
class LOWPOLYFPSPACK425_API UFunctionLibraryCommon : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static void DebugMessage(EDebugType DebugType, const FString& Message, float Duration = 1.0f);

	UFUNCTION(BlueprintCallable)
		static void ClampPlayerYawViewAngle(AController* PlayerController ,const float AvailableAngleAmount);

	UFUNCTION(BlueprintCallable)
		static void UnClampPlayerYawView(class AController* PlayerController);
	
};
