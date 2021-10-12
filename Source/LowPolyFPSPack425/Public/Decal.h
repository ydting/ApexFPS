// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Decal.generated.h"

UCLASS()
class LOWPOLYFPSPACK425_API ADecal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADecal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Decal")
	void SetupDecalComp(class UMaterialInterface* DecalMaterial, float Lifespan,float DecalSize);

	UPROPERTY(Category = "Decal", VisibleAnywhere, BlueprintReadWrite)
		class UDecalComponent* DecalComp;

};
