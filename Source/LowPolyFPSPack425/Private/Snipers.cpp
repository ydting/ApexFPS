// Fill out your copyright notice in the Description page of Project Settings.


#include "Snipers.h"
#include "ApexCharacterBase.h"
#include "Components/SceneCaptureComponent2D.h"


ASnipers::ASnipers()
{
	SliderMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("SliderMesh"));
	SliderMesh->SetupAttachment(SliderComponent);
	SliderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//===============================================================
	
	IronSightMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("IronSightMesh"));
	IronSightMesh->SetupAttachment(ScopeComponent);
	IronSightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//===============================================================


	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BulletMesh"));
	BulletMesh->SetupAttachment(MagazineComponent);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//===============================================================
	
	Lid_Rear_Components = CreateDefaultSubobject<USceneComponent>(FName("Lid_Rear_Components"));
	Lid_Rear_Components->SetupAttachment(GetRootComponent());

	Lid_Rear_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Lid_Rear_Mesh"));
	Lid_Rear_Mesh->SetupAttachment(Lid_Rear_Components);
	Lid_Rear_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Lid_Front_Components = CreateDefaultSubobject<USceneComponent>(FName("Lid_Front_Components"));
	Lid_Front_Components->SetupAttachment(GetRootComponent());

	Lid_Front_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Lid_Front_Mesh"));
	Lid_Front_Mesh->SetupAttachment(Lid_Front_Components);
	Lid_Front_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//===============================================================
	
	Bipod_Left_Components = CreateDefaultSubobject<USceneComponent>(FName("Bipod_Left_Components"));
	Bipod_Left_Components->SetupAttachment(GetRootComponent());

	Bipod_Left_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Bipod_Left_Mesh"));
	Bipod_Left_Mesh->SetupAttachment(Bipod_Left_Components);
	Bipod_Left_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Bipod_Right_Components = CreateDefaultSubobject<USceneComponent>(FName("Bipod_Right_Components"));
	Bipod_Right_Components->SetupAttachment(GetRootComponent());

	Bipod_Right_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Bipod_Right_Mesh"));
	Bipod_Right_Mesh->SetupAttachment(Bipod_Right_Components);
	Bipod_Right_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//===============================================================
	
	Barrel_Components = CreateDefaultSubobject<USceneComponent>(FName("Barrel_Components"));
	Barrel_Components->SetupAttachment(GetRootComponent());

	Barrel_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Barrel_Mesh"));
	Barrel_Mesh->SetupAttachment(Barrel_Components);
	Barrel_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Handle_Components = CreateDefaultSubobject<USceneComponent>(FName("Handle_Components"));
	Handle_Components->SetupAttachment(GetRootComponent());

	Handle_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Handle_Mesh"));
	Handle_Mesh->SetupAttachment(Handle_Components);
	Handle_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	SniperScope = CreateDefaultSubobject<USceneCaptureComponent2D>(FName(TEXT("SniperScope")));
	SniperScope->SetupAttachment(WeaponComponent);
	
	//===========================
	
}

void ASnipers::AttachToOwnerArms()
{
	Super::AttachToOwnerArms();

	if (GetOwner())
	{
		AApexCharacterBase* CharacterOwner = Cast<AApexCharacterBase>(GetOwner());
		if (CharacterOwner)
		{
			Lid_Rear_Components->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Lid_Rear_CompAttachName);

			Lid_Front_Components->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Lid_Front_CompAttachName);

			Bipod_Left_Components->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, BipodLeftCompAttachName);

			Bipod_Right_Components->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, BipodRightCompAttachName);

			Barrel_Components->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, BarrelCompAttachName);

			Handle_Components->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandleCompAttachName);
		}
	}
}
