// Fill out your copyright notice in the Description page of Project Settings.


#include "ApexCharacterBase.h"

#include "EasyLibrary.h"
#include "WeaponBase.h"
#include "ParkourMovementComp.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMesh.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense.h"

#include "Engine/World.h"
#include "FunctionLibraryCommon.h"
#include "Interactable.h"
#include "PickupableWeapon.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

#include "YDTPlugins/Public/StatComponent.h"
#include "YDTPlugins/Public/InventoryComponent.h"
#include "YDTPlugins/Public/Pickupable.h"
#include "YDTPlugins/Public/EquipmentComponent.h"

// Sets default values
AApexCharacterBase::AApexCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*AIPerceptionSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(FName("AIPerceptionSource"));*/
// 	AIPerceptionSource->bAutoRegister = true;
// 	AIPerceptionSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
// 	AIPerceptionSource->RegisterForSense(TSubclassOf<UAISense_Damage>());
 /*	AIPerceptionSource->RegisterWithPerceptionSystem();*/

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));

	ArmAnchorComponent = CreateDefaultSubobject<USceneComponent>(FName("ArmsAnchor"));
	ArmAnchorComponent->SetupAttachment(GetMesh());
	ArmAnchorComponent->SetRelativeLocation(FVector(0,0,145));

	BodyCollision =  CreateDefaultSubobject<UCapsuleComponent>(FName("BodyCollision"));
	BodyCollision->SetupAttachment(GetRootComponent());
	BodyCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("ArmsMesh"));
	ArmsMesh->SetupAttachment(ArmAnchorComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));
	SpringArm->SetupAttachment(ArmsMesh);
	SpringArm->TargetArmLength = 0.0f;

	FPSCamera = CreateDefaultSubobject<UCameraComponent>(FName("FPSCamera"));
	FPSCamera->SetupAttachment(SpringArm);
	FPSCamera->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	FPSCamera->SetFieldOfView(85.f);

	BulletSpawnPosition = CreateDefaultSubobject<UArrowComponent>(FName("BulletSpawnPosition"));
	BulletSpawnPosition->SetupAttachment(FPSCamera);

	GrenadeSpawnPosition = CreateDefaultSubobject<UArrowComponent>(FName("GrenadeSpawnPosition"));
	GrenadeSpawnPosition->SetupAttachment(FPSCamera);
	GrenadeSpawnPosition->SetRelativeLocationAndRotation(FVector(85, -38, 2), FRotator(0, 5.0f, 5.0f));

	KnifeComponent = CreateDefaultSubobject<USceneComponent>(FName("KnifeComponent"));
	KnifeComponent->SetupAttachment(ArmsMesh);

	KnifeMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("KnifeMesh"));
	KnifeMesh->SetupAttachment(KnifeComponent);

	ParkourMovementComp = CreateDefaultSubobject<UParkourMovementComp>(FName("ParkourMovementComp"));
	ParkourMovementComp->SetIsReplicated(true);
	
	
	HealthComp = CreateDefaultSubobject<UStatComponent>(FName("StatComp"));


	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(FName("InventoryComp"));
	InventoryComp->SetIsReplicated(true);
	
	EquipmentComp = CreateDefaultSubobject<UEquipmentComponent>(FName("EquipmentComp"));
	EquipmentComp->SetIsReplicated(true);
	
	
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	
	CameraUpdateInterval = 0.01f;
	UpdateBulletSpreadInterval = 0.01f;
	ResetArmRotationInterval = 0.01f;
	CheckFrontInterval = 0.1f;
	CheckFrontDistance = 200.0f;
	CheckFrontRadius = 8.0f;
	
	MouseSensitivityVertical = 1.0f;
	MouseSensitivityHorizontal = 1.0f;

	DefaultArmsHeight = 145.0f;
	CrouchArmsHeight = 65.0f;
	ArmsHeightInterpSpeed = 7.0f;

	MinCameraPitch = -65.0f;
	MaxCameraPitch = 65.0f;

	IntervalBetweenPickupNewWeapon = 0.25f;
	
	bUsePrimarySlot = true;

	CurrentSelectedHealingItemId = -1;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AApexCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);

	if (HasAuthority())
	{
		Initial_Server();
	}
	else
	{
		Initial_Client();
	}

	if (IsLocallyControlled())
	{
		GetWorldTimerManager().SetTimer(CheckFrontTimerHandle, this, &AApexCharacterBase::ServerCheckFront, CheckFrontInterval, true);
		GetWorldTimerManager().SetTimer(CameraUpdateTimerHandle, this, &AApexCharacterBase::SmoothFOVUpdate, CameraUpdateInterval, true);
		GetWorldTimerManager().SetTimer(UpdateBulletSpreadTimerHandle, this, &AApexCharacterBase::UpdateBulletSpread, UpdateBulletSpreadInterval, true);

		FTimerHandle TempHandle;
		GetWorldTimerManager().SetTimer(TempHandle, this, &AApexCharacterBase::UpdateLastInputVector, 0.01f, true);
	}
	
	ParkourMovementComp->Initialize();
}

void AApexCharacterBase::Initial_Server_Implementation()
{
	OnTakeAnyDamage.AddDynamic(this,&AApexCharacterBase::OnTakingAnyDamage);

	//event broadcast at server
	InventoryComp->OnPickupRegenItem.AddDynamic(this,&AApexCharacterBase::OnPickRegenItem);
	InventoryComp->OnPickupAttachment.AddDynamic(this,&AApexCharacterBase::OnPickedAttachment);

}

void AApexCharacterBase::Initial_Client_Implementation()
{

	//Update Widget broadcast at client
	InventoryComp->OnUpdateInventoryWidget.AddDynamic(this,&AApexCharacterBase::UpdateInventoryWidget);
	InventoryComp->OnPickupAmmo.AddDynamic(this,&AApexCharacterBase::UpdateWeaponAmmo);
	EquipmentComp->OnUpdateEquipment.AddDynamic(this,&AApexCharacterBase::UpdatePlayerHP_Timer);

	
	UpdateHealSlotWidget();
}

