// Fill out your copyright notice in the Description page of Project Settings.


#include "BallActor.h"
#include "ARPin.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABallActor::ABallActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	TriggerSphere->SetSphereRadius(SphereBound);
	TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));
	TriggerSphere->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ABallActor::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::PlaySound2D(this, ShootSound);
}

// Called every frame
void ABallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ZonePinComponent)
	{
		if(StaticMeshComponent->GetComponentLocation().Z <= ZonePinComponent->GetLocalToWorldTransform().GetLocation().Z - 100)
		{
			Destroy();
		}
	}
	
	timer += DeltaTime;
	if(timer >= DespawnTime)
	{
		Destroy();
		timer = 0;
	}
}

