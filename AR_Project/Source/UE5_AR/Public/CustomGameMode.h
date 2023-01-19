// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ZonePlaceableActor.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

//Forward Declarations
class APlaceableActor;
class AHelloARManager;
class ABloxActor;
class ABallActor;
class UARPin;

//! @brief Controls the game functionality
UCLASS()
class UE5_AR_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:

	APlaceableActor* SpawnedActor;
	AHelloARManager*  ARManager;

public:

	//! @brief Sets up ARPawn and GameState
	ACustomGameMode();
	virtual ~ACustomGameMode() = default;

	//! @brief Fills level maps, then initialises the scenes actors
	virtual void StartPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * @brief Perform a line trace at the screen position provided and place a default Placeable actor on it.
	 * Create another actor class that inherits from APlaceableActor and is spawned here instead of the default one. Make sure this new object is placed on the Plane rather than halfway through a plane
	 * @param ScreenPos Pass the Screen Position as an FVector
	 */
	virtual void LineTraceSpawnActor(FVector ScreenPos);

	//! @brief Sets blox in current level back to original position and set to not interactable.
	//! Sets blox in the new level to the spawned zone position and and is set to interactable.
	//! Resets level counters
	//! @param NewLevelNum - Takes in the level that is wanting to be spawned
	UFUNCTION(BlueprintCallable, Category="UI")
		void SpawnLevel(int NewLevelNum);

	//! @brief Move actors in active level back to original position.
	//! Deletes spawned zone
	//! Starts ar plane tracking
	UFUNCTION(BlueprintCallable, Category="UI")
		void ResetZone();

	UFUNCTION(BlueprintCallable, Category="UI")
		void StartPlaneTracking();

	//! @brief  Spawns 3x3 cube of basic blox
	void SpawnBloxCube();

	//! @brief Deprojects touch position into world space, performs a line trace,
	//! then spawns a ball actor and adds a set force in the direction of the line trace
	//! @param ScreenPos - Takes in screen position of where it was touched.
	void SpawnProjectile(FVector ScreenPos);

	//! @brief Gets blox in level from the map and sets the tracking.
	//! @param BIsBloxTracking - Used to turn blox tracking on and off
	void SetBloxTracking(bool BIsBloxTracking);

	
	void SpawnMainZoneActor(FVector ScreenPos);
	
	//!@brief Perform a line trace at the screen position provided to update the spawned zone actors position
	//!while the user holds their finger on the zone.
	//!@param ScreenPos Pass the Screen Position as an FVector
	void MoveMainZoneActor(FVector ScreenPos);

	//!@brief Uses GetAllActorsOfClass function to get the blox
	//in the scene and sort them by the level they are in
	//! filling the BloxLevelMap
	void FillBloxMapArray();

	//!@brief Uses GetAllActorsOfClass function to get the zones
	//in the scene and sort them by the level they are in
	//! filling the ZoneLevelMap
	void FillZoneMap();

	//!@brief Iterates through the blox and zones in the scene
	//! Sets the blox relative transform and tracking on
	//! Sets the zone actor gem blox amount
	void InitLevelActors();

	//!@brief Checks if all the gem blox in the level have been destroyed, if true BIsWinState = true
	//! Checks if player runs out of throwable balls and not all gem blox are gone, if true BIsLoseState = true
	void CheckWinState(float DeltaTime);
	
	UPROPERTY(Category="Placeable",EditAnywhere,BlueprintReadWrite)
	TSubclassOf<APlaceableActor> ZonePlacableToSpawn;

	UPROPERTY(Category="Placeable",EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ABallActor> Ball;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	int LevelNum = 0;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	int ActiveLevelGemBloxAmount;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	int ActiveLevelGemBloxHit = 0;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	int ActiveLevelThrowAmountLimit;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	int ActiveLevelThrowAmount = 0;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	bool BIsWinState = false;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	bool BIsLoseState = false;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	bool BZoneLocked = false;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	bool BZoneSpawned = false;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	bool BIsInMenu = false;

	UPROPERTY(Category="UI",EditAnywhere,BlueprintReadWrite)
	bool BMute = false;
	
	bool BActiveLevel = false;

	//Used to check lose state after a timer, allows user to win with ball limit reached 
	bool CheckLose = false;

	//Used during check lose state
	float timer = 0;

	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	USoundBase* ZoneSpawnSound;

	//Contains all level blox in scene
	TMap<int, TArray<ABloxActor*>>  BloxLevelMap;

	//Contains all level zones in scene
	TMap<int, AZonePlaceableActor*>  ZoneLevelMap;
	
	UARPin* ZonePinComponent;
};