// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickupable.h"

#include "InventoryComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
APickupable::APickupable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemBoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("ItemBoxCollision"));
	ItemBoxCollision->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void APickupable::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);


	if (bSetupInfoFromDataTable && ItemsDataTable)
	{
		const FName RowName = FName(*FString::FromInt(ItemId));
		
		FItemInfo* LocalItemInfo = ItemsDataTable->FindRow<FItemInfo>(RowName,FString("ItemInfo"));

		if (LocalItemInfo)
		{
			SetupItemInfo(*LocalItemInfo);
		}
	}
}

// Called every frame
void APickupable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupable::SetupItemInfo(FItemInfo& NewItemInfo)
{
	ItemInfo = NewItemInfo;
}

int32 APickupable::ModifyItemNum(int32 AddingValue)
{
	//Do Nothing when the adding value is positive and item stack is max
	if(!FMath::IsNegativeFloat(AddingValue) && ItemInfo.bIsStackFull) return ItemInfo.ItemNum;
	
	ItemInfo.ItemNum += AddingValue;
	if (ItemInfo.ItemNum <= 0 )
	{
		ItemInfo.ItemNum = 0;
	}

	if (ItemInfo.ItemNum >= ItemInfo.ItemMaximumStackAmount)
	{
		ItemInfo.ItemNum = ItemInfo.ItemMaximumStackAmount;
		ItemInfo.ItemMaximumStackAmount = true;
	}

	return ItemInfo.ItemNum;
}

void APickupable::OnInteract_Implementation(AActor* InteractBy)
{
	if (UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(InteractBy->GetComponentByClass(UInventoryComponent::StaticClass())))
	{
		if (InventoryComp->OnPickupItem(ItemInfo))
		{
		
			this->Destroy();
		
		}
	}
}

void APickupable::Server_Destroy_Implementation()
{
	this->Destroy();
}




