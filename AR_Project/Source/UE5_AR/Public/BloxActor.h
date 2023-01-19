// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BloxActor.generated.h"

class UARPin;

//! @brief Used for inheritence to create a data only blueprint of a Blox.
//! Holds the functionality for the different Blox

UCLASS()
class UE5_AR_API ABloxActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	//! @brief Sets the static mesh and trigger box for the Blox
	ABloxActor();

protected:
	
	//! @brief Called when the game starts or when spawned.
	//! Sets the original transform for future use in the MoveActorToOrigin function
	//! Starts tracking the collision, sets the blox invisible.
	virtual void BeginPlay() override;

public:	
	//! @brief Called every frame.
	//! Handles Blox tracking, Updates different blox functionality,
	//! Updates trigger box position for collision.
	//! Checks if the blox falls off the level is so "de-spawn"
	//! @param DeltaTime - used for timers
	virtual void Tick(float DeltaTime) override;

	//! @brief Sets the position of the blox to the spawned zone location plus the blox difference.
	//!
	//! @param PinComponent - Used for tracking the blox to the zone.
	//! @param OriginZoneLocation - Used to calculate the RelativeTransform
	void SetUpLevel(UARPin* PinComponent,  FVector OriginZoneLocation);

	//! @brief Sets the blox actor back to its original position and tracks the level zone.
	//! Makes it non interactable
 	void MoveActorToOrigin();

	//! @brief Used to calculate the Gem Blox functionality.
	//! Checks if it is a gem blox, if its velocity exceeds a set float "de-spawn", and if it was hit "de-spawn".
	//! @param DeltaTime - used for timers
	void GemBloxUpdate(const float DeltaTime);

	//! @brief Used to calculate the Void Blox functionality.
	//! Checks if it is a void blox, and if it was hit "de-spawn".
	void VoidBloxUpdate();

	//! @brief Checks Collision.
	//! Checks if the blox collided with the ball or zone.
	// declare overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(Category = "Blox Info", VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* SceneComponent;

	// Used to simulate physics
	UPROPERTY(Category = "Blox Info", VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

	//Used to simulate collision
	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* TriggerBox;

	//Used to track which level the Blox is apart of
	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	int Level;
		
	//Used to set the Gem Blox functionality in editor
	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	bool bIsGemBlox = false;

	//Used to set the Void Blox functionality in editor
	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	bool bIsVoidBlox = false;

	//Used to set the trigger box size
	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	float BoxBound = 45;

	//Used to set the Blox Velocity limit before "de-spawning"
	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	float VelocityLimit = 65;//120

	//Used to delay Blox "de-spawning"
	float timer = 0;

	//Used to set the blox tracking the spawned zone
	bool BIsTracking = true;

	//Used to set blox tracking the level zone
	bool BIsTrackingOrigin = true;

	//Used to only calculate blox functionality if its level is in play
	bool BIsPlaying = false;

	//Used for collision
	bool BIsBloxHit = false;
	bool BIsGemBloxHit = false;
	bool bIsVoidBloxHit = false;

	UARPin* ActivePin;

	//Used to track the blox
	FTransform RelativeTransform;

	//Used to set the blox position when not in play
	FTransform OriginalTransform;

	//Used to calculate the blox difference for tracking
	FTransform OriginalZoneTransform;

	//Sound effects for blox
	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	USoundBase* DespawnSound;

	UPROPERTY(Category = "Blox Info", EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound;
	
};

