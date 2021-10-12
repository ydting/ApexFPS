// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ApexCharacterBase.h"
#include "BulletCasing.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectile.h"
#include "Particles/ParticleEmitter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName("Root_Component"));
	RootComponent = RootSceneComponent;
	
	WeaponComponent = CreateDefaultSubobject<USceneComponent>(FName("Weapon_Component"));
	WeaponComponent->SetupAttachment(GetRootComponent());
	WeaponComponent->SetIsReplicated(true);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponComponent);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SilencerMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("SilencerMesh"));
	SilencerMesh->SetupAttachment(WeaponMesh);
	SilencerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SliderComponent = CreateDefaultSubobject<USceneComponent>(FName("Slider_Component"));
	SliderComponent->SetupAttachment(GetRootComponent());

	ScopeComponent = CreateDefaultSubobject<USceneComponent>(FName("ScopeComponent"));
	ScopeComponent->SetupAttachment(GetRootComponent());

	ScopeMesh_02 = CreateDefaultSubobject<UStaticMeshComponent>(FName("ScopeMesh_02"));
	ScopeMesh_02->SetupAttachment(ScopeComponent);
	ScopeMesh_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScopeMesh_02->SetVisibility(false);

	ScopeMesh_03 = CreateDefaultSubobject<UStaticMeshComponent>(FName("ScopeMesh_03"));
	ScopeMesh_03->SetupAttachment(ScopeComponent);
	ScopeMesh_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScopeMesh_03->SetVisibility(false);

	ScopeMesh_04 = CreateDefaultSubobject<UStaticMeshComponent>(FName("ScopeMesh_04"));
	ScopeMesh_04->SetupAttachment(ScopeComponent);
	ScopeMesh_04->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScopeMesh_04->SetVisibility(false);

	ScopeMesh_4x = CreateDefaultSubobject<UStaticMeshComponent>(FName("ScopeMesh_4x"));
	ScopeMesh_4x->SetupAttachment(ScopeComponent);
	ScopeMesh_4x->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScopeMesh_4x->SetVisibility(false);
	
	MagazineComponent = CreateDefaultSubobject<USceneComponent>(FName("Magazine_Component"));
	MagazineComponent->SetupAttachment(GetRootComponent());

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("MagazineMesh"));
	MagazineMesh->SetupAttachment(MagazineComponent);
	MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//===============================================================

	DefaultFOV = 90.0f;
	DefaultCameraPos = FVector(-5.f, 0.f, 0.f);
	
	ResetShotsInARowDuration = 0;
	
	WeaponCompAttachName = FName("Weapon_Position");
	SilencerAttachName = FName("SilencerPosition");
	CasingSpawnSocket = FName("CasingPosition");
	MuzzleFlashSocket = FName("MuzzleFlash");
	SilencerMuzzleFlashSocket = FName("SilencerMuzzleFlash");
	
	bUseDebug = false;

	Attachments.Add(EWeaponAttachmentType::Scope,FItemInfo());
	Attachments.Add(EWeaponAttachmentType::Mag,FItemInfo());

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
	
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		ScopeInfo.AimFOV = DefaultAimFOV;
		ScopeInfo.AimCameraPos = DefaultAimCameraPos;

	}

	if (DT_WeaponsInfo)
	{
		const FName WeaponFName = UEnum::GetValueAsName<EWeaponName>(WeaponName);
		
		WeaponSettings = DT_WeaponsInfo->FindRow<FWeaponSettings>(WeaponFName,"");
		if (WeaponSettings)
		{
			MaxMagazineAmmo = WeaponSettings->BaseAmmoClipAmount;
		}
	}
	else if(!WeaponSettings)
	{
		WeaponSettings = new FWeaponSettings();
	}


	bIsOutOfAmmo = CurrentMagazineAmmo <= 0;
	
	if (CurrentMagazineAmmo > MaxMagazineAmmo) CurrentMagazineAmmo = MaxMagazineAmmo;

	check(WeaponSettings);
	
	FireLoopInterval = 60/ WeaponSettings->FireRate;
	if (ResetShotsInARowDuration <= 0)
	{
		ResetShotsInARowDuration = FireLoopInterval * 1.5f;
	}
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AWeaponBase::IsTheAttachmentBetter(EWeaponAttachmentType AttachhmentType, ERarity Rarity) const
{
	 const FItemInfo Result = Attachments.FindRef(AttachhmentType);

	//if not contains this type of attachment then return false
	if (!FItemInfo::IsValid(Result) && !Attachments.Contains(AttachhmentType))
	{
		return false;
	}
	
	return Rarity >= Result.ItemRarity ;
}

