// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletCasing.generated.h"

class UStaticMeshComponent;

UCLASS()
class LOWPOLYFPSPACK425_API ABulletCasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletCasing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PlaySFX();

	UPROPERTY(Category = "Meshes", VisibleAnywhere)
		UStaticMeshComponent* CasingMesh;

	UPROPERTY(Category = "SoundSettings", EditDefaultsOnly)
		class USoundBase* CasingSound;

	UPROPERTY(Category = "RotationSettings", EditDefaultsOnly)
		float RotatingSpeed;
};