// Called every frame
void AApexCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DeltaSecond = DeltaTime;

	if(IsLocallyControlled())
	{
		UpdateArmRelativeRot(ArmAnchorComponent->GetRelativeRotation().Pitch);
	}
	else
	{
		ArmAnchorComponent->SetRelativeRotation(FRotator(ArmsPitchValue,0.0f,0.0f));
	}

	UpdateArmHeight();

}

void AApexCharacterBase::Client_UpdatePlayerHP_Implementation()
{
	UpdatePlayerHP_Timer();
}

void AApexCharacterBase::UpdatePlayerHP_Timer()
{
	if (GetLocalRole() == ROLE_Authority && !IsLocallyControlled())
	{
		Client_UpdatePlayerHP();
		return;
	}

	FTimerHandle Temp;

	GetWorldTimerManager().SetTimer(Temp,this,&AApexCharacterBase::UpdatePlayerHP,0.05f,false,0.1f);
}


void AApexCharacterBase::UpdateWeaponAmmo_Client_Implementation()
{
	UpdateWeaponAmmo();
}

void AApexCharacterBase::UpdateWeaponInfo_Client_Implementation()
{
	if(GetLocalRole() == ROLE_Authority && !IsLocallyControlled())
	{
		UE_LOG(LogTemp,Warning,TEXT("Bugged"));
	}
	UpdateWeaponInfo();
	
}

void AApexCharacterBase::OnPickRegenItem(const int32 PickedHealingItemId)
{
	if (CurrentSelectedHealingItemId == -1)
	{
		SetCurrentSelectedHealingItemId(PickedHealingItemId);
	}
	OnRep_ActorAtFront();
}

void AApexCharacterBase::Client_UpdateHealSlotWidget_Implementation()
{
	UpdateHealSlotWidget();
}

void AApexCharacterBase::Client_UpdateHealingWidget_Implementation(const float RemainingTime,const float TotalTime)
{
	UpdateHealingWidget(RemainingTime,TotalTime);
}

void AApexCharacterBase::OnChangedHealingItem(const int32 NewHealingItemId)
{
	if (InventoryComp->GetSameItemTotalNum(NewHealingItemId) >= 1)
	{
		SetCurrentSelectedHealingItemId(NewHealingItemId);
	}
	else
	{
		SetCurrentSelectedHealingItemId(-1);
	}

	UpdateHealSlotWidget();
}

void AApexCharacterBase::SetCurrentSelectedHealingItemId_Implementation(const int32 NewHealingItemId)
{
	CurrentSelectedHealingItemId = NewHealingItemId;
}

FVector AApexCharacterBase::GetOwnerLastInputVector()
{
	return LastInputVector;
}

void AApexCharacterBase::SetAiming_Implementation(const bool InputValue)
{
	bIsAiming = InputValue;
}

void AApexCharacterBase::InitializeComponentsAttach()
{
	if (ArmsMesh->SkeletalMesh)
	{
		SpringArm->AttachToComponent(ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SpringArmAttachSocket);

		FPSCamera->AttachToComponent(SpringArm, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FPSCameraAttachSocket);

		KnifeComponent->AttachToComponent(ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, KnifeAttachSocket);
	}
}

void AApexCharacterBase::Fire_Pressed()
{
	if (bIsHealing)
	{
		Server_OnHealInterrupt();
		return;
	}
	
	if (CurrentSelectedWeapon)
	{
		//one of them are true cant Fire Weapon
		if ( !(ParkourMovementComp->IsSprinted() || bIsHolstered || bIsMeleeAttacking || bIsThrowingGrenade || bIsClosingToWall || bIsReloading || bIsInspectingWeapon)   )
		{
			CurrentSelectedWeapon->StartFire();

			bIsShooting = true;

			//Reenable Timer
			GetWorldTimerManager().SetTimer(ResetArmRotationTimerHandle, this, &AApexCharacterBase::ResetArmRotation, ResetArmRotationInterval, true);

			StartShootingRot = GetControlRotation();
			StartShootPitch = ArmAnchorComponent->GetRelativeRotation().Pitch;
		}

	}
}

void AApexCharacterBase::Fire_Released()
{
	if (CurrentSelectedWeapon)
	{
		CurrentSelectedWeapon->EndFire();

		bIsShooting = false;
	}
}

void AApexCharacterBase::Reload_Pressed()
{
	if (CurrentSelectedWeapon)
	{
		//TODO Bad Hardcore references
		const uint8 byte = (uint8)CurrentSelectedWeapon->WeaponSettings->AmmoType;
		int32 id = 1001 + byte;
		//if dont have ammo to reload
		if (InventoryComp->GetSameItemTotalNum(id) == 0) return;
		
		//one of them are true cant reload Weapon
		if (  !(bIsReloading  ||  ParkourMovementComp->IsSprinted() || bIsHolstered || bIsMeleeAttacking || bIsThrowingGrenade || bIsClosingToWall || bIsInspectingWeapon || bIsShooting || bIsHealing)  )
		{
			CurrentSelectedWeapon->StartReload();
		}
	}
}

void AApexCharacterBase::LookUp(float Value)
{
	LookUpValue = Value;

	if(bOpenedWidget) return;

	FRotator NewRot = ArmAnchorComponent->GetRelativeRotation().GetInverse();

	NewRot.Pitch = FMath::ClampAngle(NewRot.Pitch + Value,MinCameraPitch,MaxCameraPitch);
	NewRot = NewRot.GetInverse();
	
	ArmAnchorComponent->SetRelativeRotation(NewRot);
}

void AApexCharacterBase::TurnRight(float Value)
{
	TurnRightValue = Value;
	AddControllerYawInput(Value * MouseSensitivityHorizontal);
}

