// Fill out your copyright notice in the Description page of Project Settings.


#include "BloxActor.h"
#include "ARPin.h"
#include "BallActor.h"
#include "CustomGameMode.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABloxActor::ABloxActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetBoxExtent(FVector(BoxBound,BoxBound,BoxBound));
}

// Called when the game starts or when spawned
void ABloxActor::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalTransform = StaticMeshComponent->GetComponentTransform();
	
	SetActorHiddenInGame(true);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABloxActor::OnOverlapBegin);
}

// Called every frame
void ABloxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsValid(ActivePin))
	{
		if(BIsPlaying)
		{
			TriggerBox->SetWorldLocation(StaticMeshComponent->GetComponentLocation());
			TriggerBox->SetWorldRotation(StaticMeshComponent->GetComponentRotation());
			
			GemBloxUpdate(DeltaTime);
			VoidBloxUpdate();

			if(StaticMeshComponent->GetComponentLocation().Z <= ActivePin->GetLocalToWorldTransform().GetLocation().Z - 100)
			{
				//Send Cube back to original position on hit
				MoveActorToOrigin();
			}
		}

		// Making sure the actor remains on the ARPin that has been found.
		if(BIsTracking && !BIsTrackingOrigin)
		{
			auto TrackingState = ActivePin->GetTrackingState();
		
			switch (TrackingState)
			{
			case EARTrackingState::Tracking:
				SceneComponent->SetVisibility(true);
			
				StaticMeshComponent->SetWorldTransform(RelativeTransform * ActivePin->GetLocalToWorldTransform());
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Pin Location: %s"), *PinComponent->GetLocalToWorldTransform().GetLocation().ToString()));
				break;
	
			case EARTrackingState::NotTracking:
				ActivePin = nullptr;
				break;
	
			}
		}
	}

	if (BIsTrackingOrigin && BIsTracking)
	{
		//get transform of zone
		StaticMeshComponent->SetWorldTransform(RelativeTransform * OriginalZoneTransform);
	}
}

void ABloxActor::SetUpLevel(UARPin* PinComponent,  FVector OriginZoneLocation)
{
	StaticMeshComponent->SetSimulatePhysics(true);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("setuplevel function reached"));

	//Get Pin from the placed zone actor
	ActivePin = PinComponent;
	
	//Calculate the blocks position from the original level zone
	FVector BlockDiff = StaticMeshComponent->GetComponentLocation() - OriginZoneLocation;
	
	//Set the blocks position to the spawned zones position then add the block difference to keep the original formation
	StaticMeshComponent->SetWorldLocation(ActivePin->GetLocalToWorldTransform().GetLocation() + BlockDiff);
	TriggerBox->SetWorldLocation(StaticMeshComponent->GetComponentLocation());
	TriggerBox->SetWorldRotation(StaticMeshComponent->GetComponentRotation());
	
	BIsPlaying = true;
	//BIsTracking = true;
	BIsTrackingOrigin = false;
	SetActorHiddenInGame(false);
	StaticMeshComponent->SetEnableGravity(true);
}

void ABloxActor::MoveActorToOrigin()
{

	//play sound
	UGameplayStatics::PlaySoundAtLocation(this, DespawnSound, StaticMeshComponent->GetComponentLocation(), StaticMeshComponent->GetComponentRotation());
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Cube Destroy reached")));
	BIsPlaying = false;
	SetActorHiddenInGame(true);
	StaticMeshComponent->SetWorldTransform(OriginalTransform);//print this position to screen
	//TriggerBox->SetWorldTransform(OriginalTransform);
	TriggerBox->SetWorldLocation(OriginalTransform.GetLocation());
	TriggerBox->SetWorldRotation(OriginalTransform.GetRotation());
	StaticMeshComponent->SetPhysicsLinearVelocity(FVector(0,0,0));
	StaticMeshComponent->SetPhysicsAngularVelocityInDegrees(FVector(0,0,0));
	StaticMeshComponent->SetEnableGravity(false);
	StaticMeshComponent->SetSimulatePhysics(false);
	BIsTracking = true;
	//ActivePin = nullptr;
	BIsTrackingOrigin = true;
	
	if(bIsGemBlox)
	{
		//add score
		auto Temp = GetWorld()->GetAuthGameMode();
		auto GM = Cast<ACustomGameMode>(Temp);
		GM->ActiveLevelGemBloxHit++;
	}
	
}

void ABloxActor::GemBloxUpdate(const float DeltaTime)
{
	if(bIsGemBlox)
	{
		FVector LinearVelocity = StaticMeshComponent->GetPhysicsLinearVelocity();
		if(LinearVelocity.X > VelocityLimit || LinearVelocity.Y > VelocityLimit || LinearVelocity.Z > VelocityLimit)
		{
			//Start timer
			timer += DeltaTime;
			if(timer >= 0.2)
			{
				MoveActorToOrigin();
				timer = 0;
			}
		}

		if(BIsGemBloxHit)
		{
			//Start timer
			timer += DeltaTime;
			if(timer >= 1)
			{
				BIsGemBloxHit = false;
				MoveActorToOrigin();
				timer = 0;
			}
		}
	}
}

void ABloxActor::VoidBloxUpdate()
{
	if(bIsVoidBlox)
	{
		if(bIsVoidBloxHit)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("void blox hit")));
			bIsVoidBloxHit = false;
			MoveActorToOrigin();
		}
	}
	
}

void ABloxActor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("on overlap called")));
	// check if Actors do not equal nullptr
	if(BIsPlaying)
	{
		if (OtherActor && (OtherActor != this)) 
		{
			ABallActor* TempBall = Cast<ABallActor>(OtherActor);
			if(IsValid(TempBall))
			{
				if(bIsVoidBlox)
				{
					bIsVoidBloxHit = true;
				}
				if(bIsGemBlox)
				{
					BIsGemBloxHit = true;
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("ball hit gem")));
				}
				//play sound
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, StaticMeshComponent->GetComponentLocation(), StaticMeshComponent->GetComponentRotation());
			}

			AZonePlaceableActor* TempZone = Cast<AZonePlaceableActor>(OtherActor);
			if(IsValid(TempZone))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("temp zpne valid")));
				if(bIsGemBlox)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("gem hit zone")));
					BIsGemBloxHit = true;
				}
			}
		}	
	}
}

void ABloxActor::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap End"));
	}
}