// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"


#include "EasyLibrary.h"
#include "EquipmentComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	DefaultInventorySlotNum = 10;
	EmptySlotIndex = -1;
	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UInventoryComponent::IsInventoryFull() const
{
	const bool Cond_1 = FindEmptySlot() == -1;
	const bool Cond_2 = InventoryItemsArray.Num() >= GetInventoryTotalSlotNum() - 1;

	return Cond_1 && Cond_2;
}

int32 UInventoryComponent::GetItemIndexByItemId(const int32 ItemId) const
{
	int32 LocatedItemIndex = -1;
	
	for (int32 i = 0; i < InventoryItemsArray.Num(); ++i)
	{
		if (InventoryItemsArray[i].ItemId == ItemId)
		{
			if (InventoryItemsArray[i].bIsStackFull == false)
			{
				return i;
			}
			else
			{
				LocatedItemIndex = i;
			}
		}
	}

	return LocatedItemIndex;
}

int32 UInventoryComponent::GetSameItemTotalNum(const int32 ItemId) const
{
	int32 TotalNum = 0;
	
	for (const auto ItemSlot : InventoryItemsArray)
	{
		if (ItemSlot.ItemId == ItemId)
		{
			TotalNum += ItemSlot.ItemNum;
		}
	}

	return TotalNum;
}

bool UInventoryComponent::RemoveItemNumsBySameItemId(int32 RemoveCount,const int32 ItemId)
{
	for (int i = InventoryItemsArray.Num() - 1; i >= 0; --i)
	{
		if (RemoveCount >= 1)
		{
			if (InventoryItemsArray[i].ItemId == ItemId)
			{
				//if a slot having enough num to take away
				if (InventoryItemsArray[i].ItemNum >= RemoveCount)
				{
					if(InventoryItemsArray[i].ItemNum == RemoveCount)
					{
						ClearSlot(i);
					}
					else
					{
						InventoryItemsArray[i].ItemNum -= RemoveCount;
					}
					
					RemoveCount = 0;
				}
				//if not enough
				else
				{
					RemoveCount -= InventoryItemsArray[i].ItemNum;
					ClearSlot(i);
				}
			}
		}
		else	break;
	}

	return RemoveCount == 0;
}

void UInventoryComponent::OnRep_InventoryItem()
{
	UpdateWidgetOnPicked(LatestPickedItemInfo);
}

bool UInventoryComponent::OnPickupItem(FItemInfo& PickedItemInfo)
{
	//Detect What category it is
	switch (PickedItemInfo.RequiredSlot)
	{
		case ERequiredSlot::Helmet:
		case ERequiredSlot::BodyShield:
		case ERequiredSlot::Backpack:
		case ERequiredSlot::KnockdownShield:
		case ERequiredSlot::Weapon:


			if (UEquipmentComponent* EquipComp = Cast<UEquipmentComponent>(GetOwner()->GetComponentByClass(UEquipmentComponent::StaticClass())))
			{
				return EquipComp->OnPickupGear(PickedItemInfo);
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("WARNING!!!,Equipment Component Didnt found in ControlledCharacter class"));
			}
		
			break;
		case ERequiredSlot::Inventory:
			{
				if (PickedItemInfo.AttachmentType != EWeaponAttachmentType::None)
				{
					//Attachment category
					//if weapon have slot to attach , attach directly to last select weapon instead of adding to inventory
					OnPickupAttachment.Broadcast(PickedItemInfo);

					return true;
				}
				else
				{
					return AddItemToInventory(PickedItemInfo);
				}
			}
			break;
		default: ;
	}	
	

	//Gear Category ,get owner equipment component

	return false;
}

bool UInventoryComponent::AddItemToInventory(FItemInfo& AddedItemInfo)
{
	if (AddedItemInfo.bIsStackable)
	{
		const int32 ItemIndex = GetItemIndexByItemId(AddedItemInfo.ItemId);

		//Have same ItemId on a slot
		if (ItemIndex != -1)
		{
			//if its not stack full
			if(InventoryItemsArray[ItemIndex].bIsStackFull == false)
			{
				FItemInfo& Item = InventoryItemsArray[ItemIndex];
	
				const int32 AmountToMax = Item.ItemMaximumStackAmount - Item.ItemNum;

				//if the pick up num is more than amount to max,mean will stack max a slot
				if (AddedItemInfo.ItemNum >= AmountToMax + 1)
				{
					AddedItemInfo.ItemNum -= AmountToMax;

					Item.ItemNum += AmountToMax;
					Item.bIsStackFull = true;
				}
				//if the pick up num is less equal than amount to max,pick up and destroy the instance
				else if (AmountToMax >= AddedItemInfo.ItemNum)
				{
					if (AmountToMax == AddedItemInfo.ItemNum)
					{
						Item.bIsStackFull = true;
					}
					
					//Take all item num
					Item.ItemNum += AddedItemInfo.ItemNum;
					
					AddedItemInfo.ItemNum = 0;

					LatestPickedItemInfo = AddedItemInfo;
					OnRep_InventoryItem();
					
					return true;
				}
			}
		}
		
	}
	
	return AddItemToNewSlot(AddedItemInfo);
}

