// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourMovementComp.h"

#include <string>

#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Components/CapsuleComponent.h"
#include "FunctionLibraryCommon.h"
#include "Net/UnrealNetwork.h"
#include "ApexCharacterBase.h"

// Sets default values for this component's properties
UParkourMovementComp::UParkourMovementComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bUseCameraShake = true;

	MantleHeight = 180.0f;
	LedgeGrabJumpOffForce = 300.0f;
	LedgeGrabJumpHeight = 400.0f;
	LedgeGrabViewAngle = 180.0f;
	
	VerticalWallRunSpeed = 300.0f;

	SprintSpeed = 800.0f;
	SlideImpulseAmount = 30.0f;
	SlideStartImpulseAmount = 600.0f;

	
}


// Called when the game starts
void UParkourMovementComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



// Called every frame
void UParkourMovementComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DeltaSeconds = DeltaTime;
	// ...
	if (bMantleCheckGateOpened)
	{
		MantleCheck();
	}

	if (bMantleGateOpened)
	{
		MantleMovement();
	}

	if (bVerticalWallRunGateOpened)
	{
		VerticalWallRunUpdate();
	}

	if (bSlideGateOpened)
	{
		SlideUpdate();
	}

	if (bUseDebug)
	{
		FString VerticalGateState = TEXT("VerticalGate:");
		VerticalGateState.Append(bVerticalWallRunGateOpened ? TEXT("True") : TEXT("False"));

		UFunctionLibraryCommon::DebugMessage(EDebugType::Debug,VerticalGateState,0.0f);

		FString MantleGateState = TEXT("MantleGate:");
		MantleGateState.Append(bVerticalWallRunGateOpened ? TEXT("True") : TEXT("False"));

		UFunctionLibraryCommon::DebugMessage(EDebugType::Debug,MantleGateState,0.0f);
	}
	
}

bool UParkourMovementComp::CanVerticalWallRun() const
{
	bool bVerticalWallRun = false;
	const bool bStateAble = CurrentParkourMovement == EParkourMovementType::None || CurrentParkourMovement == EParkourMovementType::VerticalWallRun || CurrentParkourMovement == EParkourMovementType::Sprint;

	const float ForwardInput = GetForwardInput();
	
	if (ForwardInput > 0.0f && bStateAble && CharacterMovementComp->IsFalling())
	{
		bVerticalWallRun = true;
	}

	return  bVerticalWallRun;
}

float UParkourMovementComp::GetForwardInput() const
{
	const FVector ActorForwardVector = MyOwner->GetActorForwardVector();

	const FVector LastMovementInput =  MyOwner->GetOwnerLastInputVector();
	
	return FVector::DotProduct(	ActorForwardVector,LastMovementInput);
}

void UParkourMovementComp::GetMantleVector(FVector& EyesVector, FVector& FeetVector) const
{
	FVector EyesLoc = FVector(0.0f);
	FRotator EyesRot = FRotator(0.0f);

	MyOwner->GetActorEyesViewPoint(EyesLoc,EyesRot);

	const FVector UpVector = MyOwner->GetActorUpVector() * (-LedgeGrabHeightOffset);
 	const FVector ForwardVector = MyOwner->GetActorForwardVector() * 50.0f;

	//Promote EyesVector
	EyesVector = EyesLoc + ForwardVector + UpVector;
	if (bUseDebug)
	{
		DrawDebugSphere(GetWorld(),EyesVector,12.0f,12,FColor::Red,false,5.0f);
	}
	
	const float FeetHeight = MyOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - MantleHeight;

	//Promote Feet Vector
	FeetVector = (MyOwner->GetActorLocation() - FVector(0,0,FeetHeight)) + ForwardVector;
}

bool UParkourMovementComp::CanMantle() const
{
	const bool IsForwarding = GetForwardInput() > 0.0f;
	
	const bool IsMantleMode = CurrentParkourMovement == EParkourMovementType::LedgeGrab || IsQuickMantle();

	return IsForwarding && IsMantleMode;
}

