// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotguns.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "ApexCharacterBase.h"

AShotguns::AShotguns()
{
	
	SliderMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("SliderMesh"));
	SliderMesh->SetupAttachment(SliderComponent);
	SliderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//==============================================================
	
	IronSightMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("IronSightMesh"));
	IronSightMesh->SetupAttachment(ScopeComponent);
	IronSightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//===============================================================
	
	FlapComponent = CreateDefaultSubobject<USceneComponent>(FName("FlapComponent"));
	FlapComponent->SetupAttachment(GetRootComponent());

	FlapMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("FlapMesh"));
	FlapMesh->SetupAttachment(FlapComponent);
	FlapMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//===============================================================
	
	ShellCasingComponent = CreateDefaultSubobject<USceneComponent>(FName("ShellCasingComponent"));
	ShellCasingComponent->SetupAttachment(GetRootComponent());

	ShellCasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ShellCasingMesh"));
	ShellCasingMesh->SetupAttachment(ShellCasingComponent);
	ShellCasingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//===============================================================
	
	BulletSpawnPerShot = 8;

}

void AShotguns::AttachToOwnerArms()
{
	Super::AttachToOwnerArms();

	if (GetOwner())
	{
		AApexCharacterBase* CharacterOwner = Cast<AApexCharacterBase>(GetOwner());
		if (CharacterOwner)
		{
			FlapComponent->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FlapCompAttachName);

			ShellCasingComponent->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ShellCasingCompAttachName);
		}
	}
}

void AShotguns::Fire()
{
	if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
	{
		if (WeaponAnimMontages.Fire && !bIsOutOfAmmo)
		{
			MyOwner->PlayFPSAnimMontage(WeaponAnimMontages.Fire);

			for (int32 i = 0; i < BulletSpawnPerShot; i++)
			{
				SpawnBullet(true);
			}

			SpawningCasing();

			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(RecoilCameraShake);
			
			LastFireTime = AActor::GetGameTimeSinceCreation();

			bIsOutOfAmmo = (--CurrentMagazineAmmo) == 0;
		}
	}
}

