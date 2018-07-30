// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TankAimingComponent.generated.h"

UENUM()
enum class EFiringState : uint8 {
	Reloading,
	Aiming,
	Locked,
	OutOfAmmo
};

class UTankBarrel;
class UTankTurret;
class AProjectile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BATTLETANK_API UTankAimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTankAimingComponent();
	UFUNCTION(BlueprintCallable, Category = "Setup")
		void Initialize(UTankBarrel* BarrelToSet, UTankTurret* TurretToSet);
	void AimAt(FVector HitLocation);
	UFUNCTION(BlueprintCallable, Category = "Firing")
		void Fire();
	EFiringState GetFiringState() const;
	UFUNCTION(BlueprintCallable, Category = "Firing")
		int GetRoundsLeft() const;
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(BlueprintReadOnly, Category = "State")
		EFiringState FiringState = EFiringState::Aiming;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		float LaunchSpeed = 4000;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		TSubclassOf<AProjectile> ProjectileBlueprint;
	UTankBarrel* Barrel = nullptr;
	UTankTurret* Turret = nullptr;
	void MoveBarrelTowards(FVector AimDirection);
	bool IsBarrelMoving();
	FVector AimDirection;
	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		int32 MaxRounds = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		int32 RoundsLeft = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		double ResetAmmoTimeInSeconds = 6;
	UPROPERTY(EditDefaultsOnly, Category = "Firing")
		float ReloadTimeInSeconds = 3;
	double LastFireTime = 0;
	double LastAddAmmoTime = 0;
	};
