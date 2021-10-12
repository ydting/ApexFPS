// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Interactable.h"
#include "GameFramework/Actor.h"

#include "Headers.h"
#include "Pickupable.generated.h"



class UStaticMeshComponent;

UCLASS()
class YDTPLUGINS_API APickupable : public AActor , public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		FItemInfo GetItemInfo() const { return ItemInfo;}

	UFUNCTION(BlueprintCallable)
		ERequiredSlot GetItemRequiredSlot() const { return ItemInfo.RequiredSlot;}
	
	UFUNCTION(BlueprintCallable)
		void SetupItemInfo(FItemInfo& NewItemInfo);

	/**
	 * @brief 
	 * @param AddingValue the value to modify,can be negative or positive
	 * @return New Current value
	 */
	UFUNCTION(BlueprintCallable)
		int32 ModifyItemNum(int32 AddingValue);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Interact")
	void OnInteract(AActor* InteractBy);
	virtual void OnInteract_Implementation(AActor* InteractBy) override;

	UFUNCTION(Server,Unreliable)
	void Server_Destroy();
protected:

	UPROPERTY(Category="PickableItem",VisibleAnywhere,BlueprintReadOnly)
		class UBoxComponent* ItemBoxCollision;
	
	UPROPERTY(Category="PickableItem",EditDefaultsOnly)
		FItemInfo ItemInfo;

	UPROPERTY(Category="PickableItem",EditDefaultsOnly)
		UDataTable* ItemsDataTable;
	
	UPROPERTY(Category="PickableItem",EditAnywhere,BlueprintReadWrite)
		bool bSetupInfoFromDataTable;

	UPROPERTY(Category="PickableItem",EditDefaultsOnly)
		int32 ItemId;
};