float AApexCharacterBase::PlayFPSAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName) const
{
	UAnimInstance* AnimInstance = (ArmsMesh) ? ArmsMesh->GetAnimInstance() : nullptr;

	if (AnimInstance && AnimMontage)
	{
		//makes duration cant be modify after
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.0f)
		{
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
	}

	return 0.0f;
}

int32 AApexCharacterBase::TakingAmmo(int32 RequestAmmo) const
{
	const uint8 Ebyte = (uint8)CurrentSelectedWeapon->WeaponSettings->AmmoType;
	const int32 ItemId = 1001 + Ebyte;

	//total having ammo
	const int32 TotalAmmo = InventoryComp->GetSameItemTotalNum(ItemId);

	const int32 TakingNum = TotalAmmo >= RequestAmmo ? RequestAmmo : TotalAmmo;

	InventoryComp->RemoveItemNumsBySameItemId(TakingNum,ItemId);

	return TakingNum;
}

FVector AApexCharacterBase::GetBulletSpawnPosition() const
{
	return BulletSpawnPosition->GetComponentLocation();
}

void AApexCharacterBase::Server_OnMovementChanged_Implementation(const EMovementMode& NewMovementMode,EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	ParkourMovementComp->OnMovementChanged(PrevMovementMode,NewMovementMode);
}


void AApexCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_OnMovementChanged(GetCharacterMovement()->MovementMode,PrevMovementMode, PreviousCustomMode);
	}

	ParkourMovementComp->OnMovementChanged(PrevMovementMode,GetCharacterMovement()->MovementMode);
}

void AApexCharacterBase::Client_Crouch_Implementation(bool CrouchPressed)
{
	if (CrouchPressed)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void AApexCharacterBase::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);

	TargetArmsHeight = CrouchArmsHeight;

	if (GetLocalRole() == ROLE_Authority && !IsLocallyControlled())
	{
		Client_Crouch(true);
		return;
	}
}

void AApexCharacterBase::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
	 
	TargetArmsHeight = DefaultArmsHeight;

	GetCharacterMovement()->UnCrouch();

	if (GetLocalRole() == ROLE_Authority && !IsLocallyControlled())
	{
		FTimerHandle Temp;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this,FName("Client_Crouch"),false);
		GetWorldTimerManager().SetTimer(Temp,Delegate,0.1f,false,0.1f);
		return;
	}
}

void AApexCharacterBase::Server_UpdateWeaponAttachment_Implementation(bool IsPrimarySlot,
	EWeaponAttachmentType AttachmentType, const FItemInfo& AttachmentInfo, UDataTable* DT_AttachmentInfo)
{
	UpdateWeaponAttachment(IsPrimarySlot,AttachmentType,AttachmentInfo,DT_AttachmentInfo);
}

void AApexCharacterBase::UpdateWeaponAttachment(bool IsPrimarySlot, EWeaponAttachmentType AttachmentType,
                                                               const FItemInfo& AttachmentInfo, UDataTable* DT_AttachmentInfo)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_UpdateWeaponAttachment(IsPrimarySlot,AttachmentType,AttachmentInfo,DT_AttachmentInfo);
	}
	
	AWeaponBase* SlotToUpdate = IsPrimarySlot ? PrimaryWeaponSlot : SecondaryWeaponSlot;

	SlotToUpdate->UpdateAttachment(AttachmentType,AttachmentInfo,DT_AttachmentInfo);

}

void AApexCharacterBase::Server_AddItemToInventory_Implementation(const FItemInfo& AddedItemInfo)
{
	FItemInfo Added = AddedItemInfo;
	InventoryComp->AddItemToInventory(Added);
}

void AApexCharacterBase::Server_DropInventoryItem_Implementation(const int32 SlotIndex,const bool IsDropAll)
{
	InventoryComp->DropItem(SlotIndex,IsDropAll);
}

void AApexCharacterBase::DropSlotWeapon_Implementation(const bool DropPrimaryWeapon)
{
	const FVector ForwardLocation = (GetActorForwardVector() * 100.f) + GetActorLocation();

	const FRotator TempRotator = FRotator(0.f,0.f,0.f);

	AWeaponBase** PrepareDropWeapon = DropPrimaryWeapon ? &PrimaryWeaponSlot : &SecondaryWeaponSlot;
	
	//if not change weapon depend by use what slot,and drop the slot weapon and pick up new one
	if(!(*PrepareDropWeapon)->WeaponSettings->PickupClass)
	{
		UFunctionLibraryCommon::DebugMessage(EDebugType::Warning,TEXT("Weapons Pickup class is not empty!!!"));
		return;
	}

	//remove all attachment
	TMap<EWeaponAttachmentType,FItemInfo> Attachments = (*PrepareDropWeapon)->Attachments;
	(*PrepareDropWeapon)->Attachments.Empty();

	for(const TPair<EWeaponAttachmentType,FItemInfo> Pair : Attachments)
	{
		const FVector RandForwardLocation = (GetActorForwardVector() * 100.f) + (GetActorRightVector() * FMath::RandRange(75,200))+ GetActorLocation();
		
		GetWorld()->SpawnActor<APickupable>(Pair.Value.PickupableClass,RandForwardLocation,TempRotator);
	}

	APickupableWeapon* PickupInstance = GetWorld()->SpawnActor<APickupableWeapon>((*PrepareDropWeapon)->WeaponSettings->PickupClass,ForwardLocation,TempRotator);
	PickupInstance->WeaponClass = (*PrepareDropWeapon)->GetClass();
	PickupInstance->RemainingAmmoInWeapon = (*PrepareDropWeapon)->CurrentMagazineAmmo;

	(*PrepareDropWeapon)->Destroy();
	
}

