// Fill out your copyright notice in the Description page of Project Settings.

#include "TankAimingComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TankBarrel.h"
#include "Engine/World.h"
#include "TankTurret.h"
#include "Projectile.h"

// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

void UTankAimingComponent::BeginPlay() {
	Super::BeginPlay();
	LastFireTime = FPlatformTime::Seconds();
	RoundsLeft = MaxRounds;
}

void UTankAimingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Checks if ResetAmmoTime has passed since the (last fire or the last add of ammo) to add another round
	if((FPlatformTime::Seconds() - LastFireTime) > ResetAmmoTimeInSeconds && (FPlatformTime::Seconds() - LastAddAmmoTime) > ResetAmmoTimeInSeconds) {
		RoundsLeft++;  
		RoundsLeft = FMath::Clamp<double>(RoundsLeft, 0, MaxRounds); 
		LastAddAmmoTime = FPlatformTime::Seconds();
	} 
	if(RoundsLeft <= 0) {
		FiringState = EFiringState::OutOfAmmo;
	} else if((FPlatformTime::Seconds() - LastFireTime) < ReloadTimeInSeconds) {
		FiringState = EFiringState::Reloading;
	} else if(IsBarrelMoving()) {
		FiringState = EFiringState::Aiming;
	} else {
		FiringState = EFiringState::Locked;
	}
}

EFiringState UTankAimingComponent::GetFiringState() const {
	return FiringState;
}

int UTankAimingComponent::GetRoundsLeft() const {
	return RoundsLeft;
}

bool UTankAimingComponent::IsBarrelMoving() {
	if(!ensure(Barrel)) {return false;}
	return !(Barrel->GetForwardVector().Equals(AimDirection, .01));
}

void UTankAimingComponent::Initialize(UTankBarrel* BarrelToSet, UTankTurret* TurretToSet) {
	if(!ensure(BarrelToSet)) {return;}
	Barrel = BarrelToSet;
	if(!ensure(TurretToSet)) {return;}
	Turret = TurretToSet;
}


void UTankAimingComponent::AimAt(FVector HitLocation) {
	if(!ensure(Barrel)) {return;}
	FVector OutLaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity	
	(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		0,
		0,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);
	if(bHaveAimSolution) {
		AimDirection = OutLaunchVelocity.GetSafeNormal();
		MoveBarrelTowards(AimDirection);
	} /*/else {
		MoveBarrelTowards(GetOwner()->GetActorForwardVector());
	}     Use if you want barrel to return to default   /*/
}

void UTankAimingComponent::MoveBarrelTowards(FVector AimDirection) {
	if(!ensure(Barrel || Turret)) {return;}
	auto BarrelRotator = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = AimDirection.Rotation();
	auto DeltaRotator = AimAsRotator - BarrelRotator;
	
	Barrel->Elevate(DeltaRotator.Pitch);
	if(FMath::Abs(DeltaRotator.Yaw) > 180) {
		DeltaRotator.Yaw *= -1;
	}
	Turret->Rotate(DeltaRotator.Yaw);
}

void UTankAimingComponent::Fire() {
	
	if(FiringState == EFiringState::Locked || FiringState == EFiringState::Aiming) {
		if(!ensure(Barrel)) { return; }
		if(!ProjectileBlueprint) {
			UE_LOG(LogTemp, Warning, TEXT("No projectile blueprint set"));
			return;
		}
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileBlueprint,
			Barrel->GetSocketLocation(FName("Projectile")),
			Barrel->GetSocketRotation(FName("Projectile"))
			);
		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = FPlatformTime::Seconds();
		RoundsLeft--;
	}
}

