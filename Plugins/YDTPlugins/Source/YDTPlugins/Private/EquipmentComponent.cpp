// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "Pickupable.h"
#include "InventoryComponent.h"
#include "Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UEquipmentComponent::OnPickupGear(FItemInfo& NewGearItemInfo)
{
	bool SuccessChangedGear = false;
	
	switch (NewGearItemInfo.RequiredSlot)
	{
	case ERequiredSlot::Helmet:
		if (NewGearItemInfo.ItemRarity > HelmetSlot.ItemRarity || HelmetSlot.ItemId == 0)
		{
			HelmetSlot = NewGearItemInfo;
			SuccessChangedGear = true;
		}
		break;
	case ERequiredSlot::BodyShield:
		if (NewGearItemInfo.ItemRarity > BodyShieldSlot.ItemRarity || BodyShieldSlot.ItemId == 0)
		{
			BodyShieldSlot = NewGearItemInfo;
			SuccessChangedGear = true;
		}
		break;
	case ERequiredSlot::Backpack:
		if (NewGearItemInfo.ItemRarity > BackpackSlot.ItemRarity || BackpackSlot.ItemId == 0)
		{
			BackpackSlot = NewGearItemInfo;
			
			UInventoryComponent* InvenComp = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));

			if (InvenComp)
			{
				InvenComp->ExtendInventorySlot(BackpackSlot.ItemNum);

				SuccessChangedGear = true;
			}
		}
		break;
	case ERequiredSlot::KnockdownShield:
		break;
	case ERequiredSlot::Weapon:
		break;

		/*	
		case ERequiredSlot::Attachment_Scope: break;
		case ERequiredSlot::Attachment_Extend_Light: break;
		case ERequiredSlot::Attachment_Extend_Heavy: break;
		default: ;
		*/
		
	}

	UpdateEquipment();
	
	return SuccessChangedGear;
}

void UEquipmentComponent::OnModifyShieldValue(const float Amount)
{
	if (BodyShieldSlot.ItemId != 0)
	{
		BodyShieldSlot.ItemNum += Amount;

		if (BodyShieldSlot.ItemNum >= BodyShieldSlot.ItemMaximumStackAmount)
		{
			BodyShieldSlot.ItemNum = BodyShieldSlot.ItemMaximumStackAmount;
		}
		else if (BodyShieldSlot.ItemNum <= 0)
		{
			BodyShieldSlot.ItemNum = 0;
		}
	}
	
}

void UEquipmentComponent::OnShieldTakingDamage(int32 DamageAmount,AController* EventInstigator, AActor* DamageCauser,const UDamageType* DamageType)
{
	if (BodyShieldSlot.ItemNum >= DamageAmount)
	{
		OnModifyShieldValue(DamageAmount * -1);
	}
	else
	{
		DamageAmount -= BodyShieldSlot.ItemNum;
		BodyShieldSlot.ItemNum = 0;

		//The Remaining Damage Apply to hp
		UGameplayStatics::ApplyDamage(GetOwner(),DamageAmount,EventInstigator,DamageCauser,DamageType->GetClass());
	
	}
}

void UEquipmentComponent::UpdateEquipment()
{
	OnUpdateEquipment.Broadcast();
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent,HelmetSlot);
	DOREPLIFETIME(UEquipmentComponent,BodyShieldSlot);
	DOREPLIFETIME(UEquipmentComponent,BackpackSlot);
}