void AApexCharacterBase::AddWeaponToSlot_Implementation(const bool AddToPrimary, AWeaponBase* NewWeapon)
{
	AWeaponBase** TargetSlot = AddToPrimary ? &PrimaryWeaponSlot : &SecondaryWeaponSlot;
	AWeaponBase* AnotherSlot = AddToPrimary ? SecondaryWeaponSlot : PrimaryWeaponSlot;

	//add to slot and attach to owners arms
	*TargetSlot = NewWeapon;
	(*TargetSlot)->AttachToOwnerArms();

	//should it be hidden in game
	const bool NewWeaponShouldHidden = AddToPrimary != bUsePrimarySlot;
	(*TargetSlot)->SetActorHiddenInGame(NewWeaponShouldHidden);

	//if current using this slot, set holstered to false and hide another slot weapon 

	bIsHolstered = false;

	//if add to slot same as  current slot
	if (AddToPrimary == bUsePrimarySlot)
	{
		CurrentSelectedWeapon = *TargetSlot;

		//hide another slot
		if (AnotherSlot)
		{
			AnotherSlot->SetActorHiddenInGame(true);
		}
	}

	FTimerHandle TempHandle;
	GetWorldTimerManager().SetTimer(TempHandle,this,&AApexCharacterBase::UpdateWeaponInfo_Client,0.1f,false,0.1f);
	
}

void AApexCharacterBase::OnPickupWeapon_Implementation(AWeaponBase* NewWeapon)
{
	//if both weapon slot have weapon
	if (IsValid(PrimaryWeaponSlot) && IsValid(SecondaryWeaponSlot))
	{
		//play holstered and take out

		bIsHolstered = true;

		//TODO Highlight this advanced timer using style

		//Drop Weapon
		FTimerHandle DropTimer;
		FTimerDelegate DropTimerDelegate;
		DropTimerDelegate.BindUFunction(this,FName("DropSlotWeapon"),bUsePrimarySlot);

		GetWorldTimerManager().SetTimer(DropTimer,DropTimerDelegate,IntervalBetweenPickupNewWeapon - 0.1f,false,IntervalBetweenPickupNewWeapon - 0.1f);

		//add weapon to slot
		FTimerHandle Timer;
		FTimerDelegate TimerDelegate;

		//add to slot is bUsePrimarySlot because will drop the current slot and pick up to the slot
		TimerDelegate.BindUFunction(this,FName("AddWeaponToSlot"),bUsePrimarySlot,NewWeapon);
		
		GetWorldTimerManager().SetTimer(Timer,TimerDelegate,IntervalBetweenPickupNewWeapon,false,IntervalBetweenPickupNewWeapon);

		UpdateWeaponInfo_Client();
		
	}
	else
	{
		//one of them must be not valid
		const bool bToPrimary = !IsValid(PrimaryWeaponSlot) ? true : false;

		AddWeaponToSlot(bToPrimary,NewWeapon);
		
	}
}

void AApexCharacterBase::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AApexCharacterBase::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void AApexCharacterBase::Server_Sprint_Implementation(bool bPressed)
{
	if (bPressed)
	{
		Sprint_Pressed();
	}
	else
	{
		Sprint_Released();
	}
}

void AApexCharacterBase::Sprint_Pressed()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Sprint(true);
	}
	ParkourMovementComp->SprintStart();
}

void AApexCharacterBase::Sprint_Released()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Sprint(false);
	}
	ParkourMovementComp->SprintEnd();
}

void AApexCharacterBase::Server_Crouch_Implementation(bool bPressed)
{
	if (bPressed)
	{
		Crouch_Pressed();
	}
	else
	{
		Crouch_Released();
	}
}

void AApexCharacterBase::Crouch_Pressed()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Crouch(true);
	}

	ParkourMovementComp->CrouchEvent();
}

void AApexCharacterBase::Crouch_Released()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Crouch(false);
	}

	ParkourMovementComp->CrouchEnd();
}

void AApexCharacterBase::Server_CrouchSwitch_Implementation()
{
	CrouchSwitch_Pressed();
}

void AApexCharacterBase::CrouchSwitch_Pressed()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_CrouchSwitch();
	}
	ParkourMovementComp->CrouchEvent();
}

void AApexCharacterBase::Aim_Pressed()
{
	if (bOpenedWidget || !CurrentSelectedWeapon) return;

	if(bIsHealing)
	{
		Server_OnHealInterrupt();
		return;
	}
	
	CurrentSelectedWeapon->OnPresesdAim();

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this,FName(TEXT("SetAiming")),true);
	
	GetWorldTimerManager().SetTimer(AimTimer,TimerDelegate,0.2f,false,CurrentSelectedWeapon->AimDelay);
}

void AApexCharacterBase::Aim_Released()
{
	if (bOpenedWidget) return;

	GetWorldTimerManager().ClearTimer(AimTimer);
	
	if (CurrentSelectedWeapon)
	{
		CurrentSelectedWeapon->OnReleaseAim();
	}
	
	SetAiming(false);
}

void AApexCharacterBase::Server_Jump_Implementation()
{
	Jump_Pressed();
}

void AApexCharacterBase::Jump_Pressed()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Jump();
	}
	
	Jump();

	ParkourMovementComp->JumpEvent();
	
}

void AApexCharacterBase::Holster_Pressed()
{
	if (!bIsHolstered)
	{
		bIsHolstered = true;
	}
}

void AApexCharacterBase::InspectWeapon_Pressed()
{
	if (CurrentSelectedWeapon)
	{
		if (CurrentSelectedWeapon->WeaponAnimMontages.InspectWeapon)
		{
			PlayFPSAnimMontage(CurrentSelectedWeapon->WeaponAnimMontages.InspectWeapon);
		}
	}
}

