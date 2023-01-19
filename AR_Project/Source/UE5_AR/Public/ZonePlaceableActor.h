// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "ZonePlaceableActor.generated.h"

/**
 * 
 */
class UARPin;

UCLASS()

//! @brief Inherited from placeable actor, used to create a data only blueprint of a zone.
class UE5_AR_API AZonePlaceableActor : public APlaceableActor
{
	GENERATED_BODY()
	

protected:
	//! @brief Sets the trigger box for the zone
	AZonePlaceableActor();
	
	// Called when the game starts or when spawned
	//! @brief Sets default zone to be hidden in game, over-ridden from placeable actor
	void BeginPlay() override;

public:	
	// Called every frame
	//! @brief Sets trigger box to track position, tracks zone to pin
	//! @param DeltaTime
	void Tick(float DeltaTime) override;

	//Used to track which level the zone is apart of
	UPROPERTY(Category = "Zone Info", EditAnywhere, BlueprintReadWrite)
	int Level;

	//Used to track how many gem blox are contained in the level
	UPROPERTY(Category = "Zone Info", EditAnywhere, BlueprintReadWrite)
	int GemBloxAmount;

	//Used to set how many throws the player is allowed in the level the zone is apart of
	UPROPERTY(Category = "Zone Info", EditAnywhere, BlueprintReadWrite)
	int LevelThrowAmountLimit;

	//Used for calculating collision with blox
	UPROPERTY(Category = "Zone Info", EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* TriggerBox;
	
	//Used to track the zone to the pins location set by the player
	UARPin* PinComponent;
};
