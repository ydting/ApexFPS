// Fill out your copyright notice in the Description page of Project Settings.


#include "EasyLibrary.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

FHitResult UEasyLibrary::SimpleCapsuleTrace(UObject* WorldContextObject, USceneComponent* TraceFrom,
                                            const float TraceDistance, const float TraceRadius, TArray<AActor*> IgnoredActors,bool bUseDebug, ECollisionChannel TraceChannel)
{
	const FVector StartPos = TraceFrom->GetComponentLocation();
	const FVector Direction = TraceFrom->GetForwardVector() * TraceDistance;
	FVector EndPos = StartPos + Direction;

	const FRotator RotFromStartToEnd = FRotationMatrix::MakeFromX(EndPos - StartPos).Rotator();
	
	FHitResult Hit;

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);

	FCollisionQueryParams CollisionQuery;
	CollisionQuery.AddIgnoredActors(IgnoredActors);

	FCollisionShape Shape;
	Shape.SetCapsule(TraceRadius,0.0f);
	
	if (World->SweepSingleByChannel(Hit,StartPos,EndPos,RotFromStartToEnd.Quaternion(),TraceChannel,Shape,CollisionQuery))
	{
		EndPos = Hit.Location;
	}
	
	if (bUseDebug)
	{
		const FColor DebugColor = Hit.bBlockingHit ? FColor::Green : FColor::Red;

		DrawDebugLine(World,StartPos,EndPos,DebugColor,false,1.0f);
	}
	
	return Hit;
}

FHitResult UEasyLibrary::SimpleLineTrace(UObject* WorldContextObject, USceneComponent* TraceFrom,
                                         const float TraceDistance,bool bUseDebug,ECollisionChannel TraceChannel)
{
	const TArray<AActor*> TempArray;
	return SimpleLineTrace(WorldContextObject,TraceFrom,TraceDistance,TempArray,bUseDebug,TraceChannel);
}

FHitResult UEasyLibrary::SimpleLineTrace(UObject* WorldContextObject, USceneComponent* TraceFrom,
											const float TraceDistance, TArray<AActor*> IgnoredActors, bool bUseDebug, ECollisionChannel TraceChannel)
{
	const FVector StartPos = TraceFrom->GetComponentLocation();
	const FVector Direction = TraceFrom->GetForwardVector() * TraceDistance;
	FVector EndPos = StartPos + Direction;

	FHitResult Hit;

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);

	FCollisionQueryParams CollisionQuery;
	CollisionQuery.AddIgnoredActors(IgnoredActors);
	
	if (World->LineTraceSingleByChannel(Hit, StartPos, EndPos,TraceChannel,CollisionQuery))
	{
		EndPos = Hit.Location;
	}
	
	
	if (bUseDebug)
	{
		const FColor DebugColor = Hit.bBlockingHit ? FColor::Green : FColor::Red;

		DrawDebugLine(World,StartPos,EndPos,DebugColor,false,1.0f);
	}
	
	return Hit;
}