bool UParkourMovementComp::IsQuickMantle() const
{
	return MantleTraceDistance > MyOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

FVector UParkourMovementComp::GetSlideVector() const
{
	FVector SlideVector = FVector(0.0f);
	
	const FVector StartPos = MyOwner->GetActorLocation();

	const FVector Direction = MyOwner->GetActorUpVector() * -200.0f;
		
	const FVector EndPos = StartPos * Direction;

	FHitResult Hit;

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartPos, EndPos, ECC_Visibility))
	{
		SlideVector = FVector::CrossProduct(Hit.ImpactNormal,MyOwner->GetActorRightVector());
		SlideVector *= -1;
	}
	
	return SlideVector;
}

void UParkourMovementComp::Initialize()
{
	if (GetOwner())
	{
		MyOwner = Cast<AApexCharacterBase>(GetOwner());
		if (MyOwner)
		{
			CharacterMovementComp = MyOwner->GetCharacterMovement();

			DefaultGroundFriction = CharacterMovementComp->GroundFriction;
			DefaultBrakingDecelerationWalking = CharacterMovementComp->BrakingDecelerationWalking;

			DefaultWalkSpeed = CharacterMovementComp->MaxWalkSpeed;
			DefaultCrouchSpeed = CharacterMovementComp->MaxWalkSpeedCrouched;
		}
	}
}

void UParkourMovementComp::PlayCameraShake_Implementation(TSubclassOf<UCameraShakeBase> CameraShake)
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(),CameraShake,MyOwner->GetActorLocation(),0.0f,100.0f);
	
}

void UParkourMovementComp::OnMovementChanged(const EMovementMode& PrevMoveMode,
	const EMovementMode& NewMoveMode)
{
	if (CharacterMovementComp)
	{
		PrevMovementMode = PrevMoveMode;
		CurrentMovementMode = NewMoveMode;
	}

	if (PrevMovementMode == EMovementMode::MOVE_Walking && CurrentMovementMode == EMovementMode::MOVE_Falling)
	{
		JumpEvent();
		EndAllEvents();
		OpenSlideGate();
	}

	if (PrevMovementMode == EMovementMode::MOVE_Falling && CurrentMovementMode == EMovementMode::MOVE_Walking)
	{
		OnLand();
		CheckQueues();
	}
}

void UParkourMovementComp::ParkourMovementChanged(const EParkourMovementType& PrevParkourMove,
													const EParkourMovementType& NewParkourMove)
{
	PrevParkourMovement = PrevParkourMove;
	CurrentParkourMovement = NewParkourMove;
	ResetMovement();
}

bool UParkourMovementComp::SetParkourMovementMode(EParkourMovementType NewParkourMode)
{
	bool Changed = false;

	if (NewParkourMode != CurrentParkourMovement)
	{
		ParkourMovementChanged(CurrentParkourMovement,NewParkourMode);
		Changed = true;
	}

	return Changed;
}

bool UParkourMovementComp::ParkourCancel()
{
	if (CurrentParkourMovement == EParkourMovementType::Mantle || CurrentParkourMovement == EParkourMovementType::VerticalWallRun || CurrentParkourMovement == EParkourMovementType::LedgeGrab)
	{
		VerticalWallRunEnd(0.5f);

		return true;
	}

	return false;
}

