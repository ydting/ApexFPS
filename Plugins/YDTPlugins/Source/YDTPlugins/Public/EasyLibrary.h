// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EasyLibrary.generated.h"

/**
 * 
 */
UCLASS()
class YDTPLUGINS_API UEasyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static FHitResult SimpleCapsuleTrace(UObject* WorldContextObject,class USceneComponent* TraceFrom, const float TraceDistance
													, const float TraceRadius , TArray<AActor*> IgnoredActors, bool bUseDebug = false
													, ECollisionChannel TraceChannel = ECC_Visibility);

	static FHitResult SimpleLineTrace(UObject* WorldContextObject,class USceneComponent* TraceFrom, const float TraceDistance
										,bool bUseDebug = false,ECollisionChannel TraceChannel = ECC_Visibility);

	static FHitResult SimpleLineTrace(UObject* WorldContextObject,class USceneComponent* TraceFrom, const float TraceDistance,TArray<AActor*> IgnoredActors
										,bool bUseDebug = false,ECollisionChannel TraceChannel = ECC_Visibility);
};