FItemInfo AWeaponBase::GetAttachmentInfoByType(EWeaponAttachmentType AttachmentType) const
{
	const auto Pair = Attachments.Find(AttachmentType);

	return *Pair;
}

void AWeaponBase::OnPresesdAim()
{
	if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
	{
		if (MyOwner)
		{
			const FString SectionName = TEXT("Aim_In_") + ScopeInfo.MontageSuffix.ToString();
			
			MyOwner->PlayFPSAnimMontage(WeaponAnimMontages.AimIn,1.f,FName(*SectionName));

			if (ScopeInfo.bUseScopeWidget)
			{
				if (ScopeInfo.ScopeWidgetClass)
				{
					ScopeWidget = CreateWidget<UUserWidget>(GetWorld(),ScopeInfo.ScopeWidgetClass);
					ScopeWidget->AddToViewport();
				}
			}
		}
	}
}

void AWeaponBase::OnReleaseAim()
{
	if (ScopeWidget != nullptr)
	{
		ScopeWidget->RemoveFromParent();
	}
	
	if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
    	{
    		if (MyOwner)
    		{
    			const FString SectionName = TEXT("Aim_In_") + ScopeInfo.MontageSuffix.ToString();
    			
    			MyOwner->PlayFPSAnimMontage(WeaponAnimMontages.AimIn,-1.f,FName(*SectionName));
    		}
    	}
}

void AWeaponBase::StartFire()
{
	GetWorldTimerManager().ClearTimer(ReloadTimer);

	if (bIsOutOfAmmo)
	{
		if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
		{
			if (bIsOutOfAmmo)
			{
				UAnimMontage* MontageToPlay = MyOwner->bIsAiming ? WeaponAnimMontages.AimFire_OutOfAmmo : WeaponAnimMontages.Fire_OutOfAmmo;
				//if montage not valid return
				if (MontageToPlay && MyOwner->bIsAiming)
				{
					const FString SectionName = TEXT("Aim_Fire_Out_Of_Ammo_") + ScopeInfo.MontageSuffix.ToString();
				
					MyOwner->PlayFPSAnimMontage(MontageToPlay,1.0f,FName(*SectionName));
				}
				else if(MontageToPlay)
				{
					MyOwner->PlayFPSAnimMontage(MontageToPlay);
				}
				else return;

				PlayWeaponSFX(EWeaponAction::Fire_OutOfAmmo);
			}
		}
	}
	
	float StartDelay = FireLoopInterval;

	const float IntervalBetweenLastFire = AActor::GetGameTimeSinceCreation() - LastFireTime;
	if (IntervalBetweenLastFire >= FireLoopInterval)
	{
		StartDelay = 0.0f;
	}

	if (IntervalBetweenLastFire >= ResetShotsInARowDuration)
	{
		ShotsInARow = 0;
	}

	switch (WeaponSettings->FireMode)
	{
	case EFireMode::Single:
		GetWorldTimerManager().SetTimer(FireLoopTimer, this, &AWeaponBase::Fire, FireLoopInterval, false, StartDelay);
		break;
	case EFireMode::Automatic:
		GetWorldTimerManager().SetTimer(FireLoopTimer, this, &AWeaponBase::Fire, FireLoopInterval, true, StartDelay);
	default:
		break;
	}
}

void AWeaponBase::ServerFire_Implementation()
{
	Fire();
}

void AWeaponBase::Fire()
{
	//if not calling at server
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}
	
	if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
	{
		if (!bIsOutOfAmmo)
		{
			UAnimMontage* MontageToPlay = MyOwner->bIsAiming ? WeaponAnimMontages.AimFire : WeaponAnimMontages.Fire;
			//if montage not valid return
			if (MontageToPlay && MyOwner->bIsAiming)
			{
				const FString SectionName = TEXT("Aim_Fire_") + ScopeInfo.MontageSuffix.ToString();
				
				MyOwner->PlayFPSAnimMontage(MontageToPlay,1.0f,FName(*SectionName));
			}
			else if(MontageToPlay)
			{
				MyOwner->PlayFPSAnimMontage(MontageToPlay);
			}
			else return;


			if(GetLocalRole() == ROLE_Authority)
			{
				SpawnBullet();

				SpawningCasing();
			
				LastFireTime = AActor::GetGameTimeSinceCreation();
			}

			ShotsInARow++;
			
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(RecoilCameraShake);

			bIsOutOfAmmo = (--CurrentMagazineAmmo) == 0;
			
			MyOwner->UpdateWeaponAmmo_Client();

		}
		else
		{
			EndFire();
		}
	}
}



