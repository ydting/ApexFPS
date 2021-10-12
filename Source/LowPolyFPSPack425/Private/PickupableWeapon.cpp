// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupableWeapon.h"
#include "Components/BoxComponent.h"

#include "ApexCharacterBase.h"
#include "WeaponBase.h"

// Sets default values
APickupableWeapon::APickupableWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void APickupableWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupableWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupableWeapon::OnInteract_Implementation(AActor* InteractBy)
{
	if (WeaponClass)
	{
		if (AApexCharacterBase* MyOwner = Cast<AApexCharacterBase>(InteractBy))
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = InteractBy;
			
			AWeaponBase* WeaponInstance = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass,SpawnParameters);

			WeaponInstance->SetRemainingAmmo(RemainingAmmoInWeapon);

			MyOwner->OnPickupWeapon(WeaponInstance);

			this->Destroy();
		}
	}
}

