// Fill out your copyright notice in the Description page of Project Settings.


#include "ZonePlaceableActor.h"
#include "ARPin.h"
#include "ARBlueprintLibrary.h"
#include "Components/BoxComponent.h"


AZonePlaceableActor::AZonePlaceableActor()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetBoxExtent(FVector(35, 35,3.5));
}

void AZonePlaceableActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
}

void AZonePlaceableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Pin Location: %s"), *GetActorLocation().ToString()));

	TriggerBox->SetWorldLocation(StaticMeshComponent->GetComponentLocation());
	TriggerBox->SetWorldRotation(StaticMeshComponent->GetComponentRotation());
	
	//Making sure the actor remains on the ARPin that has been found.
	if(PinComponent)
	{
		auto TrackingState = PinComponent->GetTrackingState();
		
		switch (TrackingState)
		{
		case EARTrackingState::Tracking:
			SceneComponent->SetVisibility(true);
			SetActorTransform(PinComponent->GetLocalToWorldTransform());
			break;
	
		case EARTrackingState::NotTracking:
			PinComponent = nullptr;
	
			break;
	
		}
	}
}
