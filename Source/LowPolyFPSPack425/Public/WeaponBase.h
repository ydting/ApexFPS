// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PickupableWeapon.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "YDTPlugins/Public/Headers.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "WeaponBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UAnimMontage;
class USoundBase;
class ABulletCasing;
class UParticleSystem;
class UCurveFloat;

USTRUCT(BlueprintType)
struct FWeaponAnimMontage : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* InspectWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* ThrowGrenade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* OpenReload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* ReloadInsert;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* CloseReload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* ReloadEmptyWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* ReloadLoadedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Fire_OutOfAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AimFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AimFire_OutOfAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AimIn;
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol,
	Rifle,
	SMG,
	Sniper,
	Shotguns,
	Launcher
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	LightAmmo,
	HeavyAmmo,
	ShotgunAmmo,
	SniperAmmo
};

UENUM(BlueprintType)
enum EWeaponName
{
	Glock18,
	FiveSeven,
	P1911,
	MK6,
	AK47,
	M401,
	GL56,
	MP9,
	UMP45,
	MP7,
	P90,
	MP5,
	XM1014,
	M24,
	M82,
	Dragunov
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Single,
	Automatic,
	Launcher
};

UENUM(BlueprintType)
enum class EWeaponAction : uint8
{
	Fire,
	Fire_OutOfAmmo,
	StartReload,
	Reloading,
	Reloading_OutOfAmmo,
	EndReload,
	HolsterWeapon,
	TakeoutWeapon,
	AimIn_Out,
};

UENUM(BlueprintType)
enum class EAxisDirection : uint8
{
	X,
	Y,
	Z
};

USTRUCT(BlueprintType)
struct FWeeponRecoilInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "RecoilSettings|BulletSpread", EditDefaultsOnly, BlueprintReadWrite)
		UCurveFloat* RecoilCurveX;

	UPROPERTY(Category = "RecoilSettings|BulletSpread", EditDefaultsOnly, BlueprintReadWrite)
		UCurveFloat* RecoilCurveY;

	UPROPERTY(Category = "RecoilSettings|BulletSpread", EditDefaultsOnly, BlueprintReadWrite)
		float BulletSpreadMultiplierAiming  = 0.075;

	UPROPERTY(Category = "RecoilSettings|BulletSpread", EditDefaultsOnly, BlueprintReadWrite)
		float BulletSpreadMultiplier = 0.15f;

	UPROPERTY(Category = "RecoilSettings|BulletSpread", EditDefaultsOnly, BlueprintReadWrite)
		float BulletSpreadResetInterpSpeed = 5.0f;

	UPROPERTY(Category = "RecoilSettings", EditDefaultsOnly, BlueprintReadWrite)
		float VerticalSpreadRate = 0.01f;

	UPROPERTY(Category = "RecoilSettings", EditDefaultsOnly, BlueprintReadWrite)
		float HorizontalSpreadRate = 0.f;

	UPROPERTY(Category = "RecoilSettings", EditDefaultsOnly, BlueprintReadWrite)
		float RecoilRate = 0.01f;

	UPROPERTY(Category = "RecoilSettings", EditDefaultsOnly, BlueprintReadWrite)
		int32 SpreadAfterShootCount = 5;
	
};

