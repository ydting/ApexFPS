// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Headers.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupAmmoSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupAttachmentSignature,const FItemInfo&,AttachhmentInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInventoryWidgetSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupRegenSignature,const int32,PickupedHealingItemId);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YDTPLUGINS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	
	/**
	 * @brief If Inventory Full,cant pick up any item
	 */
	UFUNCTION(BlueprintCallable)
		bool IsInventoryFull() const ;

	
	/**
	 * @brief Get same ItemId ItemIndex
	 * @param ItemId the ItemId to get
	 * @return return ItemIndex in inventoryArray,return -1 if not exists
	 */
	UFUNCTION(BlueprintCallable)
	int32 GetItemIndexByItemId(const int32 ItemId) const;

	UFUNCTION(BlueprintCallable)
	int32 GetSameItemTotalNum(const int32 ItemId) const;

	bool RemoveItemNumsBySameItemId(int32 RemoveCount,const int32 ItemId);

	UFUNCTION()
	void OnRep_InventoryItem();
	
	/**
	 * @brief 
	 * @param PickedItemInfo The Pickable item info
	 * @return Should Destroy The Instance of pickable
	 */
	UFUNCTION(BlueprintCallable)
	bool OnPickupItem(UPARAM(ref) FItemInfo& PickedItemInfo);
	
	UFUNCTION(BlueprintCallable)
		bool AddItemToInventory(UPARAM(ref)FItemInfo& AddedItemInfo);

	UFUNCTION(BlueprintCallable)
		bool AddItemToNewSlot(UPARAM(ref)FItemInfo& ItemInfo);

	/**
	 * @brief 
	 * @param SlotIndex Slot Index to drop
	 * @param IsDropAll drop all if true,false drop a stack,less than a stack drop all
	 */
	UFUNCTION(BlueprintCallable)
		void DropItem(const int32 SlotIndex,const bool IsDropAll);

	UFUNCTION(BlueprintCallable)
	int32 GetInventoryTotalSlotNum() const { return DefaultInventorySlotNum + AddOnSlotNum;}

	UFUNCTION(BlueprintCallable)
		int32 FindEmptySlot() const;

	UFUNCTION(BlueprintCallable)
		void ClearSlot(const int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void ExtendInventorySlot(const int32 TotalAddOnSlots);
	
	UFUNCTION(Client,Unreliable)
	void UpdateWidgetOnPicked(const FItemInfo& PickedItemInfo);
	
	UPROPERTY(Category="Events",BlueprintAssignable)
	FOnPickupAmmoSignature OnPickupAmmo;

	UPROPERTY(Category="Events",BlueprintAssignable)
	FOnPickupRegenSignature OnPickupRegenItem;

	FOnPickupAttachmentSignature OnPickupAttachment;
	
	UPROPERTY(Category="Events",BlueprintAssignable)
	FOnUpdateInventoryWidgetSignature OnUpdateInventoryWidget;
	
protected:
	
	UPROPERTY(Category="Inventory",EditDefaultsOnly,BlueprintReadOnly,ReplicatedUsing=OnRep_InventoryItem)
		TArray<FItemInfo> InventoryItemsArray;
	
	/**
	 * @brief how many slot having when no equipped backpack
	 */
	UPROPERTY(Category="Inventory",EditDefaultsOnly,BlueprintReadOnly)
		int32 DefaultInventorySlotNum;

	FItemInfo LatestPickedItemInfo;

	UPROPERTY(Replicated)
	int32 AddOnSlotNum;
	
	UPROPERTY(Replicated)
	bool bInventoryFull;

	int32 EmptySlotIndex;
};
