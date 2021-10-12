// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Pickupable.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateEquipmentSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YDTPLUGINS_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquipmentComponent();

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool OnPickupGear(FItemInfo& NewGearItemInfo);

	/**
	 * @brief Is Body Shield Cracked
	 * @return Shield Cracked Or Not Valid will return true
	 */
	UFUNCTION(BlueprintCallable)
	bool IsBodyShieldCracked() const { return !FItemInfo::IsValid(BodyShieldSlot) || BodyShieldSlot.ItemNum <= 0; }

	void OnModifyShieldValue(const float Amount);
	
	void OnShieldTakingDamage(int32 DamageAmount,AController* EventInstigator, AActor* DamageCauser,const UDamageType* DamageType);
	
	UPROPERTY(Category="Gear",BlueprintReadOnly,EditDefaultsOnly,ReplicatedUsing=UpdateEquipment)
		FItemInfo HelmetSlot;

	UPROPERTY(Category="Gear",BlueprintReadOnly,EditDefaultsOnly,ReplicatedUsing=UpdateEquipment)
		FItemInfo BodyShieldSlot;

	UPROPERTY(Category="Gear",BlueprintReadOnly,EditDefaultsOnly,ReplicatedUsing=UpdateEquipment)
		FItemInfo BackpackSlot;

	UFUNCTION()
	void UpdateEquipment();
	
	UPROPERTY(BlueprintAssignable)
	FOnUpdateEquipmentSignature OnUpdateEquipment;
	
};