void UParkourMovementComp::ResetMovement()
{
	if (CurrentParkourMovement == EParkourMovementType::None || CurrentParkourMovement == EParkourMovementType::Crouch)
	{
		EMovementMode NewMovementMode = EMovementMode::MOVE_None;

		switch (CurrentParkourMovement)
		{
		case EParkourMovementType::None:
			NewMovementMode = EMovementMode::MOVE_Walking;
			break;
		case EParkourMovementType::VerticalWallRun:
			NewMovementMode = EMovementMode::MOVE_Falling;
			break;
		case EParkourMovementType::LedgeGrab:
			NewMovementMode = EMovementMode::MOVE_Falling;
			break;
		case EParkourMovementType::Mantle:
			NewMovementMode = EMovementMode::MOVE_Walking;
			break;
		case EParkourMovementType::Slide:
			NewMovementMode = EMovementMode::MOVE_Walking;
			break;
		case EParkourMovementType::Sprint:
			NewMovementMode = EMovementMode::MOVE_Walking;
			break;
		case EParkourMovementType::Crouch:
			NewMovementMode = EMovementMode::MOVE_Walking;
			break;
		}

		CharacterMovementComp->SetPlaneConstraintEnabled(false);
		
		CharacterMovementComp->GravityScale = 1.0f;
		CharacterMovementComp->SetMovementMode(NewMovementMode);
		CharacterMovementComp->GroundFriction = DefaultGroundFriction;
		CharacterMovementComp->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
		CharacterMovementComp->MaxWalkSpeed = DefaultWalkSpeed;
		CharacterMovementComp->MaxWalkSpeedCrouched = DefaultCrouchSpeed;

	}
}

void UParkourMovementComp::LedgeGrabJump()
{
	if (CurrentParkourMovement == EParkourMovementType::LedgeGrab || CurrentParkourMovement == EParkourMovementType::VerticalWallRun || CurrentParkourMovement == EParkourMovementType::Mantle)
	{
		UFunctionLibraryCommon::UnClampPlayerYawView(GetOwner()->GetInstigatorController());

		VerticalWallRunEnd(0.35f);

		//Make Character Jump Backward

		const FVector LaunchVelocity = FVector(VerticalWallRunNormal.X * LedgeGrabJumpOffForce, VerticalWallRunNormal.Y * LedgeGrabJumpOffForce,LedgeGrabJumpHeight);	
		MyOwner->LaunchCharacter(LaunchVelocity,false,true);
		
	}
	
}

void UParkourMovementComp::MantleCheck()
{
	if (CanMantle())
	{
		MantleStart();
	}
}

void UParkourMovementComp::MantleStart()
{
	if (SetParkourMovementMode(EParkourMovementType::Mantle))
	{
		const TSubclassOf<UCameraShakeBase> CameraShakeToPlay = IsQuickMantle() ? CameraShake_QuickMantle : CameraShake_Mantle;
		PlayCameraShake(CameraShakeToPlay);
		CloseMantleCheckGate();
		OpenMantleGate();

		UFunctionLibraryCommon::UnClampPlayerYawView(GetOwner()->GetInstigatorController());
	}
	
}

void UParkourMovementComp::MantleMovement()
{
	FVector StartPos = MyOwner->GetActorLocation();
	StartPos.Z = 0;

	FVector TargetLoc = MantlePosition;
	TargetLoc.Z = 0;

	//Lerp to new pos
	const float InterpSpeed = IsQuickMantle() ? QuickMantleInterpSpeed : DefaultMantleInterpSpeed;
	const FVector NewPos = FMath::VInterpTo(MyOwner->GetActorLocation(),MantlePosition,DeltaSeconds,InterpSpeed);
	MyOwner->SetActorLocation(NewPos);


	if (FVector::Dist(MyOwner->GetActorLocation(),MantlePosition) < 8.0f)
	{
		VerticalWallRunEnd(0.5f);
	}
	
}