void AApexCharacterBase::SmoothFOVUpdate() const
{
	if (!CurrentSelectedWeapon) return;

	if (bIsAiming && !bIsReloading && !ParkourMovementComp->IsSprinted() && !bIsInspectingWeapon)
	{
		const FVector NewCameraLocation = FMath::VInterpTo(FPSCamera->GetRelativeLocation(), CurrentSelectedWeapon->ScopeInfo.AimCameraPos, DeltaSecond, CameraInterpSpeed);
		FPSCamera->SetRelativeLocation(NewCameraLocation);

		const float NewFOV = FMath::FInterpTo(FPSCamera->FieldOfView, CurrentSelectedWeapon->ScopeInfo.AimFOV, DeltaSecond, CameraInterpSpeed);
		FPSCamera->FieldOfView = NewFOV;

	}
	else
	{
		const FVector NewCameraLocation = FMath::VInterpTo(FPSCamera->GetRelativeLocation(), CurrentSelectedWeapon->DefaultCameraPos, DeltaSecond, CameraInterpSpeed);
		FPSCamera->SetRelativeLocation(NewCameraLocation);

		const float NewFOV = FMath::FInterpTo(FPSCamera->FieldOfView, CurrentSelectedWeapon->DefaultFOV, DeltaSecond, CameraInterpSpeed);
		FPSCamera->FieldOfView = NewFOV;
	}
}

void AApexCharacterBase::ResetArmRotation()
{
	if (CurrentSelectedWeapon)
	{
		//If Player is Pressing Recoil,Don't help reverse recoil
		if(FMath::Abs(LookUpValue) >= 1.0f || FMath::Abs(TurnRightValue) >= 1.0f)
		{
			GetWorldTimerManager().ClearTimer(ResetArmRotationTimerHandle);
		}
		//when player not shooting
		else if ( !bIsShooting )
		{
			const float SpreadResetInterpSpeed = CurrentSelectedWeapon->WeaponSettings->RecoilInfo.BulletSpreadResetInterpSpeed;

			FRotator ArmRot = ArmAnchorComponent->GetRelativeRotation();
			const float NewPitch = FMath::FInterpTo(ArmRot.Pitch,StartShootPitch,DeltaSecond,SpreadResetInterpSpeed);
			ArmRot.Pitch = NewPitch;
			
			const FRotator ControllerRot = GetControlRotation();
			const FRotator NewRotation = FMath::RInterpTo(ControllerRot,StartShootingRot,DeltaSecond,SpreadResetInterpSpeed);

			//if the pitch recoil is very close dont reverse recoil
			if (FMath::IsNearlyEqual(ArmRot.Pitch,StartShootPitch,0.1f))
			{
				GetWorldTimerManager().ClearTimer(ResetArmRotationTimerHandle);
			}
			
			GetController()->SetControlRotation(NewRotation);
			ArmAnchorComponent->SetRelativeRotation(ArmRot);
		}
	}
}

void AApexCharacterBase::UpdateArmRelativeRot_Implementation(float NewArmPitch)
{
	ArmsPitchValue = NewArmPitch;
}

void AApexCharacterBase::UpdateBulletSpread()
{
	if (CurrentSelectedWeapon)
	{
		if (bIsShooting && !CurrentSelectedWeapon->bIsOutOfAmmo)
		{
			const float Pitch = ArmAnchorComponent->GetComponentRotation().Pitch;

			const UCurveFloat* RecoilCurveX = CurrentSelectedWeapon->WeaponSettings->RecoilInfo.RecoilCurveX;
			const UCurveFloat* RecoilCurveY = CurrentSelectedWeapon->WeaponSettings->RecoilInfo.RecoilCurveY;
			
			if (Pitch >= -65.0f && Pitch <= 65.0f && RecoilCurveX && RecoilCurveY)
			{
				const float BulletSpreadRate_Aim = CurrentSelectedWeapon->WeaponSettings->RecoilInfo.BulletSpreadMultiplierAiming;
				const float BulletSpreadRate_Hip = CurrentSelectedWeapon->WeaponSettings->RecoilInfo.BulletSpreadMultiplier;
				
				const float BulletSpreadIntensity = bIsAiming ? BulletSpreadRate_Aim : BulletSpreadRate_Hip;

				const float TimeToGetValue = CurrentSelectedWeapon->ShotsInARow + RecoilTime;
				
				//only add the value between current time and previous time  
				const float YawValue =  RecoilCurveX->GetFloatValue(TimeToGetValue) * BulletSpreadIntensity;
				const float PitchValue =  RecoilCurveY->GetFloatValue(TimeToGetValue) * BulletSpreadIntensity;

				AddControllerYawInput(YawValue);
				ArmAnchorComponent->AddRelativeRotation(FRotator(PitchValue,0,0));
				
				float MinTime = 0.0f;
				float CurveLength = 0.0f;
				RecoilCurveX->GetTimeRange(MinTime,CurveLength);

				if (TimeToGetValue >= CurveLength)
				{
					RecoilTime = 0.0f;
				}
				else
				{
					RecoilTime += UpdateBulletSpreadInterval;
				}
			}
		}
	}
}

void AApexCharacterBase::OnTakingAnyDamage_Implementation(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
															AController* InstigatedBy, AActor* DamageCauser)
{
	//TODO Real Damage Ignore Shield Direct Damage Health
	//if shield cracked or not valid damage to health
	if (EquipmentComp->IsBodyShieldCracked())
	{
		HealthComp->ModifyValue(Damage * -1);
		//UpdatePlayerHP_Timer();
	}
	else
	{
		EquipmentComp->OnShieldTakingDamage(Damage,InstigatedBy,DamageCauser,DamageType);
	}

	UpdatePlayerHP_Timer();
}

void AApexCharacterBase::Interact_Pressed()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Interact_Pressed();
		return;
	}
	
	if (ActorAtFront)
	{
		if (ActorAtFront->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_OnInteract(ActorAtFront,this);
			UFunctionLibraryCommon::DebugMessage(EDebugType::Debug,TEXT("Pressed"),2.0f);
		}
	}
}

void AApexCharacterBase::Server_Interact_Pressed_Implementation()
{
	Interact_Pressed();
}

