// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Headers.h"
#include "GameFramework/Character.h"
#include "YDTPlugins/Public/Interactable.h"
#include "ApexCharacterBase.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USceneComponent;
class UArrowComponent;
class UBoxComponent;
class USkeletalMesh;
class UStaticMeshComponent;
class UAIPerceptionStimuliSourceComponent;	
class AWeaponBase;
	
UCLASS()
class LOWPOLYFPSPACK425_API AApexCharacterBase : public ACharacter , public IInteractable
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable,Server,Reliable)
	virtual void Initial_Server();

	UFUNCTION(Client,Reliable)
	void Initial_Client();

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
	
public:	

	//===============================================================
	//Function

	AApexCharacterBase();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//function: 
	//          PlayFPSAnimMontage 
	//Access:
	//           public  
	//Parameter:
	//          [in] class UAnimMontage * Montage - montage to play
	//          [in] float InPlayRate - montage play rate
	//          [in] FName StartSection - StartSection
	//Returns:
	//          float - Duration of montage
	//Remarks:
	//          ...
	//author:    MSI[ydting]
	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
	float PlayFPSAnimMontage(class UAnimMontage* Montage, float InPlayRate = 1.0f, FName StartSection = NAME_None) const;

	/**
	 * @brief taking ammo from inventory
	 * @param RequestAmmo Amount to request taking
	 * @return success taking amount
	 */
	UFUNCTION(BlueprintCallable)
	int32 TakingAmmo(int32 RequestAmmo) const;

	FVector GetBulletSpawnPosition() const;

	UFUNCTION(Server,Unreliable)
	void Server_OnMovementChanged(const EMovementMode& NewMovementMode,EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	UFUNCTION(Client,Unreliable)
	void Client_Crouch(bool CrouchPressed);
	
	virtual void Crouch(bool bClientSimulation = false) override;

	virtual void UnCrouch(bool bClientSimulation = false) override;

	//===============================================================

	UFUNCTION(BlueprintCallable)
	void UpdateWeaponAttachment(bool IsPrimarySlot,EWeaponAttachmentType AttachmentType,const FItemInfo& AttachmentInfo,UDataTable* DT_AttachmentInfo);

	UFUNCTION(Server,Reliable)
	void Server_UpdateWeaponAttachment(bool IsPrimarySlot,EWeaponAttachmentType AttachmentType,const FItemInfo& AttachmentInfo,UDataTable* DT_AttachmentInfo);
	
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void Server_AddItemToInventory(const FItemInfo& AddedItemInfo);
	
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void Server_DropInventoryItem(const int32 SlotIndex,const bool IsDropAll);
	
	UFUNCTION(Server,Unreliable,BlueprintCallable)
	void DropSlotWeapon(const bool DropPrimaryWeapon);
	
	UFUNCTION(Server,Reliable)
	void AddWeaponToSlot(const bool AddToPrimary, AWeaponBase* NewWeapon);
	
	UFUNCTION(Server,Reliable)
	void OnPickupWeapon(AWeaponBase* NewWeapon);

	void SwitchWeapon(const bool SwitchToPrimary);

	UFUNCTION(Server,Unreliable)
	void Server_SwitchWeapon(const bool SwitchToPrimary);
	
	void OpenSwitchWeaponGate() { bSwitchingWeapon = false;}

	UFUNCTION()
	void OnPickedAttachment(const FItemInfo& AttachmentInfo);

	bool AddAttachmentToWeapon(bool IsPrimaryWeapon,FItemInfo& AttachmentInfo,bool SkipCheckAnotherSlot = false);
	
	UFUNCTION(Client,Unreliable)
	void Client_OnPickedAttachment(const FItemInfo& AttachmentInfo);

	UDataTable* GetDataTableByAttachmentType(EWeaponAttachmentType AttachmentType) const;
	//===============================================================

	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerInfoWidget();

	//===============================================================

	UFUNCTION(BlueprintImplementableEvent)
		void OnDetectedFrontActor(AActor* DetectedActor);

	//===============================================================
	//Widget Update

	UFUNCTION(Client,Unreliable)
		void Client_UpdatePlayerHP();

	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePlayerHP();

	void UpdatePlayerHP_Timer();
	
	UFUNCTION(Client,Unreliable)
	void UpdateWeaponAmmo_Client();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponAmmo();


	
	UFUNCTION(Client,Unreliable)
	void UpdateWeaponInfo_Client();
	
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void UpdateWeaponInfo();


	
	UFUNCTION()
	void OnPickRegenItem(const int32 PickedHealingItemId);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateInventoryWidget();

	UFUNCTION(Client,Unreliable)
	void Client_UpdateHealSlotWidget();
	
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void UpdateHealSlotWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void Show_HideHealingSelectMenu(const bool Show = true);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHealingWidget(const int32 HealingItemId,const bool Show);

	UFUNCTION(Client,Unreliable)
	void Client_UpdateHealingWidget(const float RemainingTime,const float TotalTime);
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealingWidget(const float RemainingTime,const float TotalTime);
	
	//===============================================================

	UFUNCTION(BlueprintCallable)
	void OnChangedHealingItem(const int32 NewHealingItemId);

	UFUNCTION(Server,Unreliable)
	void SetCurrentSelectedHealingItemId(const int32 NewHealingItemId);
	
	//===============================================================
	
	UFUNCTION()
	FVector GetOwnerLastInputVector();
	
private:

	UFUNCTION(Server,Unreliable)
	void SetAiming(const bool InputValue);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void InitializeComponentsAttach();

	UFUNCTION()
		void Fire_Pressed();

	UFUNCTION()
		void Fire_Released();

	UFUNCTION()
		void Reload_Pressed();

	UFUNCTION()
	void LookUp(float Value);

	UFUNCTION()
	void TurnRight(float Value);
	
	UFUNCTION()
		void MoveForward(float Value);
	
	UFUNCTION()
		void MoveRight(float Value);

	UFUNCTION(Server,Unreliable)
		void Server_Sprint(bool bPressed);
	
	UFUNCTION()
		void Sprint_Pressed();

	UFUNCTION()
		void Sprint_Released();

	UFUNCTION(Server,Unreliable)
	void Server_Crouch(bool bPressed);
	
	UFUNCTION()
		void Crouch_Pressed();

	UFUNCTION()
		void Crouch_Released();

	UFUNCTION(Server,Unreliable)
	void Server_CrouchSwitch();
	
	UFUNCTION()
		void CrouchSwitch_Pressed();

	
	
	UFUNCTION()
		void Aim_Pressed();

	UFUNCTION()
		void Aim_Released();

	UFUNCTION(Server,Unreliable)
	void Server_Jump();
	
	UFUNCTION()
    	void Jump_Pressed();	
    	
    UFUNCTION()
        void Holster_Pressed();

	UFUNCTION()
		void InspectWeapon_Pressed();

	UFUNCTION()
		void Interact_Pressed();

	UFUNCTION(Server,Unreliable)
		void Server_Interact_Pressed();
	
	void PrimaryWeapon_Pressed();
	
	void SecondaryWeapon_Pressed();

	void HealingSlot_Pressed();

	UFUNCTION(Server,Unreliable)
	void Server_HealingSlot_Released();
	
	void HealingSlot_Released();
	
	//===============================================================
	
	UFUNCTION()
	void SmoothFOVUpdate() const;

	//===============================================================
	//Recoil Pattern System
	
	UFUNCTION()
	void ResetArmRotation();

	UFUNCTION(Server,Unreliable)
	void UpdateArmRelativeRot(float NewArmPitch);
	
	UFUNCTION()
	void UpdateBulletSpread();

	//===============================================================

	UFUNCTION(Server,Reliable)
	void OnTakingAnyDamage(AActor* DamagedActor, float Damage , const class UDamageType* DamageType
											, class AController* InstigatedBy, AActor* DamageCauser );

	//===============================================================

	UFUNCTION(Server,Unreliable)
	void ServerCheckFront();

	UFUNCTION()
	void OnRep_ActorAtFront();

	UFUNCTION()
	void UpdateArmHeight();
	
	void OnHealingTick();

	UFUNCTION(Client,Unreliable)
	void Client_OnHealInterrupt();

	UFUNCTION(Server,Unreliable)
	void Server_OnHealInterrupt();

	void OnHealingDone();

	UFUNCTION()
	void WeaponAttachRefresh() const;

	UFUNCTION(Server,Unreliable)
	void SetLastInputVector(FVector LastInput);
	
	void UpdateLastInputVector();
	
public:

	//===============================================================
	//Components


// 	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
// 		UAIPerceptionStimuliSourceComponent* AIPerceptionSource;

	UPROPERTY(Category = "Components",VisibleAnywhere,BlueprintReadOnly)
		class UCapsuleComponent* BodyCollision;
	
	UPROPERTY(Category = "Components",VisibleAnywhere,BlueprintReadOnly)
		USceneComponent* ArmAnchorComponent;

	UPROPERTY(Category = "Components",VisibleAnywhere,BlueprintReadOnly)
		USkeletalMeshComponent* ArmsMesh;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
		USpringArmComponent* SpringArm;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
		UCameraComponent* FPSCamera;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
		UArrowComponent* BulletSpawnPosition;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
		UArrowComponent* GrenadeSpawnPosition;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* KnifeComponent;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* KnifeMesh;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		class UParkourMovementComp* ParkourMovementComp;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		class UStatComponent* HealthComp;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		class UInventoryComponent* InventoryComp;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadWrite)
		class UEquipmentComponent* EquipmentComp;
	
	//===============================================================
	//Variables

	UPROPERTY(Category = "DataTable", EditDefaultsOnly, BlueprintReadOnly)
		 UDataTable* DT_HealItem;

	UPROPERTY(Category = "DataTable", EditDefaultsOnly, BlueprintReadOnly)
		UDataTable* DT_ScopeItems;

	UPROPERTY(Category = "DataTable", EditDefaultsOnly, BlueprintReadOnly)
		UDataTable* DT_Magazine;
	
	//===============================================================
	
	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName SpringArmAttachSocket;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName FPSCameraAttachSocket;

	UPROPERTY(Category = "Socket", EditDefaultsOnly, BlueprintReadWrite)
		FName KnifeAttachSocket;

	//===============================================================

	UPROPERTY(Category = "Weapon", EditDefaultsOnly, BlueprintReadWrite,Replicated)
		AWeaponBase* CurrentSelectedWeapon;

	UPROPERTY(Category = "Weapon", EditDefaultsOnly, BlueprintReadWrite,ReplicatedUsing=WeaponAttachRefresh) 
		AWeaponBase* PrimaryWeaponSlot;

	UPROPERTY(Category = "Weapon", EditDefaultsOnly, BlueprintReadWrite,ReplicatedUsing=WeaponAttachRefresh) 
		AWeaponBase* SecondaryWeaponSlot;

	UPROPERTY(Category = "Weapon", EditDefaultsOnly, BlueprintReadWrite) 
		bool bUsePrimarySlot;

	UPROPERTY(Category = "Weapon", EditDefaultsOnly, BlueprintReadWrite) 
		float SwitchWeaponInterval;

	bool bSwitchingWeapon;

	//===============================================================

	UPROPERTY(Category = "Consumable", EditDefaultsOnly, BlueprintReadOnly,ReplicatedUsing=UpdateHealSlotWidget) 
	int32 CurrentSelectedHealingItemId;;

	
	//===============================================================

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		float DefaultArmsHeight;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		float CrouchArmsHeight;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		float ArmsHeightInterpSpeed;

	//===============================================================

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		bool bIsShooting;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		bool bIsJumping;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		bool bIsReloading;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite,Replicated)
	bool bIsAiming;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite,Replicated)
		bool bIsHolstered;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		bool bIsThrowingGrenade;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		bool bIsClosingToWall;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		bool bIsInspectingWeapon;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		bool bIsMeleeAttacking;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite,Replicated)
		bool bIsHealing;

	UPROPERTY(Category = "Character", EditDefaultsOnly, BlueprintReadWrite)
		bool bOpenedWidget;
	
	//===============================================================
	
	UPROPERTY(Category = "BulletSpread", EditDefaultsOnly, BlueprintReadWrite)
		float UpdateBulletSpreadInterval;

	UPROPERTY(Category = "BulletSpread", EditDefaultsOnly, BlueprintReadWrite)
		float ResetArmRotationInterval;

	//===============================================================

	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		float CameraInterpSpeed;

	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		float CameraUpdateInterval;
	
	UPROPERTY(Category = "AimSettings|Mouse", EditDefaultsOnly, BlueprintReadWrite)
		float MouseSensitivityHorizontal;

	UPROPERTY(Category = "AimSettings|Mouse", EditDefaultsOnly, BlueprintReadWrite)
		float MouseSensitivityVertical;

	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		float MinCameraPitch;

	UPROPERTY(Category = "AimSettings", EditDefaultsOnly, BlueprintReadWrite)
		float MaxCameraPitch; 

	//===============================================================

	UPROPERTY(Category = "InteractSystem" ,BlueprintReadOnly,ReplicatedUsing=OnRep_ActorAtFront)
	AActor* ActorAtFront;
	
	UPROPERTY(Category = "InteractSystem",EditDefaultsOnly ,BlueprintReadOnly)
		float CheckFrontInterval;

	UPROPERTY(Category = "InteractSystem",EditDefaultsOnly ,BlueprintReadOnly)
		float CheckFrontDistance;

	UPROPERTY(Category = "InteractSystem",EditDefaultsOnly ,BlueprintReadOnly)
		float CheckFrontRadius;
	
	UPROPERTY(Category = "InteractSystem",EditDefaultsOnly ,BlueprintReadOnly)
		TEnumAsByte<ECollisionChannel> CheckInteractChannel;

	UPROPERTY(Category = "InteractSystem",EditDefaultsOnly ,BlueprintReadOnly)
		float IntervalBetweenPickupNewWeapon;
	
	//===============================================================

	UPROPERTY(Category = "InteractSystem",EditDefaultsOnly ,BlueprintReadOnly)
		bool bUseDebug;

	//===============================================================

protected:


	// For Smooth Crouch Transition
	UPROPERTY(Replicated)
	float TargetArmsHeight;

	UPROPERTY(Replicated,BlueprintReadOnly)
	float ArmsPitchValue;
	
	float LookUpValue;
	float TurnRightValue;

	//===============================================================
	//Recoil Pattern System
	FRotator StartShootingRot;
	float StartShootPitch;
	//===============================================================

	float HealingPressedTime;

	FTimerHandle HealingSelectMenuHandle;
	
	float RecoilTime;

	float StepBeforeRecoilTime;
	
	float DeltaSecond;

	FTimerHandle CameraUpdateTimerHandle;
	
	FTimerHandle ResetArmRotationTimerHandle;

	FTimerHandle UpdateBulletSpreadTimerHandle;

	FTimerHandle CheckFrontTimerHandle;

	//===============================================================
	//Healing System
	
	FTimerHandle HealingTickTimer;

	FTimerHandle AimTimer;

	float RemainingHealingTime;

	float TotalHealingTime;

	int32 CurrentHealingItemId;

	UAnimMontage* HealingMontage;

	UPROPERTY(Replicated)
	FVector LastInputVector;
	
};