bool AWeaponBase::ServerFire_Validate()
{
	return true;
}

void AWeaponBase::EndFire()
{
	GetWorldTimerManager().ClearTimer(FireLoopTimer);

	if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
	{
		MyOwner->bIsShooting = false;
	}
}


void AWeaponBase::SpawnBullet_Implementation(bool bForceSpread)
{
	if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		FCollisionResponseParams CollisionResponseParams;

		FVector StartPos = MyOwner->GetBulletSpawnPosition();
		FVector ShootDirection = MyOwner->BulletSpawnPosition->GetForwardVector() * WeaponSettings->ShootingRange;

		
		if (ShotsInARow >= WeaponSettings->RecoilInfo.SpreadAfterShootCount)
		{
			ShootDirection = FMath::VRandCone(ShootDirection, WeaponSettings->RecoilInfo.HorizontalSpreadRate, WeaponSettings->RecoilInfo.VerticalSpreadRate) * WeaponSettings->ShootingRange;
		}
		
		FVector EndPos = StartPos + ShootDirection;

		FRotator LookAtRotation;
		FName SocketName = bUsingSilencer ? SilencerMuzzleFlashSocket : MuzzleFlashSocket;
		FVector SpawnLoc = WeaponMesh->GetSocketLocation(SocketName);
		
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECC_Visibility, QueryParams, CollisionResponseParams))
		{
			EndPos = HitResult.Location;
			FVector X = HitResult.Location - SpawnLoc;

			LookAtRotation = FRotationMatrix::MakeFromX(X).Rotator();
		}
		else
		{
			FVector X = EndPos - SpawnLoc;

			LookAtRotation = FRotationMatrix::MakeFromX(X).Rotator();
		}

		if (bUseDebug)
		{
			//DrawDebugSphere(GetWorld(), StartPos, 3.0f, 12.0f, FColor::Green, false, 1.0f, 0, 1.0f);
			DrawDebugSphere(GetWorld(), EndPos, 3.0f, 12.0f, FColor::Green, false, 1.0f, 0, 1.0f);
			
			DrawDebugDirectionalArrow(GetWorld(),SpawnLoc,(LookAtRotation.Vector() * HitResult.Distance) + SpawnLoc,3.0f,FColor::Green,false,3.0f);
		}
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;

		if (WeaponSettings->BulletProjectileClass)
		{
			AProjectile* Instance = GetWorld()->SpawnActor<AProjectile>(WeaponSettings->BulletProjectileClass,SpawnLoc,LookAtRotation, SpawnParams);
			Instance->Damage = WeaponSettings->Damage;
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT(" Missing Projectile Class  On %s "),*GetName());
		}

		PlayWeaponMuzzleFlash();
		if (!MyOwner->IsLocallyControlled())
		{
			Client_PlayWeaponSFX(EWeaponAction::Fire);
		}
		else
		{
			PlayWeaponSFX(EWeaponAction::Fire);
		}
	}
}

void AWeaponBase::StartReload()
{
	if(!GetNeedReload()) return;
	
	if (WeaponSettings->bIsSingleReload)
	{
		if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
		{
			if (WeaponAnimMontages.OpenReload)
			{
				const float Duration = MyOwner->PlayFPSAnimMontage(WeaponAnimMontages.OpenReload);

				MyOwner->bIsReloading = true;

				PlayWeaponSFX(EWeaponAction::StartReload);

				FTimerHandle TempHandle;
				GetWorldTimerManager().SetTimer(TempHandle, this, &AWeaponBase::Reload, Duration, false);
			}
		}
	}
	else
	{
		Reload();
	}
}

void AWeaponBase::ServerReload_Implementation()
{
	Reload();
}

