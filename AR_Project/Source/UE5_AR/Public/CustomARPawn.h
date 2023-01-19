// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ARBlueprintLibrary.h"
#include "GameFramework/Pawn.h"
#include "CustomARPawn.generated.h"

class UARSessionConfig;
class UCameraComponent;

//! @brief Class used to handle player input
UCLASS()
class UE5_AR_API ACustomARPawn : public APawn
{
	GENERATED_BODY()

public:
	//! @brief Sets the camera for the player
	ACustomARPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//! @brief Recognises user touching the screen, Used to spawn the zone, then throw balls
	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

	//! @brief Recognises user holding the screen, Used to drag the spawned zone into new positions
	virtual void OnScreenHold(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* CameraComponent;
};
