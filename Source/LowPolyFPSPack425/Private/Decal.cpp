// Fill out your copyright notice in the Description page of Project Settings.


#include "Decal.h"
#include "Components/DecalComponent.h"

// Sets default values
ADecal::ADecal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(FName("DecalComp"));
	DecalComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ADecal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADecal::SetupDecalComp(class UMaterialInterface* DecalMaterial, float Lifespan, float DecalSize)
{
	if (!DecalMaterial) return;
	if (!DecalComp) return;
	DecalComp->SetDecalMaterial(DecalMaterial);
	DecalComp->DecalSize = FVector(DecalSize);
	
}