void AWeaponBase::Reload()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerReload();
	}
	
	if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(GetOwner()))
	{
		if (WeaponSettings->bIsSingleReload)
		{
			if (WeaponAnimMontages.ReloadInsert)
			{
				const float Duration = MyOwner->PlayFPSAnimMontage(WeaponAnimMontages.ReloadInsert) - 0.1f;

				FTimerHandle TempHandle;
				GetWorldTimerManager().SetTimer(TempHandle, this, &AWeaponBase::ReloadDone, Duration, false);

				MyOwner->bIsReloading = true;

				PlayWeaponSFX(EWeaponAction::Reloading);
			}
		}
		else
		{
			if (WeaponAnimMontages.ReloadLoadedWeapon && WeaponAnimMontages.ReloadEmptyWeapon)
			{
				UAnimMontage* MontageToPlay = bIsOutOfAmmo ? WeaponAnimMontages.ReloadEmptyWeapon : WeaponAnimMontages.ReloadLoadedWeapon;

				const float Duration = MyOwner->PlayFPSAnimMontage(MontageToPlay) - 0.1f;

				FTimerHandle TempHandle;
				GetWorldTimerManager().SetTimer(TempHandle, this, &AWeaponBase::ReloadDone, Duration, false);

				MyOwner->bIsReloading = true;

				PlayWeaponSFX(EWeaponAction::Reloading);
			}
		}
	}
}



void AWeaponBase::ReloadDone()
{
	if (GetOwner())
	{
		AApexCharacterBase* CharacterOwner = Cast<AApexCharacterBase>(GetOwner());
		if (CharacterOwner)
		{
			if (WeaponSettings->bIsSingleReload)
			{
				const int32 AmountToFill = MaxMagazineAmmo - CurrentMagazineAmmo;

				const int32 SuccessTakingAmount = CharacterOwner->TakingAmmo(1);

				if (SuccessTakingAmount >= 1)//Success Taking Ammo
				{
					CurrentMagazineAmmo += 1;
					bIsOutOfAmmo = false;

					if (AmountToFill - 1 != 0) //Still Need Reload
					{
						CharacterOwner->bIsReloading = false;
						GetWorldTimerManager().SetTimer(ReloadTimer, this, &AWeaponBase::Reload, 0.01f, false,0.1f);
					}
					else //Done Reload
					{
						//Play Close Reload Montage
						if (WeaponAnimMontages.CloseReload)
						{
							CharacterOwner->PlayFPSAnimMontage(WeaponAnimMontages.CloseReload);
						}

						PlayWeaponSFX(EWeaponAction::EndReload);

						CharacterOwner->bIsReloading = false;
					}
				}
				else //Player Ammo Empty
				{
					CharacterOwner->bIsReloading = false;
				}
			}
			else
			{
				const int32 AmountToFill = MaxMagazineAmmo - CurrentMagazineAmmo;

				const int32 SuccessTakingAmount = CharacterOwner->TakingAmmo(AmountToFill);

				if (SuccessTakingAmount >= 1)
				{
					CurrentMagazineAmmo += SuccessTakingAmount;
					bIsOutOfAmmo = false;

				}

				CharacterOwner->bIsReloading = false;
			}
		}

		CharacterOwner->UpdateWeaponAmmo_Client();
	}
}

void AWeaponBase::AttachToOwnerArms()
{
	if (GetOwner())
	{
		AApexCharacterBase* CharacterOwner = Cast<AApexCharacterBase>(GetOwner());
		if (CharacterOwner)
		{
			SetActorLocation(FVector(0,0,0));
			
			this->AttachToActor(CharacterOwner,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			
			WeaponComponent->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponCompAttachName);
			SetActorRelativeLocation(FVector(0,0,0));

			SliderComponent->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SliderCompAttachName);

			ScopeComponent->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ScopeCompAttachName);

			MagazineComponent->AttachToComponent(CharacterOwner->ArmsMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, MagazineCompAttachName);
		}
	}
}

void AWeaponBase::Client_PlayWeaponSFX_Implementation(EWeaponAction WeaponAction)
{
	PlayWeaponSFX(WeaponAction);
}

void AWeaponBase::PlayWeaponMuzzleFlash()
{
	UParticleSystem* ParticleToPlay = bUsingSilencer ? SilencerMuzzleFlash : MuzzleFlash;

	const FName SocketName = bUsingSilencer ? SilencerMuzzleFlashSocket : MuzzleFlashSocket;
	const FVector MuzzleSpawnLocation = WeaponMesh->GetSocketLocation(SocketName);
	const FRotator MuzzleSpawnRotation = WeaponMesh->GetSocketRotation(SocketName);
	
	UGameplayStatics::SpawnEmitterAttached(ParticleToPlay,WeaponMesh,SocketName);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),BulletTrail,MuzzleSpawnLocation,MuzzleSpawnRotation);

	if (GetLocalRole() == ROLE_Authority)
	{
		ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
		if (CharacterOwner)
		{
			if (!CharacterOwner->IsLocallyControlled())
			{
				Client_PlayMuzzleFlash();
			}
		}
	}
	
}

