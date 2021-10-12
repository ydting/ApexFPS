// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Snipers.generated.h"

/**
 * 
 */
UCLASS()
class LOWPOLYFPSPACK425_API ASnipers : public AWeaponBase
{
	GENERATED_BODY()

public:

	ASnipers();

	virtual void AttachToOwnerArms() override;

	//===============================================================
	//Components

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* SliderMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* IronSightMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* BulletMesh;

	//===========================

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Lid_Rear_Components;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Lid_Rear_Mesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Lid_Front_Components;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Lid_Front_Mesh;

	//===========================

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Bipod_Left_Components;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Bipod_Left_Mesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Bipod_Right_Components;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Bipod_Right_Mesh;

	//===========================
	
	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Barrel_Components;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Barrel_Mesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Handle_Components;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Handle_Mesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
		USceneCaptureComponent2D* SniperScope;
	
	//===============================================================
	//Sockets

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName Lid_Rear_CompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName Lid_Front_CompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName BipodLeftCompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName BipodRightCompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName BarrelCompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName HandleCompAttachName;

};