void AApexCharacterBase::PrimaryWeapon_Pressed()
{
	if (bSwitchingWeapon) return;
	
	SwitchWeapon(true);
}

void AApexCharacterBase::SecondaryWeapon_Pressed()
{
	if (bSwitchingWeapon) return;
	
	SwitchWeapon(false);
}

void AApexCharacterBase::HealingSlot_Pressed()
{
	HealingPressedTime = GetWorld()->GetTimeSeconds();

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this,TEXT("Show_HideHealingSelectMenu"),true);

	GetWorldTimerManager().SetTimer(HealingSelectMenuHandle,TimerDelegate,0.2f,false,0.2f);
}

void AApexCharacterBase::Server_HealingSlot_Released_Implementation()
{

	const FName RowName = FName(*FString::FromInt(CurrentSelectedHealingItemId));
	const FHealingInfo* HealingInfo = DT_HealItem->FindRow<FHealingInfo>(RowName,"");

	if (HealingInfo)
	{
		RemainingHealingTime =  HealingInfo->HealTimeTakes;
		TotalHealingTime = RemainingHealingTime;
			
		CurrentHealingItemId = CurrentSelectedHealingItemId;
	
		GetWorldTimerManager().SetTimer(HealingTickTimer,this,&AApexCharacterBase::OnHealingTick,0.1f,true,0.1f);
		bIsHealing = true;

		ParkourMovementComp->HealingStart();

		if (HealingInfo->HealingMontage)
		{
			HealingMontage = HealingInfo->HealingMontage;
			//Play Healing Montage
			bIsHolstered = true;

			FTimerHandle TempHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this,FName("PlayFPSAnimMontage"),HealingMontage,1.0f,FName("NAME_None"));

			GetWorldTimerManager().SetTimer(TempHandle,TimerDelegate,0.2f,false,0.2f);
				
		}
	}
	
}

void AApexCharacterBase::HealingSlot_Released()
{
	if (GetWorld()->GetTimeSeconds() - HealingPressedTime <= 0.2f)
	{
		if(!DT_HealItem)
		{
			UE_LOG(LogTemp,Warning,TEXT("Warning!!! Missing HealItemInfo Data Table in ApexCharacterBase Class"));
			return;
		}

		Server_HealingSlot_Released();

		const FName RowName = FName(*FString::FromInt(CurrentSelectedHealingItemId));
		const FHealingInfo* HealingInfo = DT_HealItem->FindRow<FHealingInfo>(RowName,"");

		if (HealingInfo)
		{

			ParkourMovementComp->HealingStart();
			
			if (HealingInfo->HealingMontage)
			{
				HealingMontage = HealingInfo->HealingMontage;
				//Play Healing Montage

				FTimerHandle TempHandle;
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this,FName("PlayFPSAnimMontage"),HealingMontage,1.0f,FName("NAME_None"));

				GetWorldTimerManager().SetTimer(TempHandle,TimerDelegate,0.2f,false,0.2f);

			}

			ShowHealingWidget(CurrentSelectedHealingItemId,true);
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Warning!!! Cant Find Match Row"));
		}
	}

	Show_HideHealingSelectMenu(false);

	GetWorldTimerManager().ClearTimer(HealingSelectMenuHandle);
}


void AApexCharacterBase::OnRep_ActorAtFront()
{
	OnDetectedFrontActor(ActorAtFront);
}

void AApexCharacterBase::UpdateArmHeight()
{
	const float NewArmHeight = FMath::FInterpTo(ArmAnchorComponent->GetRelativeLocation().Z,TargetArmsHeight,DeltaSecond,ArmsHeightInterpSpeed);
	FVector ArmLoc = ArmAnchorComponent->GetRelativeLocation();
	ArmLoc.Z = NewArmHeight;
	ArmAnchorComponent->SetRelativeLocation(ArmLoc);
}

void AApexCharacterBase::OnHealingTick()
{
	RemainingHealingTime -= 0.1f;

	if (IsLocallyControlled())
	{
		UpdateHealingWidget(RemainingHealingTime,TotalHealingTime);
	}
	else
	{
		Client_UpdateHealingWidget(RemainingHealingTime,TotalHealingTime);	
	}
	
	if (RemainingHealingTime <= 0)
	{
		OnHealingDone();
	}
	
}

void AApexCharacterBase::Client_OnHealInterrupt_Implementation()
{
	ShowHealingWidget(-1,false);

	ParkourMovementComp->HealingEnd();

	if (ArmsMesh->GetAnimInstance()->Montage_IsPlaying(HealingMontage))
	{
		ArmsMesh->GetAnimInstance()->Montage_Stop(0.2f,HealingMontage);
	}
}

void AApexCharacterBase::Server_OnHealInterrupt_Implementation()
{
	if (IsLocallyControlled())
	{
		ShowHealingWidget(-1,false);
	}
	else
	{
		Client_OnHealInterrupt();
	}

	bIsHolstered = false;

	if (ArmsMesh->GetAnimInstance()->Montage_IsPlaying(HealingMontage))
	{
		ArmsMesh->GetAnimInstance()->Montage_Stop(0.2f,HealingMontage);
	}
	
	GetWorldTimerManager().ClearTimer(HealingTickTimer);

	bIsHealing = false;
	ParkourMovementComp->HealingEnd();
}