void AWeaponBase::Client_PlayMuzzleFlash_Implementation()
{
	PlayWeaponMuzzleFlash();
}

void AWeaponBase::SpawningCasing() const
{
	if (*(WeaponSettings->CasingMeshClass))
	{
		const FTransform SpawnT = WeaponMesh->GetSocketTransform(CasingSpawnSocket);

		FActorSpawnParameters SpawnParam;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const ABulletCasing* TempCasing = GetWorld()->SpawnActor<ABulletCasing>(WeaponSettings->CasingMeshClass, SpawnT, SpawnParam);

		const FVector Impulse = SpawnT.GetRotation().GetForwardVector() * FMath::FRandRange(13.f, 16.f);

		TempCasing->CasingMesh->AddImpulse(Impulse);
		
	}
}

bool AWeaponBase::GetNeedReload()
{
	return !(CurrentMagazineAmmo == MaxMagazineAmmo);
}

void AWeaponBase::SetRemainingAmmo(const int32 AmmoCount)
{
	CurrentMagazineAmmo = AmmoCount;

	bIsOutOfAmmo = CurrentMagazineAmmo <= 0;
	if (CurrentMagazineAmmo > MaxMagazineAmmo) CurrentMagazineAmmo = MaxMagazineAmmo;

	if (GetLocalRole() == ROLE_Authority)
	{
		//Client_SetRemainingAmmo(AmmoCount);
	}
}

void AWeaponBase::Client_SetRemainingAmmo_Implementation(const int32 AmmoCount)
{
	SetRemainingAmmo(AmmoCount);
}

void AWeaponBase::UpdateAttachment(EWeaponAttachmentType AttachmentType,const FItemInfo& AttachmentInfo,UDataTable* DT_AttachmentInfo)
{
	const FName ItemIdName = *FString::FromInt(AttachmentInfo.ItemId);

	switch (AttachmentType)
	{
	case EWeaponAttachmentType::Scope:
		{
			FScopeInfo* LocalScopeInfo = DT_AttachmentInfo->FindRow<FScopeInfo>(ItemIdName,"");

			if (LocalScopeInfo)
			{
				Attachments.Add(AttachmentType,AttachmentInfo);

				const EScopeType PrevScopeType = ScopeInfo.ScopeType;
				
				ScopeInfo = *LocalScopeInfo;

				UpdateScopeVisibility(PrevScopeType);

			}
			else
			{
				Attachments.Add(AttachmentType,AttachmentInfo);
				
				ScopeInfo.AimFOV = DefaultAimFOV;
				ScopeInfo.AimCameraPos = DefaultAimCameraPos;

				const EScopeType PrevScopeType = ScopeInfo.ScopeType;
				
				ScopeInfo.MontageSuffix = FName(TEXT("Iron_Sights"));
				ScopeInfo.ScopeType = EScopeType::IronSight;

				UpdateScopeVisibility(PrevScopeType);
			}
		}
		break;

	case EWeaponAttachmentType::Mag:
		break;
	case EWeaponAttachmentType::Barrel:
		break;
	case EWeaponAttachmentType::Stocks:
		break;
	}
	
}

void AWeaponBase::UpdateScopeVisibility(const EScopeType PrevScopeType)
{
	switch (PrevScopeType)
	{
	case EScopeType::IronSight:
		break;
	case EScopeType::TwoFour:
		ScopeMesh_4x->SetVisibility(false);
		break;
	case EScopeType::Two:
		ScopeMesh_02->SetVisibility(false);
		break;
	case EScopeType::OneHolo:
		ScopeMesh_03->SetVisibility(false);
		break;
	case EScopeType::OneDigital:
		ScopeMesh_04->SetVisibility(false);
		break;
	default: ;
	}

	switch (ScopeInfo.ScopeType)
	{
	case EScopeType::IronSight:
		break;
	case EScopeType::TwoFour:
		ScopeMesh_4x->SetVisibility(true);
		break;
	case EScopeType::Two:
		ScopeMesh_02->SetVisibility(true);
		break;
	case EScopeType::OneHolo:
		ScopeMesh_03->SetVisibility(true);
		break;
	case EScopeType::OneDigital:
		ScopeMesh_04->SetVisibility(true);
		break;
	default: ;
	}
}

void AWeaponBase::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AWeaponBase,ScopeInfo,COND_OwnerOnly);
}