void UParkourMovementComp::VerticalWallRunMovement()
{
	FVector EyesLoc = FVector(0.0f);
	FVector FeetLoc = FVector(0.0f);

	GetMantleVector(EyesLoc,FeetLoc);

	const FVector EndVector = FeetLoc + ( MyOwner->GetActorForwardVector() * 50.0f );

	FHitResult Hit;

	//If there is something to climb and holding forward input
	if (GetWorld()->LineTraceSingleByChannel(Hit,FeetLoc,EndVector,ECC_Visibility) && GetForwardInput() > 0.0f)
	{
		SetParkourMovementMode(EParkourMovementType::VerticalWallRun);

		//Push Up Character

		//Force to stick wall
		VerticalWallRunNormal = Hit.ImpactNormal;

		//Make it climb more when is about to Exhausted
		const float TimeSinceClimbing = GetWorld()->TimeSeconds - VerticalGateOpenedTime;
		if (bUseDebug)
		{
			UFunctionLibraryCommon::DebugMessage(EDebugType::Debug,FString::SanitizeFloat(TimeSinceClimbing),0.1f);
		}
		if (TimeSinceClimbing >= VerticalWallRunDuration)
		{
			VerticalWallRunEnd(2.0f);
		}
		const float VelocityZ = VerticalWallRunSpeed * TimeSinceClimbing;

		
		const FVector LaunchVelocity = FVector(VerticalWallRunNormal.X * -600,VerticalWallRunNormal.Y * -600,VerticalWallRunSpeed);
		
		MyOwner->LaunchCharacter(LaunchVelocity,true,true);
	}
	else //Nothing to climb on
 	{
		VerticalWallRunEnd(0.35f);
	}
}

void UParkourMovementComp::VerticalWallRunUpdate()
{
	if (CanVerticalWallRun())
	{
		FHitResult Hit;
		FVector EyesVec = FVector(0.0f);
		FVector FeetVec = FVector(0.0f);

		GetMantleVector(EyesVec,FeetVec);

		const FCollisionShape CapsuleCollision = FCollisionShape::MakeCapsule(20.0f,10.0f);

		if (GetWorld()->SweepSingleByChannel(Hit,EyesVec,FeetVec,FQuat::Identity,ECC_Visibility,CapsuleCollision))
		{
			MantleTraceDistance = Hit.Distance;

			//when climb to a ledge
			if (CharacterMovementComp->IsWalkable(Hit))
			{
				MantlePosition = Hit.ImpactPoint + FVector(0,0,MyOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + LedgeGrabHeightOffset);

				if (bUseDebug)
				{
					DrawDebugSphere(GetWorld(),MantlePosition,12.0f,12.0f,FColor::Green,false,2.0f);
					DrawDebugSphere(GetWorld(),Hit.ImpactPoint,12.0f,12.0f,FColor::Green,false,2.0f);
				}
				
				CloseVerticalRunGate();

				if(SetParkourMovementMode(EParkourMovementType::LedgeGrab))
				{
					UFunctionLibraryCommon::ClampPlayerYawViewAngle(GetOwner()->GetInstigatorController(),LedgeGrabViewAngle);

					CharacterMovementComp->DisableMovement();
					CharacterMovementComp->StopMovementImmediately();
					CharacterMovementComp->GravityScale = 0.0f;
					PlayCameraShake(CameraShake_Jump);

					if (IsQuickMantle())
					{
						OpenMantleCheckGate();
					}
					else
					{
						FTimerHandle TempHandle;
						GetWorld()->GetTimerManager().SetTimer(TempHandle,this,&UParkourMovementComp::OpenMantleCheckGate,0.25f,false);
					}
				}
			}
			else
			{
				VerticalWallRunMovement();
			}
		}
		else
		{
			VerticalWallRunMovement();
		}
	}
	else
	{
		VerticalWallRunEnd(0.35f);
	}
}

void UParkourMovementComp::VerticalWallRunEnd(float Duration)
{
	if (CurrentParkourMovement == EParkourMovementType::VerticalWallRun || CurrentParkourMovement == EParkourMovementType::LedgeGrab || CurrentParkourMovement == EParkourMovementType::Mantle)
	{
		SetParkourMovementMode(EParkourMovementType::None);

		CloseVerticalRunGate();
		CloseMantleCheckGate();

		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle,this,&UParkourMovementComp::OpenVerticalRunGate,Duration,false);
	}
}



void UParkourMovementComp::OpenGates()
{
	OpenVerticalRunGate();
	OpenSlideGate();
}

