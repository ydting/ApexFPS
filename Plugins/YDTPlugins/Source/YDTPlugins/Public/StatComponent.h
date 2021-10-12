// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Headers.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnModifyValuedSignature ,float,NewCurrentValue,float,NewPercentageOfValue,bool,IsEmptyValue,bool,IsFullValue);



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YDTPLUGINS_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief Modify CurrentStatValue
	 * @param Value TheValue to add,if want to Subtract StatValue put a negative value into 
	 * @return after modified current value
	 */
	UFUNCTION(Category="StatsComponents",BlueprintCallable)
	float ModifyValue(const float Value);

	UFUNCTION(Category="StatsComponents",BlueprintCallable)
		FStatValue GetStatValue() const { return this->StatValue;}

	UFUNCTION(Category="StatsComponents",BlueprintCallable)
		float GetCurrentValue() const { return this->StatValue.CurrentValue;}

	UFUNCTION(Category="StatsComponents",BlueprintCallable)
		float GetCurrentValuePercentage() const ;
	
	UFUNCTION(Category="StatsComponents",BlueprintCallable)
		void SetStatValue(const FStatValue& NewStatValue) { this->StatValue = NewStatValue;}

	UFUNCTION(Category="StatsComponents",BlueprintCallable)
		void SetStatNewMinValue(const float NewMinValue) { this->StatValue.MinValue = NewMinValue;}

	UFUNCTION(Category="StatsComponents",BlueprintCallable)
		void SetStatNewMaxValue(const float NewMaxValue) { this->StatValue.MaxValue = NewMaxValue;}

	UFUNCTION(Category="StatsComponents",BlueprintCallable)
		bool IsFullValue() const { return bIsFullValue;}
	
	UFUNCTION(Category="StatsComponents",BlueprintCallable)
		bool IsEmptyValue() const { return bIsEmptyValue;}

protected:

	void RegenerateStat();

protected:
	UPROPERTY(Category="StatSettings",BlueprintAssignable)
		FOnModifyValuedSignature OnModifyValued;
	
	UPROPERTY(Category="StatsSettings",EditDefaultsOnly)
		EStatsType Stat;

	UPROPERTY(Replicated,Category="StatsSettings",EditDefaultsOnly,Replicated)	
		FStatValue StatValue;

	UPROPERTY(Category="StatsSettings|RegenerationSystem",EditDefaultsOnly)	
		bool bCanRegenerate;

	UPROPERTY(Category="StatsSettings|RegenerationSystem",EditDefaultsOnly)	
		float PerTickRegenValue;
	
	UPROPERTY(Category="StatsSettings|RegenerationSystem",EditDefaultsOnly)	
		float RegenSystemTickInterval;
	
	/**
	 * @brief Stop Regen System When Modify Value is negative and Re Enable After Interval
	 */
	UPROPERTY(Category="StatsSettings|RegenerationSystem",EditDefaultsOnly)	
		float ReEnableRegenSystemInterval;

	/**
    * @brief If CurrentValue same as MaximumValue will set to true
    */
	UPROPERTY(Replicated)
	bool bIsFullValue;

	/**
	* @brief If CurrentValue same as MinimumValue will set to true
	*/
	UPROPERTY(Replicated)
	bool bIsEmptyValue;

	FTimerHandle RegenSystemTimer;
};
