// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "AutomaticGuns.generated.h"

class USceneComponent;
class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class LOWPOLYFPSPACK425_API AAutomaticGuns : public AWeaponBase
{
	GENERATED_BODY()
	
public:

	AAutomaticGuns();

	//===============================================================
	//Components

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* SliderMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* IronSightMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* BulletMesh;

	//===============================================================
	//Variables

};
