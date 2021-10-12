// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Pistol.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYFPSPACK425_API APistol : public AWeaponBase
{
	GENERATED_BODY()
	
public:

	APistol();

	virtual void AttachToOwnerArms() override;

	//===============================================================
	//Components

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* SliderMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* IronSightMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* BulletMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* HammerComponent;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* HammerMesh;

	//===============================================================
	//Variables

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName HammerCompAttachName;

};
