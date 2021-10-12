// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "FunctionLibraryCommon.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "LowPolyFPSPack425/LowPolyFPSPack425.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Components/BoxComponent.h"
#include "Decal.h"
#include "GameFramework/Character.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileBoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("ProjectileCollision"));
	ProjectileBoxCollision->SetupAttachment(RootComponent);
	ProjectileBoxCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnComponentHit);
	ProjectileBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileBoxCollision->bReturnMaterialOnMove = true;
	ProjectileBoxCollision->bTraceComplexOnMove = true;
	ProjectileBoxCollision->SetBoxExtent(FVector(1.1f, 0.5f, 0.5f));

	ProjectileMeshes = CreateDefaultSubobject<UStaticMeshComponent>(FName("ProjectileMesh"));
	ProjectileMeshes->SetupAttachment(ProjectileBoxCollision);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovementComp->InitialSpeed = Speed;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor
								 , UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

	UMaterialInterface* ImpactDecal = BloodImpactDecal;
	UParticleSystem* ImpactParticle = BloodImpactParticle;
	float DamageMultiplier = 1.0f;

	switch (HitSurfaceType)
	{
	case METAL_SURFACE:
		ImpactDecal = MetalImpactDecal;
		ImpactParticle = MetalImpactParticle;
		break;
	case CONCRETE_SURFACE:
		ImpactDecal = ConcreteImpactDecal;
		ImpactParticle = ConcreteImpactParticle;
		break;
	case DIRT_SURFACE:
		ImpactDecal = DirtImpactDecal;
		ImpactParticle = DirtImpactParticle;
		break;
	case HEAD_SURFACE:
		DamageMultiplier = 1.25f;
		break;
	case BODY_SURFACE:
		break;
	case LEGS_SURFACE:
		DamageMultiplier = 0.8f;
		break;
	}

	//Effect

	FRotator ImpactNormalRot = FRotationMatrix::MakeFromX(Hit.Normal).Rotator();
	UGameplayStatics::SpawnEmitterAtLocation(this,ImpactParticle,Hit.Location,ImpactNormalRot);

	ImpactNormalRot.Roll = FMath::FRandRange(0.0f, 360.0f);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	
	ADecal* TempDecalActor = GetWorld()->SpawnActor<ADecal>(ADecal::StaticClass(), Hit.Location, ImpactNormalRot);
	TempDecalActor->SetupDecalComp(ImpactDecal, DecalLifeSpan, DecalSize + FMath::FRandRange(-RandomDeviation, RandomDeviation));

	if (GetLocalRole() == ROLE_Authority)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			if (!Character->IsLocallyControlled())
			{
				Client_OnComponentHit(HitComponent,OtherActor,OtherComp,NormalImpulse,Hit);
			}
		}
		
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.Location);
		
		//Apply Damage
		UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage * DamageMultiplier, GetOwner()->GetInstigatorController(), GetOwner(), ProjectileDamageType);

		SetLifeSpan(3.0f);
		
	}

	ProjectileMeshes->SetHiddenInGame(true);
	ProjectileBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void AProjectile::Client_OnComponentHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	OnComponentHit(HitComponent,OtherActor,OtherComp,NormalImpulse,Hit);
}