bool UInventoryComponent::AddItemToNewSlot(FItemInfo& ItemInfo)
{
	//Check Inventory Array have any not valid index?
	if (FindEmptySlot() != -1)
	{
		EmptySlotIndex = FindEmptySlot();
		
		InventoryItemsArray[EmptySlotIndex] = ItemInfo;

		EmptySlotIndex = -1;

		LatestPickedItemInfo = ItemInfo;
		OnRep_InventoryItem();
		
		return true;
	}

	//Check Is The Inventory Array num same as the inventory size
	if (InventoryItemsArray.Num() <= GetInventoryTotalSlotNum() - 1)
	{
		InventoryItemsArray.Add(ItemInfo);

		LatestPickedItemInfo = ItemInfo;
		OnRep_InventoryItem();
		
		return true;
	}

	return false;
}

void UInventoryComponent::DropItem(const int32 SlotIndex,const bool IsDropAll)
{
	if (InventoryItemsArray[SlotIndex].ItemId != 0)
	{
		if (!InventoryItemsArray[SlotIndex].PickupableClass)
		{
			UE_LOG(LogTemp,Warning,TEXT("Setup Pickupable class in Items Data Table"));

			return;
		}
		
		
		FItemInfo& DropItemInfo = InventoryItemsArray[SlotIndex];

		const AActor* MyOwner = GetOwner();

		const FVector ForwardOffset = MyOwner->GetActorForwardVector() *100.0f;
		const FVector SpawnLoc = MyOwner->GetActorLocation() + ForwardOffset;

		
		
		if (IsDropAll)
		{
			APickupable* SpawnedActor = GetWorld()->SpawnActor<APickupable>(DropItemInfo.PickupableClass,SpawnLoc,FRotator(0.f,0.f,0.f));
			SpawnedActor->SetupItemInfo(DropItemInfo);

			ClearSlot(SlotIndex);

			switch (DropItemInfo.ItemCategory)
			{
				case EItemCategory::Regen_Health:
				case EItemCategory::Regen_Shield:
				case EItemCategory::Regen_Combo:

				OnPickupRegenItem.Broadcast(-1);		
				
				break;
			}
		}
		else
		{
			const int32 DroppedAmount = DropItemInfo.ItemNum > DropItemInfo.PerStackAmount ? DropItemInfo.PerStackAmount : DropItemInfo.ItemNum;

			FItemInfo NewDropItemInfo = DropItemInfo;
			//make sure the pickupable instance info stackfull is false on drop stack
			NewDropItemInfo.bIsStackFull = false;
			if (DropItemInfo.ItemNum - DroppedAmount <= 0)
			{
				DropItemInfo.ItemNum = 0;

				ClearSlot(SlotIndex);

				switch (DropItemInfo.ItemCategory)
				{
				case EItemCategory::Regen_Health:
				case EItemCategory::Regen_Shield:
				case EItemCategory::Regen_Combo:

					OnPickupRegenItem.Broadcast(-1);		
			
					break;
				}
			}
			else
			{
				DropItemInfo.ItemNum -= DroppedAmount;

				InventoryItemsArray[SlotIndex].bIsStackFull = false;
			}
			
			NewDropItemInfo.ItemNum = DroppedAmount;
		
			APickupable* SpawnedActor = GetWorld()->SpawnActor<APickupable>(DropItemInfo.PickupableClass,SpawnLoc,FRotator(0.f,0.f,0.f));
			SpawnedActor->SetupItemInfo(NewDropItemInfo);
		}
	}
	
}

int32 UInventoryComponent::FindEmptySlot() const
{
	for(int32 i = 0; i <= InventoryItemsArray.Num() - 1; ++i)
	{
		if (!FItemInfo::IsValid(InventoryItemsArray[i]))
		{
			return i;
		}
	}

	return -1;
}

void UInventoryComponent::ClearSlot(const int32 SlotIndex)
{
	FItemInfo EmptyInfo = InventoryItemsArray[SlotIndex];
	EmptyInfo.ItemName = FName(TEXT("Name_NONE"));
	EmptyInfo.ItemId = 0;

	InventoryItemsArray[SlotIndex] = EmptyInfo;

	EmptySlotIndex = SlotIndex;

	OnUpdateInventoryWidget.Broadcast();
}

void UInventoryComponent::ExtendInventorySlot(const int32 TotalAddOnSlots)
{
	AddOnSlotNum = TotalAddOnSlots;
}

void UInventoryComponent::UpdateWidgetOnPicked_Implementation(const FItemInfo& PickedItemInfo)
{
	switch (PickedItemInfo.ItemCategory)
	{
		case EItemCategory::Ammo:
			OnPickupAmmo.Broadcast();
			break;
		case EItemCategory::Attachment:
			OnUpdateInventoryWidget.Broadcast();
			break;
			
		case EItemCategory::Gear:
			break;
		case EItemCategory::Gear_Defense:
			break;
		case EItemCategory::Regen_Health:
		case EItemCategory::Regen_Shield:
		case EItemCategory::Regen_Combo:

		OnPickupRegenItem.Broadcast(PickedItemInfo.ItemId);
		
		break;
		
		default: ;
	}
	
}

void UInventoryComponent::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryComponent,InventoryItemsArray);
	DOREPLIFETIME(UInventoryComponent,AddOnSlotNum);
}



