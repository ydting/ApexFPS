// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);


	//To Initialize StatValue
	ModifyValue(0.0f);

	
	if (bCanRegenerate && !bIsFullValue)
	{
		GetWorld()->GetTimerManager().SetTimer(RegenSystemTimer,this,&UStatComponent::RegenerateStat,RegenSystemTickInterval
																						,true,ReEnableRegenSystemInterval);
	}
}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UStatComponent::ModifyValue(const float Value)
{
	if (FMath::IsNegativeFloat(Value))
	{
		GetWorld()->GetTimerManager().SetTimer(RegenSystemTimer,this,&UStatComponent::RegenerateStat,RegenSystemTickInterval
																						,true,ReEnableRegenSystemInterval);
	}
	
	StatValue.CurrentValue += Value;

	if (StatValue.CurrentValue <= StatValue.MinValue)
	{
		StatValue.CurrentValue = StatValue.MinValue;
		bIsFullValue = false;
		bIsEmptyValue = true;
		
		OnModifyValued.Broadcast(StatValue.CurrentValue,GetCurrentValuePercentage(),bIsEmptyValue,bIsFullValue);
		return StatValue.CurrentValue;
	}

	if (StatValue.CurrentValue >= StatValue.MaxValue)
	{
		StatValue.CurrentValue = StatValue.MaxValue;
		bIsFullValue = true;
		bIsEmptyValue = false;

		//if MAXIMUM VALUE stop regenerate system
		GetWorld()->GetTimerManager().ClearTimer(RegenSystemTimer);

		OnModifyValued.Broadcast(StatValue.CurrentValue,GetCurrentValuePercentage(),bIsEmptyValue,bIsFullValue);
		return StatValue.CurrentValue;
	}
	
	bIsFullValue = false;
	bIsEmptyValue = false;

	OnModifyValued.Broadcast(StatValue.CurrentValue,GetCurrentValuePercentage(),bIsEmptyValue,bIsFullValue);
	return StatValue.CurrentValue;
}

float UStatComponent::GetCurrentValuePercentage() const
{
	const float TotalValue = StatValue.MaxValue + FMath::Abs(StatValue.MinValue);

	return StatValue.CurrentValue / TotalValue;
}

void UStatComponent::RegenerateStat()
{
	ModifyValue(PerTickRegenValue);
}

void UStatComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatComponent,StatValue);
	DOREPLIFETIME(UStatComponent,bIsEmptyValue);
	DOREPLIFETIME(UStatComponent,bIsFullValue);
	DOREPLIFETIME(UStatComponent,StatValue);
	
}
