// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/NavMovementComponent.h"
#include "TankMovementComponent.generated.h"

class UTankTrack;

/**
 * Responsible for diriving the tank tracks
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class BATTLETANK_API UTankMovementComponent : public UNavMovementComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Setup)
		void Initialize(UTankTrack* LeftTrackToSet, UTankTrack* RightTrack);
	
	UFUNCTION(BlueprintCallable, Category = Input)
		void IntendMoveFoward(float Throw);
	UFUNCTION(BlueprintCallable, Category = Input)
		void IntendTurnRight(float Throw);
	UFUNCTION(BlueprintCallable, Category = Input)
		void IntendTurnLeft(float Throw);



private:
	UTankTrack* LeftTrack = nullptr;
	UTankTrack* RightTrack = nullptr;
	
	
};
