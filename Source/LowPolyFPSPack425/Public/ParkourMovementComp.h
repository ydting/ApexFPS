// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourMovementComp.generated.h"

class UCameraShakeBase;

UENUM(BlueprintType)
enum class EParkourMovementType : uint8
{
	None,
	VerticalWallRun,
	LedgeGrab,
	Mantle,
	Slide,
	Sprint,
	Crouch
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWPOLYFPSPACK425_API UParkourMovementComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UParkourMovementComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const override;
	
	//Getters
	bool CanVerticalWallRun() const;

	float GetForwardInput() const;

	void GetMantleVector(FVector& EyesVector,FVector& FeetVector) const;
	
	bool CanMantle() const;

	bool IsQuickMantle() const;

	FVector GetSlideVector() const;
	
	UFUNCTION(BlueprintCallable)
		void Initialize();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable,Server,Reliable)
		void PlayCameraShake(TSubclassOf<UCameraShakeBase> CameraShake);

	UFUNCTION(BlueprintCallable)
		EParkourMovementType GetParkourMovementType() const { return this->CurrentParkourMovement;}
	
	UFUNCTION(BlueprintCallable)
		void OnMovementChanged(const EMovementMode& PrevMoveMode,const EMovementMode& NewMoveMode);
	
	UFUNCTION(BlueprintCallable)
		void ParkourMovementChanged(const EParkourMovementType& PrevParkourMove,const EParkourMovementType& NewParkourMove);

	bool SetParkourMovementMode(EParkourMovementType NewParkourMode);

	//return value = Got Parkour Cancel?
	bool ParkourCancel();
	
	void ResetMovement();

	void LedgeGrabJump();
	
	void MantleCheck();
	void MantleStart();
	void MantleMovement();

	void VerticalWallRunMovement();
	void VerticalWallRunUpdate();
	void VerticalWallRunEnd(float Duration);

		
	//===============================================================

	void OpenSlideGate() {bSlideGateOpened = true;}
	void CloseSlideGate() {bSlideGateOpened = false;}
	
	void OpenMantleCheckGate() { bMantleCheckGateOpened = true;}
	void CloseMantleCheckGate() { bMantleCheckGateOpened = false;}
	
	void OpenMantleGate() { bMantleGateOpened = true;}
	void CloseMantleGate() { bMantleGateOpened = false;}
	
	void OpenVerticalRunGate()
	{
		bVerticalWallRunGateOpened = true;
		CloseMantleGate();
		VerticalGateOpenedTime = GetWorld()->TimeSeconds;
	}
	void CloseVerticalRunGate() { bVerticalWallRunGateOpened = false;}

	void OpenGates();
	void CloseGates();


	//===============================================================

	UFUNCTION(BlueprintCallable)
	bool IsSprinted() const;
	void SprintJump();
	bool SprintStart();
	void SprintEnd();

	void CrouchEvent();


	void CrouchStart();
	void CrouchEnd();

	bool CanSlide() const;
	void SlideStart();
	void SlideUpdate();
	void SlidedEnd(bool Crouch = false);
	void SlideJump();

	void HealingStart() const { CharacterMovementComp->MaxWalkSpeed = DefaultCrouchSpeed;}
	void HealingEnd( ) const { CharacterMovementComp->MaxWalkSpeed = DefaultWalkSpeed;}
;	//===============================================================

	
	void OnLand();
	void JumpEvent();

	void EndAllEvents();

	void CheckQueues();
	
public:

	class AApexCharacterBase* MyOwner;

	class UCharacterMovementComponent* CharacterMovementComp;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		bool bUseCameraShake;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
	float DefaultMantleInterpSpeed;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
	float  QuickMantleInterpSpeed;
	
	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float MantleHeight;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float VerticalWallRunSpeed;

	/**
	 * @brief How long can vertical wall run
	 */
	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float VerticalWallRunDuration;
	
	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float LedgeGrabJumpOffForce;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float LedgeGrabJumpHeight;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		//This value more higher will make camera pos at ledgeGrab more higher
		float LedgeGrabHeightOffset;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float LedgeGrabViewAngle;
	
	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float SprintSpeed;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float SlideImpulseAmount;

	UPROPERTY(Category = "Parkour", EditDefaultsOnly, BlueprintReadWrite)
		float SlideStartImpulseAmount;
	
	UPROPERTY(Category = "CameraShake", EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UCameraShakeBase> CameraShake_LedgeGrab;

	UPROPERTY(Category = "CameraShake", EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UCameraShakeBase> CameraShake_JumpLand;

	UPROPERTY(Category = "CameraShake", EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UCameraShakeBase> CameraShake_Jump;

	UPROPERTY(Category = "CameraShake", EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UCameraShakeBase> CameraShake_QuickMantle;

	UPROPERTY(Category = "CameraShake", EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UCameraShakeBase> CameraShake_Mantle;

	UPROPERTY(Category = "Parkour|Debug", EditDefaultsOnly, BlueprintReadWrite)
		bool bUseDebug;

	
protected:

	//UPROPERTY(Replicated)
	TEnumAsByte<EMovementMode> PrevMovementMode;

	//UPROPERTY(Replicated)
	TEnumAsByte<EMovementMode> CurrentMovementMode;

	//UPROPERTY(Replicated)
	EParkourMovementType PrevParkourMovement;

	//UPROPERTY(Replicated)
	EParkourMovementType CurrentParkourMovement;

	bool bMantleCheckGateOpened;
	bool bMantleGateOpened;

	bool bVerticalWallRunGateOpened;
	float VerticalGateOpenedTime;

	bool bSlideQueued;
	bool bSprintQueued;

	FVector VerticalWallRunNormal;
	
	float MantleTraceDistance;
	FVector MantlePosition;

	bool bSlideGateOpened;
	
	float DeltaSeconds;

	float DefaultGroundFriction;
	float DefaultBrakingDecelerationWalking;
	float DefaultWalkSpeed;
	float DefaultCrouchSpeed;
};
