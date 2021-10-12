// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USoundBase;
class UParticleSystem;
class UMaterialInterface;

UCLASS()
class LOWPOLYFPSPACK425_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor
						, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Client,Unreliable)
	void Client_OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor
						, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//===============================================================
	//Components

	UPROPERTY(Category = "ProjecileSettings", VisibleAnywhere)
	class UStaticMeshComponent* ProjectileMeshes;

	UPROPERTY(Category = "ProjecileSettings", VisibleAnywhere)
	class UBoxComponent* ProjectileBoxCollision;

	UPROPERTY(Category = "Component", VisibleAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComp;

	//===============================================================
	//Variable
	
	UPROPERTY(Category = "ProjecileSettings", EditDefaultsOnly, BlueprintReadWrite)
		float Damage;

	UPROPERTY(Category = "ProjecileSettings", EditDefaultsOnly)
		float Speed;
	
	UPROPERTY(Category = "ProjecileSettings", EditDefaultsOnly)
		class USoundBase* ImpactSound;

	UPROPERTY(Category = "ProjecileSettings", EditDefaultsOnly)
		float DecalSize;

	UPROPERTY(Category = "ProjecileSettings", EditDefaultsOnly)
		float RandomDeviation;

	UPROPERTY(Category = "ProjecileSettings", EditDefaultsOnly)
		float DecalLifeSpan;

	UPROPERTY(Category = "ProjecileSettings", EditDefaultsOnly)
		TSubclassOf<UDamageType> ProjectileDamageType;
	//===============================================================

	UPROPERTY(Category = "SFX", EditDefaultsOnly, BlueprintReadWrite)
		UMaterialInterface* MetalImpactDecal;

	UPROPERTY(Category = "SFX", EditDefaultsOnly, BlueprintReadWrite)
		UMaterialInterface* ConcreteImpactDecal;

	UPROPERTY(Category = "SFX", EditDefaultsOnly, BlueprintReadWrite)
		UMaterialInterface* DirtImpactDecal;

	UPROPERTY(Category = "SFX", EditDefaultsOnly, BlueprintReadWrite)
		UMaterialInterface* BloodImpactDecal;

	//===============================================================

	UPROPERTY(Category = "VFX", EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* MetalImpactParticle;

	UPROPERTY(Category = "VFX", EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* ConcreteImpactParticle;

	UPROPERTY(Category = "VFX", EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* DirtImpactParticle;

	UPROPERTY(Category = "VFX", EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* BloodImpactParticle;

	//===============================================================
	
};