void UParkourMovementComp::CloseGates()
{
	CloseVerticalRunGate();

	if (CurrentParkourMovement != EParkourMovementType::Crouch)
	{
		CloseSlideGate();
		ResetMovement();
	}
}

bool UParkourMovementComp::IsSprinted() const
{
	return CurrentParkourMovement == EParkourMovementType::Sprint && GetOwner()->GetVelocity().Size() > 1.0f;
}

void UParkourMovementComp::SprintJump()
{
	if (CurrentParkourMovement == EParkourMovementType::Sprint)
	{
		bSprintQueued = true;	
	}
}

bool UParkourMovementComp::SprintStart()
{
	CrouchEnd();
	SlidedEnd();

	if (CurrentParkourMovement == EParkourMovementType::None && CharacterMovementComp->IsWalking())
	{
		if (SetParkourMovementMode(EParkourMovementType::Sprint))
		{
			CharacterMovementComp->MaxWalkSpeed = SprintSpeed;

			bSprintQueued = false;
			return true;
		}
	}
	else
	{
		bSlideQueued = true;
	}

	return false;
}

void UParkourMovementComp::SprintEnd()
{
	if (CurrentParkourMovement == EParkourMovementType::Sprint && CharacterMovementComp->IsWalking())
	{
		if (SetParkourMovementMode(EParkourMovementType::None))
		{
			CharacterMovementComp->MaxWalkSpeed = DefaultWalkSpeed;
		}
	}

	bSprintQueued = false;
}

void UParkourMovementComp::CrouchEvent()
{
	//Parkour Cancel not success meaning not in parkour mode
	if (!ParkourCancel() || CurrentParkourMovement == EParkourMovementType::Sprint)
	{
		if (bUseDebug)
		{
			UFunctionLibraryCommon::DebugMessage(EDebugType::Debug,FString::SanitizeFloat(MyOwner->GetOwnerLastInputVector().Size()),2.0f);
		}
		
		
		if (CanSlide())
		{
			if (CharacterMovementComp->IsWalking())
			{
				SlideStart();
			}
			else
			{
				bSlideQueued = true;
			}
		}
		else
		{
			switch (CurrentParkourMovement)
			{
			case EParkourMovementType::None:
				CrouchStart();
				break;
			case EParkourMovementType::VerticalWallRun:
				break;
			case EParkourMovementType::LedgeGrab:
				break;
			case EParkourMovementType::Mantle:
				break;
			case EParkourMovementType::Slide:
				CrouchEnd();
				break;
			case EParkourMovementType::Sprint:
				CrouchStart();
				break;
			case EParkourMovementType::Crouch:
				CrouchEnd();
				break;
			}
		}
	}
}

void UParkourMovementComp::CrouchStart()
{
	if (CurrentParkourMovement == EParkourMovementType::None)
	{
		MyOwner->Crouch(true);
		SetParkourMovementMode(EParkourMovementType::Crouch);
		bSprintQueued = false;
		bSlideQueued = false;
	}
}

void UParkourMovementComp::CrouchEnd()
{
	if (CurrentParkourMovement == EParkourMovementType::Slide || CurrentParkourMovement == EParkourMovementType::Crouch)
	{
		MyOwner->UnCrouch(true);
		SetParkourMovementMode(EParkourMovementType::None);
		SlidedEnd();
		bSprintQueued = false;
	}
}

bool UParkourMovementComp::CanSlide() const
{
	if (!CharacterMovementComp)
	{
		return false;
	}
	const bool bMoving = MyOwner->GetOwnerLastInputVector().Size() > 0.0f;
	const bool bSprinting = CurrentParkourMovement == EParkourMovementType::Sprint || bSprintQueued;

	return bMoving && bSprinting;
}

