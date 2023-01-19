// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallActor.generated.h"

class UARPin;

//! @brief Used for inheritence to create a data only blueprint of a ball.

UCLASS()
class UE5_AR_API ABallActor : public AActor
{
	GENERATED_BODY()
	
public:	
	//! @brief Sets the static mesh and trigger sphere for the ball
	ABallActor();

protected:
	
	// Called when the game starts or when spawned
	//!Plays shoot sound when created
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//! @brief Checks if the ball falls off the level if so destroy actor.
	//! Destroys ball after being spawned for a set time
	//! @param DelataTime - used for timers
	virtual void Tick(float DeltaTime) override;
	
	void SetPin(UARPin* PinComponent) {ZonePinComponent = PinComponent;};
		
	// Used to simulate physics
	UPROPERTY(Category = "Ball Info", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

	//Used to simulate collision
	UPROPERTY(Category = "Ball Info", EditAnywhere, BlueprintReadWrite)
	class USphereComponent* TriggerSphere;

	//Used to set the trigger sphere size
	UPROPERTY(Category = "Ball Info", VisibleAnywhere, BlueprintReadWrite)
	float SphereBound = 55;

	//Set amount of time ball can be alive for
	UPROPERTY(Category = "Ball Info", VisibleAnywhere, BlueprintReadWrite)
	float DespawnTime = 1.5;
	
	UPROPERTY(Category = "Ball Info", EditAnywhere, BlueprintReadWrite)
	USoundBase* ShootSound;

	//Used to track ball position against the zone for de-spawning
	UARPin* ZonePinComponent;

	float timer = 0;

};
