// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomaticGuns.h"
#include "ApexCharacterBase.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

AAutomaticGuns::AAutomaticGuns()
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
	
}