void UParkourMovementComp::SlideStart()
{
	if (!CharacterMovementComp)
	{
		return;
	}
	
	if (CharacterMovementComp->IsWalking() || CurrentParkourMovement == EParkourMovementType::Sprint)
	{
		SprintEnd();
		SetParkourMovementMode(EParkourMovementType::Slide);
		MyOwner->Crouch(true);

		
		CharacterMovementComp->GroundFriction = 0.0f;
		CharacterMovementComp->BrakingDecelerationWalking = 1400.0f;
		CharacterMovementComp->MaxWalkSpeedCrouched = 0.0f;

		FVector VelocityNormal = CharacterMovementComp->Velocity;
		VelocityNormal.Normalize();
		
		CharacterMovementComp->SetPlaneConstraintFromVectors(VelocityNormal,MyOwner->GetActorUpVector());
		CharacterMovementComp->SetPlaneConstraintEnabled(true);

		const FVector SlideVector = GetSlideVector();

		CharacterMovementComp->AddImpulse(SlideVector * SlideStartImpulseAmount,true);
		
		OpenSlideGate();
	}
}

void UParkourMovementComp::SlideUpdate()
{
	if (CurrentParkourMovement == EParkourMovementType::Slide)
	{
		const FVector SlideVector = GetSlideVector();

		if (SlideVector.Z <= -0.05f)
		{
			CharacterMovementComp->AddImpulse(SlideVector * SlideImpulseAmount,true);
			if (bUseDebug)
			{
				UFunctionLibraryCommon::DebugMessage(EDebugType::Debug,FString::SanitizeFloat(SlideVector.Z),0.2f);
			}
		}
		
		//Stop Sliding
		if (CharacterMovementComp->Velocity.Size() <= 35.0f)
		{
			SlidedEnd(true);
		}	
	}
}

void UParkourMovementComp::SlidedEnd(bool Crouch)
{
	if (CurrentParkourMovement == EParkourMovementType::Slide)
	{
		const EParkourMovementType NewParkourType = Crouch ? EParkourMovementType::Crouch : EParkourMovementType::None;
		if (SetParkourMovementMode(NewParkourType))
		{
			//If dont want crouch anymore
			if (!Crouch)
			{
				MyOwner->UnCrouch(true);
				bSlideQueued = false;
				CloseSlideGate();
			}
		}
	} 
}

void UParkourMovementComp::SlideJump()
{
	if (CurrentParkourMovement == EParkourMovementType::Slide)
	{
		SlidedEnd();
		MyOwner->Jump();
	}	
}




void UParkourMovementComp::OnLand()
{
	VerticalWallRunEnd(0.0f);
	CloseGates();
	PlayCameraShake(CameraShake_JumpLand);
}

void UParkourMovementComp::JumpEvent()
{
	if (CurrentParkourMovement == EParkourMovementType::None || CurrentParkourMovement == EParkourMovementType::Sprint)
	{
 		if (CurrentMovementMode != MOVE_Falling)
		{
			OpenGates();
			PlayCameraShake(CameraShake_Jump);
		}
	}
	else if(CurrentParkourMovement == EParkourMovementType::Crouch)
	{
		CrouchEnd();
	}
	else
	{
		//Those Jump Event will check state inside
		LedgeGrabJump();
		SlideJump();
		SprintJump();
	}
}

void UParkourMovementComp::EndAllEvents()
{
	VerticalWallRunEnd(0.0f);
	SprintEnd();
	SlidedEnd();
}

void UParkourMovementComp::CheckQueues()
{
	if (bSlideQueued)
	{
		SlideStart();
		bSlideQueued = false;
	}

	if (bSprintQueued)
	{
		SprintStart();
	}
}

void UParkourMovementComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
	//DOREPLIFETIME(UParkourMovementComp,CurrentMovementMode);
	//DOREPLIFETIME(UParkourMovementComp,PrevMovementMode);
	
	//DOREPLIFETIME(UParkourMovementComp,CurrentParkourMovement);
	//DOREPLIFETIME(UParkourMovementComp,PrevParkourMovement);
	
}