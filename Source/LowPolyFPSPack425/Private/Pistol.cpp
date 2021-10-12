// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"
#include "ApexCharacterBase.h"

APistol::APistol()
{
	PrimaryActorTick.bCanEverTick = false;

	//===============================================================
	
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BulletMesh"));
	BulletMesh->SetupAttachment(MagazineComponent);

	SliderMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Slider_Mesh"));
	SliderMesh->SetupAttachment(SliderComponent);
	
	ScopeMesh_02->SetupAttachment(SliderComponent);
	ScopeMesh_03->SetupAttachment(SliderComponent);
	ScopeMesh_04->SetupAttachment(SliderComponent);
	ScopeMesh_4x->SetupAttachment(SliderComponent);
	
	//===============================================================
	
	HammerComponent = CreateDefaultSubobject<USceneComponent>(FName("Hammer_Component"));
	HammerComponent->SetupAttachment(GetRootComponent());

	HammerMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("HammerMesh"));
	HammerMesh->SetupAttachment(HammerComponent);

}

void APistol::AttachToOwnerArms()
{
	Super::AttachToOwnerArms();

	if (GetOwner())
	{
		AApexCharacterBase* CharacterOwner = Cast<AApexCharacterBase>(GetOwner());
		if (CharacterOwner)
		{
			HammerComponent->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HammerCompAttachName);
		}
	}
}