USTRUCT(BlueprintType)
struct FWeaponSettings : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		EFireMode FireMode = EFireMode::Single;

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		EWeaponType WeaponType = EWeaponType::Pistol;
	
	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		EAmmoType AmmoType = EAmmoType::LightAmmo;

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 10.0f;
	
	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		float ShootingRange = 200000;

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		float FireRate = 126.0f;

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		bool bIsSingleReload = false;

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<class APickupableWeapon> PickupClass = nullptr;

	UPROPERTY(Category = "WeaponDefaults", EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<class ABulletCasing> CasingMeshClass  = nullptr;

	UPROPERTY(Category = "WeaponDefaults", EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<class AProjectile> BulletProjectileClass  = nullptr;
	
	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		int BaseAmmoClipAmount = 15;

	UPROPERTY(Category = "Recoil", EditDefaultsOnly, BlueprintReadWrite)
		FWeeponRecoilInfo RecoilInfo;

};

UCLASS()
class LOWPOLYFPSPACK425_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	EScopeType GetCurrentScopeType() const {return ScopeInfo.ScopeType;}

	UFUNCTION(BlueprintCallable)
	EAmmoType GetAmmoType() const { return WeaponSettings->AmmoType; }
	
	bool IsTheAttachmentBetter(EWeaponAttachmentType AttachhmentType,ERarity Rarity) const;

	FItemInfo GetAttachmentInfoByType(EWeaponAttachmentType AttachmentType) const;
	
	virtual  void OnPresesdAim();

	void OnReleaseAim();
	
	void StartFire();

	virtual void Fire();

	UFUNCTION(Server,Reliable,WithValidation)
	void ServerFire();
	
	void EndFire();

	UFUNCTION(Server,Reliable)
	virtual void SpawnBullet(bool bForceSpread = false);

	virtual void StartReload();

	void Reload();

	UFUNCTION(Server,Reliable)
	void ServerReload();

	virtual void ReloadDone();

	UFUNCTION(Category = "Weapon", BlueprintCallable)
		virtual void AttachToOwnerArms();

	UFUNCTION(Client,Unreliable)
		void Client_PlayWeaponSFX(EWeaponAction WeaponAction);

	UFUNCTION(BlueprintImplementableEvent,Category ="Weapon")
		void PlayWeaponSFX(EWeaponAction WeaponAction);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void PlayWeaponMuzzleFlash();

	UFUNCTION(Client,Unreliable)
		void Client_PlayMuzzleFlash();

	void SpawningCasing() const;

	bool GetNeedReload();

	void SetRemainingAmmo(const int32 AmmoCount);

	UFUNCTION(Client,Unreliable)
	void Client_SetRemainingAmmo(const int32 AmmoCount);

	UFUNCTION(BlueprintCallable)
	void UpdateAttachment(EWeaponAttachmentType AttachmentType,UPARAM(ref) const FItemInfo& AttachmentInfo,UDataTable* DT_AttachmentInfo);

	void UpdateScopeVisibility(const EScopeType PrevScopeType);
	
	//===============================================================
	//Components

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* RootSceneComponent;
	
	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* WeaponComponent;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* WeaponMesh;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* SilencerMesh;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* ScopeComponent;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* SliderComponent;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* MagazineComponent;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ScopeMesh_4x;
	
	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ScopeMesh_02;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ScopeMesh_03;

	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ScopeMesh_04;


	
	UPROPERTY(Category = "Component", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MagazineMesh;
	
	//===============================================================
	//Variables

	//===============================================================
	//Socket Category
	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName WeaponCompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName SilencerAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
	FName SliderCompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
	FName ScopeCompAttachName;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
	FName MagazineCompAttachName;
	
	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName CasingSpawnSocket;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName MuzzleFlashSocket;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName SilencerMuzzleFlashSocket;

	//===============================================================
	//Weapon Category

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		bool bUsingSilencer;

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly,BlueprintReadOnly)
		TEnumAsByte<EWeaponName> WeaponName = EWeaponName::FiveSeven;

	FWeaponSettings* WeaponSettings;

	UPROPERTY(Category = "WeaponSettings", EditDefaultsOnly, BlueprintReadWrite)
		UDataTable* DT_WeaponsInfo;
	
	//===============================================================
	
	UPROPERTY(Category = "WeaponMontage", EditDefaultsOnly, BlueprintReadWrite)
		FWeaponAnimMontage WeaponAnimMontages;

	UPROPERTY(Category = "Attachment", EditDefaultsOnly ,BlueprintReadOnly)
		TMap<EWeaponAttachmentType,FItemInfo> Attachments;
	
	//===============================================================
	
	UPROPERTY(Category = "AmmoSettings", EditDefaultsOnly, BlueprintReadWrite)
		int32 CurrentMagazineAmmo;

	UPROPERTY(Category = "AmmoSettings", EditDefaultsOnly, BlueprintReadWrite)
		int32 MaxMagazineAmmo;

	UPROPERTY(Category = "AmmoSettings",EditDefaultsOnly, BlueprintReadWrite,Replicated)
		bool bIsOutOfAmmo;
	
	//===============================================================
	
		float ResetShotsInARowDuration;
	
	UPROPERTY(Category = "RecoilSettings", EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<class UMatineeCameraShake> RecoilCameraShake;
	
	//===============================================================

	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		float AimDelay = 0.2f;
	
	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		float DefaultFOV;

	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		float DefaultAimFOV;
	
	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		FVector DefaultCameraPos;

	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		FVector DefaultAimCameraPos;

	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadOnly,Replicated)
	FScopeInfo ScopeInfo;
	
	UUserWidget* ScopeWidget;

	//===============================================================
	
	UPROPERTY(Category = "VFX", EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* MuzzleFlash;

	UPROPERTY(Category = "VFX", EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* SilencerMuzzleFlash;

	UPROPERTY(Category = "VFX", EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* BulletTrail;

	//===============================================================
	
	UPROPERTY(Category = "Debug", EditDefaultsOnly)
		bool bUseDebug;

	//===============================================================
	
	UPROPERTY(Category = "Debug", BlueprintReadOnly)
	int32 ShotsInARow;
	
protected:
	
	
	FTimerHandle ReloadTimer;
	
	FTimerHandle FireLoopTimer;
	
	float LastFireTime;

	float FireLoopInterval;

	bool IsIronSight;
};