void AApexCharacterBase::OnHealingDone()
{
	if(!DT_HealItem)
	{
		UE_LOG(LogTemp,Warning,TEXT("Warning!!! Missing HealItemInfo Data Table in ApexCharacterBase Class"));

		Server_OnHealInterrupt();
		return;
	}

	const FName RowName = FName(*FString::FromInt(CurrentHealingItemId));
	const FHealingInfo* HealingInfo = DT_HealItem->FindRow<FHealingInfo>(RowName,"");

	if (HealingInfo)
	{
		if (HealingInfo->bHealFull)
		{
			if (HealingInfo->HealHealth)
			{
				HealthComp->ModifyValue(999);
			}

			if (HealingInfo->HealShield)
			{
				EquipmentComp->OnModifyShieldValue(999);
			}
		}
		else
		{
			if (HealingInfo->HealHealth)
			{
				HealthComp->ModifyValue(HealingInfo->HealingAmount);
			}

			if (HealingInfo->HealShield)
			{
				EquipmentComp->OnModifyShieldValue(HealingInfo->HealingAmount);
			}
		}

		InventoryComp->RemoveItemNumsBySameItemId(1,CurrentHealingItemId);

		if (IsLocallyControlled())
		{
			UpdateHealSlotWidget();
		}
		else
		{
			Client_UpdateHealSlotWidget();
		}

		UpdatePlayerHP_Timer();
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Warning!!! Cant Find Match Row"));
	}
	
	Server_OnHealInterrupt();
	
}

void AApexCharacterBase::WeaponAttachRefresh() const
{
	if (PrimaryWeaponSlot)
	{
		PrimaryWeaponSlot->AttachToOwnerArms();
	}

	if (SecondaryWeaponSlot)
	{
		SecondaryWeaponSlot->AttachToOwnerArms();
	}
}

void AApexCharacterBase::SetLastInputVector_Implementation(FVector LastInput)
{
	LastInputVector = LastInput;
}

void AApexCharacterBase::UpdateLastInputVector()
{
	SetLastInputVector(GetCharacterMovement()->GetLastInputVector());
}

void AApexCharacterBase::ServerCheckFront_Implementation()
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Init(this,1);
	
	const FHitResult Hit = UEasyLibrary::SimpleCapsuleTrace(this,FPSCamera,CheckFrontDistance,CheckFrontRadius,IgnoredActors,bUseDebug,CheckInteractChannel);

	ActorAtFront = Hit.GetActor();

	OnRep_ActorAtFront();
}

void AApexCharacterBase::SwitchWeapon(const bool SwitchToPrimary)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SwitchWeapon(SwitchToPrimary);
		
	}
	
	bSwitchingWeapon = true;
	FTimerHandle InputDelayTimer;
	GetWorldTimerManager().SetTimer(InputDelayTimer,this,&AApexCharacterBase::OpenSwitchWeaponGate,IntervalBetweenPickupNewWeapon,false,IntervalBetweenPickupNewWeapon);
	
	AWeaponBase* SwitchToSlot = SwitchToPrimary ? PrimaryWeaponSlot : SecondaryWeaponSlot;
	
	AWeaponBase* PreviousSlot = bUsePrimarySlot ? PrimaryWeaponSlot : SecondaryWeaponSlot;
	const bool bAlreadyIsSwitchSlot = SwitchToPrimary == bUsePrimarySlot;
	
	//if the target slot don't have weapon,then only change slot selected
	if (!SwitchToSlot)
	{
		bUsePrimarySlot = SwitchToPrimary;
		bIsHolstered = true;
		return;
	}
	
	//if not using this slot and holstered weapon
	if (!bAlreadyIsSwitchSlot && bIsHolstered)
	{
		//take out weapon
		bUsePrimarySlot = SwitchToPrimary;
		bIsHolstered = false;

		//if previous slot is valid set hidden true
		if (PreviousSlot)
		{
			PreviousSlot->SetActorHiddenInGame(true);
		}
		SwitchToSlot->SetActorHiddenInGame(false);

		CurrentSelectedWeapon = SwitchToSlot;
	}
	//if not using this slot and not holstered weapon
	else if(!bAlreadyIsSwitchSlot && !bIsHolstered)
	{
		//holstered current weapon and set timer to call this function again
		bIsHolstered = true;
		FTimerHandle TempHandle;
		if (SwitchToPrimary)
		{
			GetWorldTimerManager().SetTimer(TempHandle,this,&AApexCharacterBase::PrimaryWeapon_Pressed,SwitchWeaponInterval,false,SwitchWeaponInterval);
		}
		else
		{
			GetWorldTimerManager().SetTimer(TempHandle,this,&AApexCharacterBase::SecondaryWeapon_Pressed,SwitchWeaponInterval,false,SwitchWeaponInterval);
		}
	}
	//if is this slot and not holstered weapon
	else if(bAlreadyIsSwitchSlot && !bIsHolstered)
	{
		//holstered weapon
		bIsHolstered = true;
	}
	//if is already is switch slot but is holstered weapon
	else if(bAlreadyIsSwitchSlot && bIsHolstered)
	{
		//take out weapon
		bIsHolstered = false;
	}

	UpdateWeaponInfo_Client();
}

void AApexCharacterBase::Server_SwitchWeapon_Implementation(const bool SwitchToPrimary)
{
	SwitchWeapon(SwitchToPrimary);
}

void AApexCharacterBase::OnPickedAttachment(const FItemInfo& AttachmentInfo)
{
	//if this function call at server and its not owned at server
	if (GetLocalRole() == ROLE_Authority && !IsLocallyControlled())
	{
		Client_OnPickedAttachment(AttachmentInfo);
	}
	
	if (FItemInfo::IsValid(AttachmentInfo))
	{
		FItemInfo PickedItemInfoRef = AttachmentInfo;

		//if adding not success on first weapon
		if (!AddAttachmentToWeapon(bUsePrimarySlot,PickedItemInfoRef,false))
		{
			//try second weapon
			if (AddAttachmentToWeapon(!bUsePrimarySlot,PickedItemInfoRef,false))
			{
				return;
			}

			//both failed try inventory
			else if(!InventoryComp->AddItemToInventory(PickedItemInfoRef))
			{
				//still failed drop on ground
				const FVector SpawnPoint = GetActorLocation() + (GetActorForwardVector() * 100.0f);
				GetWorld()->SpawnActor<APickupable>(AttachmentInfo.PickupableClass,SpawnPoint,FRotator(0.0f));
			}
		}
	}
	
}

