// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Shotguns.generated.h"

class USceneComponent;
class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class LOWPOLYFPSPACK425_API AShotguns : public AWeaponBase
{
	GENERATED_BODY()


public:

	AShotguns();

	virtual void AttachToOwnerArms() override;

	virtual void Fire() override;

	//===============================================================
	//Components

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* SliderMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* IronSightMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* FlapComponent;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* FlapMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* ShellCasingComponent;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* ShellCasingMesh;

	//===============================================================
	//Variables
	

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName FlapCompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName ShellCasingCompAttachName;

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		int32 BulletSpawnPerShot;
};
