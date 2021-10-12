// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "YDTPlugins/Public/Interactable.h"
#include "PickupableWeapon.generated.h"

UCLASS()
class LOWPOLYFPSPACK425_API APickupableWeapon : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupableWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category="Pickupable",VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* BoxCollision;
	
	UPROPERTY(Category="Pickupable",EditDefaultsOnly,BlueprintReadOnly)
		TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(Category="Pickupable",EditDefaultsOnly,BlueprintReadOnly)
		int32 RemainingAmmoInWeapon;

	UPROPERTY(Category="Pickupable",EditDefaultsOnly,BlueprintReadOnly)
		FText WeaponDescription;
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Interact")
	void OnInteract(AActor* InteractBy);
	virtual void OnInteract_Implementation(AActor* InteractBy) override;
};