bool AApexCharacterBase::AddAttachmentToWeapon(bool IsPrimaryWeapon,FItemInfo& AttachmentInfo,bool SkipCheckAnotherSlot)
{
	AWeaponBase* WeaponInstance = IsPrimaryWeapon ? PrimaryWeaponSlot : SecondaryWeaponSlot;

	//Weapon is valid
	if (WeaponInstance)
	{
		//the new attachment is better than current weapon having
		if (WeaponInstance->IsTheAttachmentBetter(AttachmentInfo.AttachmentType,AttachmentInfo.ItemRarity))
		{
			//make a copy of the unequip attachment info
			FItemInfo CurrentWeaponAttachmentInfo = WeaponInstance->GetAttachmentInfoByType(AttachmentInfo.AttachmentType);

			//add attachment to weapon
			if (UDataTable* AttachmentDataTable = GetDataTableByAttachmentType(AttachmentInfo.AttachmentType))
			{
				//FItemInfo AttachmentInfoRef = AttachmentInfo;
				WeaponInstance->UpdateAttachment(AttachmentInfo.AttachmentType,AttachmentInfo,AttachmentDataTable);
			}

			//if unequip attachment is valid
			if (FItemInfo::IsValid(CurrentWeaponAttachmentInfo))
			{
				//Check Should Skip this progress?
				if (!SkipCheckAnotherSlot)
				{
					//try add attachment to another weapon slot and set skip checking
					if (AddAttachmentToWeapon(!IsPrimaryWeapon,AttachmentInfo,true))
					{
						return true;
					}
				}
					
				//if another weapon attachment slot didnt have space for it, add unequipped attachment into inventory
				if(InventoryComp->AddItemToInventory(CurrentWeaponAttachmentInfo))
				{
					return true;
				}

				//inv full drop into game world
				const FVector SpawnPoint = GetActorLocation() + (GetActorForwardVector() * 100.0f);
				GetWorld()->SpawnActor<APickupable>(CurrentWeaponAttachmentInfo.PickupableClass,SpawnPoint,FRotator(0.0f));
				return true;
			}
				
			return true;
		}
		else
		{
			return false;
		}
	}
	
	return false;
}

void AApexCharacterBase::Client_OnPickedAttachment_Implementation(const FItemInfo& AttachmentInfo)
{
	OnPickedAttachment(AttachmentInfo);
}

UDataTable* AApexCharacterBase::GetDataTableByAttachmentType(EWeaponAttachmentType AttachmentType) const
{
	UDataTable* AttachmentTable = nullptr;
	
	switch (AttachmentType)
	{
	case EWeaponAttachmentType::None:
		break;
	case EWeaponAttachmentType::Scope:

		AttachmentTable = DT_ScopeItems;
		break;
	case EWeaponAttachmentType::Mag:

		AttachmentTable = DT_Magazine;
		break;
	case EWeaponAttachmentType::Barrel:
		break;
	case EWeaponAttachmentType::Stocks:
		break;
	default: ;
	}

	return AttachmentTable;
}




// Called to bind functionality to input
void AApexCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MouseLookHorizontal"), this, &AApexCharacterBase::TurnRight);
	PlayerInputComponent->BindAxis(FName("MouseLookVertical"), this, &AApexCharacterBase::LookUp);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AApexCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AApexCharacterBase::MoveRight);

	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::Fire_Pressed);
	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Released, this, &AApexCharacterBase::Fire_Released);

	PlayerInputComponent->BindAction(FName("Reload"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::Reload_Pressed);

	PlayerInputComponent->BindAction(FName("Sprint"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::Sprint_Pressed);
	PlayerInputComponent->BindAction(FName("Sprint"), EInputEvent::IE_Released, this, &AApexCharacterBase::Sprint_Released);
	
	PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::Crouch_Pressed);
	PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Released, this, &AApexCharacterBase::Crouch_Released);
	PlayerInputComponent->BindAction(FName("CrouchSwitch"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::CrouchSwitch_Pressed);
	
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::Aim_Pressed);
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Released, this, &AApexCharacterBase::Aim_Released);

	PlayerInputComponent->BindAction(FName("HolsterWeapon"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::Holster_Pressed);
	PlayerInputComponent->BindAction(FName("InspectWeapon"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::InspectWeapon_Pressed);
	
	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::Jump_Pressed);

	PlayerInputComponent->BindAction(FName("Interact"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::Interact_Pressed);

	PlayerInputComponent->BindAction(FName("PrimaryWeapon"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::PrimaryWeapon_Pressed);
	PlayerInputComponent->BindAction(FName("SecondaryWeapon"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::SecondaryWeapon_Pressed);

	PlayerInputComponent->BindAction(FName("HealingSlot"), EInputEvent::IE_Pressed, this, &AApexCharacterBase::HealingSlot_Pressed);
	PlayerInputComponent->BindAction(FName("HealingSlot"), EInputEvent::IE_Released, this, &AApexCharacterBase::HealingSlot_Released);
}

void AApexCharacterBase::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AApexCharacterBase,CurrentSelectedWeapon);
	DOREPLIFETIME(AApexCharacterBase,PrimaryWeaponSlot);
	DOREPLIFETIME(AApexCharacterBase,SecondaryWeaponSlot);

	DOREPLIFETIME(AApexCharacterBase,bIsAiming);
	DOREPLIFETIME(AApexCharacterBase,bIsHealing);
	DOREPLIFETIME(AApexCharacterBase,bIsHolstered);

	DOREPLIFETIME(AApexCharacterBase,ArmsPitchValue);
	DOREPLIFETIME(AApexCharacterBase,TargetArmsHeight);

	DOREPLIFETIME(AApexCharacterBase,LastInputVector);
	
	DOREPLIFETIME_CONDITION(AApexCharacterBase,CurrentSelectedHealingItemId,COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AApexCharacterBase,ActorAtFront,COND_OwnerOnly);
}
