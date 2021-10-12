// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletCasing.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABulletCasing::ABulletCasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("CasingMesh"));
	CasingMesh->SetupAttachment(GetRootComponent());

	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetCollisionProfileName(FName("Ragdoll"));
	CasingMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	RotatingSpeed = 2500.f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABulletCasing::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);
	
	SetLifeSpan(6.0f);

	FTimerHandle TempHandle;
	GetWorldTimerManager().SetTimer(TempHandle, this, &ABulletCasing::PlaySFX, FMath::FRandRange(0.3f, 0.6f), false);
}

// Called every frame
void ABulletCasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		CasingMesh->AddLocalRotation(FRotator(DeltaTime*RotatingSpeed, DeltaTime*RotatingSpeed, 0));
	}
}

void ABulletCasing::PlaySFX()
{
	UGameplayStatics::PlaySound2D(this, CasingSound);
}

