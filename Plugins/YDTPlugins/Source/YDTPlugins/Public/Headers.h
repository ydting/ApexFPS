#pragma once

#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Headers.generated.h"

UENUM(BlueprintType)
enum class EItemCategory : uint8 
{
	Ammo,
	Gear,
	Attachment,
	Gear_Defense,
	Regen_Health,
	Regen_Shield,
	Regen_Combo,
};

UENUM(BlueprintType)
enum class EWeaponAttachmentType : uint8
{
	None,
	Scope,
	Mag,
	Barrel,
	Stocks,
};

UENUM(BlueprintType)
enum class ERequiredSlot : uint8 
{
	Inventory,
	Helmet,
	BodyShield,
	Backpack,
	KnockdownShield,
	Weapon
};

UENUM(BlueprintType)
enum class ERarity : uint8 
{
	Common,
	Rare,
	Epic,
	Legendary,
	Heirloom
};

USTRUCT(BlueprintType)
struct FItemInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	FItemInfo()
	{
		
	}

	static bool IsValid(const FItemInfo& ItemInfoToValid) 
	{
		if(ItemInfoToValid.ItemId == 0) return false;
		if(ItemInfoToValid.ItemName == FName(TEXT("Name_NONE"))) return false;

		return true;
	}
	
	FORCEINLINE bool operator==(const FItemInfo& CompareInfo) const
	{
		if (CompareInfo.ItemId != this->ItemId) return false;
		if(CompareInfo.ItemName != this->ItemName) return false;

		return true;
	}

public:
	
	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		int32 ItemId = 0;

	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		EItemCategory ItemCategory = EItemCategory::Ammo;

	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		EWeaponAttachmentType AttachmentType = EWeaponAttachmentType::None;
	
	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		ERequiredSlot RequiredSlot = ERequiredSlot::Inventory;
	
	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		FName ItemName = FName(TEXT("Name_NONE"));

	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		FName ItemDescription = FName(TEXT(""));

	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		ERarity ItemRarity = ERarity::Common;
	
	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)	
		class UTexture2D* ItemTexture = nullptr;

	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		int32 ItemNum = 0;

	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		int32 PerStackAmount = 0;
	
	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		int32 ItemMaximumStackAmount = 0;
	
	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		bool bIsStackable = false;

	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		bool bIsStackFull = false;

	UPROPERTY(Category="Item",EditDefaultsOnly,BlueprintReadOnly)
		TSubclassOf<class APickupable> PickupableClass = nullptr;
};

USTRUCT()
struct FHealingInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Category="Healing",EditDefaultsOnly,BlueprintReadOnly)
	float HealTimeTakes;

	UPROPERTY(Category="Healing",EditDefaultsOnly,BlueprintReadOnly)
	bool HealHealth;

	UPROPERTY(Category="Healing",EditDefaultsOnly,BlueprintReadOnly)
	bool HealShield;

	UPROPERTY(Category="Healing",EditDefaultsOnly,BlueprintReadOnly)
	bool bHealFull;

	UPROPERTY(Category="Healing",EditDefaultsOnly,BlueprintReadOnly)
	int32 HealingAmount;

	UPROPERTY(Category="Healing",EditDefaultsOnly,BlueprintReadOnly)
	class UAnimMontage* HealingMontage;
};

UENUM(BlueprintType)
enum class EScopeType : uint8
{
	IronSight,
	TwoFour,
	Two,
	OneHolo,
	OneDigital
};

USTRUCT(BlueprintType)
struct FScopeInfo : public FTableRowBase
{
	GENERATED_BODY()

	FScopeInfo()
	{
		
	}

public:
	
	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	FName MontageSuffix = FName(TEXT("Iron_Sights"));

	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	class UStaticMesh* Rifle_ScopeMesh = nullptr;

	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	class UStaticMesh* SMG_ScopeMesh = nullptr;

	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	class UStaticMesh* Pistol_ScopeMesh = nullptr;

	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	float AimFOV = 90.0f;

	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	FVector AimCameraPos = FVector(0.f);

	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	EScopeType ScopeType = EScopeType::IronSight;

	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	bool bUseScopeWidget = false;

	UPROPERTY(Category="Scope",EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UUserWidget> ScopeWidgetClass = nullptr;

};

UENUM(BlueprintType)
enum class EStatsType : uint8
{
	Health,
	Stamina
};

USTRUCT(BlueprintType)
struct FStatValue
{
	GENERATED_BODY()

public:

	UPROPERTY(Category="StatValue",EditDefaultsOnly)
	float CurrentValue;

	UPROPERTY(Category="StatValue",EditDefaultsOnly)
	float MinValue;

	UPROPERTY(Category="StatValue",EditDefaultsOnly)
	float MaxValue;
};
