// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"
#include "CustomARPawn.h"
#include "HelloARManager.h"
#include "ARPin.h"
#include "ARBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "PlaceableActor.h"
#include "ARPin.h"
#include "BallActor.h"
#include "BloxActor.h"
#include "ZonePlaceableActor.h"

ACustomGameMode::ACustomGameMode():
	SpawnedActor(nullptr)
{
	// Add this line to your code if you wish to use the Tick() function
	PrimaryActorTick.bCanEverTick = true;

	// Set the default pawn and gamestate to be our custom pawn and gamestate programatically
	DefaultPawnClass = ACustomARPawn::StaticClass();
	//GameStateClass = ACustomGameState::StaticClass();
	
}

void ACustomGameMode::StartPlay() 
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Current Score: %d"), GetScore()));
	
	//Init Game Variables
	FillZoneMap();
	FillBloxMapArray();
	InitLevelActors();
	
	
	// This function will transcend to call BeginPlay on all the actors 
	Super::StartPlay();

}

void ACustomGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckWinState(DeltaSeconds);
}

void ACustomGameMode::LineTraceSpawnActor(FVector ScreenPos)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Line Trace Reached"));

	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos;
	FVector WorldDir;

	
	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(ScreenPos), WorldPos, WorldDir);
	// Notice that this LineTrace is in the ARBluePrintLibrary - this means that it's exclusive only for objects tracked by ARKit/ARCore
	auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ScreenPos), false, false, false, true);
	
	//Checks if the location is valid
	if (TraceResult.IsValidIndex(0))
	{
		// Get the first found object in the line trace - ignoring the rest of the array elements
		auto TrackedTF = TraceResult[0].GetLocalToWorldTransform();

		if (FVector::DotProduct(TrackedTF.GetRotation().GetUpVector(), WorldDir) < 0)
		{
			//Spawn the actor pin and get the transform
			UARPin* ActorPin = UARBlueprintLibrary::PinComponent(nullptr, TraceResult[0].GetLocalToWorldTransform(), TraceResult[0].GetTrackedGeometry());

			// Check if ARPins are available on your current device. ARPins are currently not supported locally by ARKit, so on iOS, this will always be "FALSE" 
			if (ActorPin)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("ARPin is valid"));
				//If the pin is valid 
				auto PinTF = ActorPin->GetLocalToWorldTransform();
				
				//Spawn a new Actor at the location if not done yet
				if (!IsValid(SpawnedActor))
				{
					const FActorSpawnParameters SpawnInfo;
					const FRotator MyRot(0, 0, 0);
					const FVector MyLoc(0, 0, 0);
					SpawnedActor = GetWorld()->SpawnActor<APlaceableActor>(ZonePlacableToSpawn, MyLoc, MyRot, SpawnInfo);
					
				}


				if (IsValid(SpawnedActor))
				{
					// Set the spawned actor location based on the Pin. Have a look at the code for Placeable Object to see how it handles the AR PIN passed on
					SpawnedActor->SetActorTransform(PinTF);
					SpawnedActor->PinComponent = ActorPin;
					ZonePinComponent = ActorPin;
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Zone Location: %s"), *SpawnedActor->GetActorLocation().ToString()));
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Zone Pin Location: %s"), *SpawnedActor->PinComponent->GetLocalToWorldTransform().GetLocation().ToString()));
				}
			}
		}
	}
}

void ACustomGameMode::SpawnMainZoneActor(FVector ScreenPos)
{
	LineTraceSpawnActor(ScreenPos);
	if(IsValid(SpawnedActor))
	{
		SpawnedActor->SetActorHiddenInGame(false);
		ARManager->SetTracking(false);
		BZoneSpawned = true;
		UGameplayStatics::PlaySound2D(this, ZoneSpawnSound);
	}
}

void ACustomGameMode::MoveMainZoneActor(FVector ScreenPos)
{
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<ACustomGameMode>(Temp);
	
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("ScreenTouch Reached"));
	
	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos;
	FVector WorldDir;
	
	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(ScreenPos), WorldPos, WorldDir);
	// Notice that this LineTrace is in the ARBluePrintLibrary - this means that it's exclusive only for objects tracked by ARKit/ARCore
	auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ScreenPos), false, false, false, true);
	
	
	//Checks if the location is valid
	if (TraceResult.IsValidIndex(0))
	{
		// Get the first found object in the line trace - ignoring the rest of the array elements
		auto TrackedTF = TraceResult[0].GetLocalToWorldTransform();
	
		if (FVector::DotProduct(TrackedTF.GetRotation().GetUpVector(), WorldDir) < 0)
		{
			//Spawn the actor pin and get the transform
			UARPin* ActorPin = UARBlueprintLibrary::PinComponent(nullptr, TraceResult[0].GetLocalToWorldTransform(), TraceResult[0].GetTrackedGeometry());
	
			// Check if ARPins are available on your current device. ARPins are currently not supported locally by ARKit, so on iOS, this will always be "FALSE" 
			if (ActorPin)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("ARPin is valid"));
				//If the pin is valid 
				auto PinTF = ActorPin->GetLocalToWorldTransform();

				SpawnedActor->PinComponent = ActorPin;
				ZonePinComponent = ActorPin;
			}
		}
	}
}

void ACustomGameMode::SpawnBloxCube()
{
	// const FActorSpawnParameters SpawnInfo;
	// const FRotator MyRot(0, 0, 0);
	// FVector MyLoc = ZonePinComponent->GetLocalToWorldTransform().GetLocation();
	//
	// //Spawns 3x3 cube of cubes
	// MyLoc.X += -10;
	// MyLoc.Y += -10;
	// MyLoc.Z += 6.5;
	//
	// for(int i3 = 0; i3<3; i3++)
	// {
	// 	for(int i2 = 0; i2<3; i2++)
	// 	{
	// 		for(int i1 = 0; i1<3; i1++)
	// 		{
	// 			ABloxActor* BloxActor = GetWorld()->SpawnActor<ABloxActor>(BasicBlox, MyLoc, MyRot, SpawnInfo);
	// 			BloxActor->SetPin(ZonePinComponent);
	// 			MyLoc.X += 10;
	// 		}
	// 		MyLoc.X = ZonePinComponent->GetLocalToWorldTransform().GetLocation().X - 10;
	// 		MyLoc.Y += 10;
	// 	}
	// 	MyLoc.Y = ZonePinComponent->GetLocalToWorldTransform().GetLocation().Y -10;
	// 	MyLoc.Z += 8;
	// }

}

void ACustomGameMode::SpawnProjectile(FVector ScreenPos)
{
	//Basic variables for functionality
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos;
	FVector WorldDir;

	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(ScreenPos), WorldPos, WorldDir);

	//for deproject screen to world world pos + world dir * how far away i want it from the camera
	FVector MyLoc = WorldPos;
	const FActorSpawnParameters SpawnInfo;
	ABallActor* BallActor = GetWorld()->SpawnActor<ABallActor>(Ball, MyLoc, FRotator(0,0,0), SpawnInfo);
	BallActor->SetPin(ZonePinComponent);
	
	TArray<UStaticMeshComponent*> Components;
	BallActor->GetComponents<UStaticMeshComponent>(Components);
	for( int32 i=0; i<Components.Num(); i++ )
	{
		UStaticMeshComponent* StaticMeshComponent = Components[i];
		StaticMeshComponent->AddForce(WorldDir*100000*StaticMeshComponent->GetMass());
	}
	
}

void ACustomGameMode::SetBloxTracking(bool BIsBloxTracking)
{
	if(BloxLevelMap.Contains(LevelNum))
	{
		for (auto NewBlox_Actor : BloxLevelMap[LevelNum])
		{
			if(!IsValid(NewBlox_Actor))
				continue;
			
			NewBlox_Actor->BIsTracking = BIsBloxTracking;
		}
	}
}

void ACustomGameMode::FillBloxMapArray()
{
	//Get all blox actors in scene
	TArray<AActor*> BloxActors_Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ABloxActor::StaticClass(), BloxActors_Found);
	
	for(auto* actors: BloxActors_Found)
	{
		ABloxActor* Blox_Actor = Cast<ABloxActor>(actors);

		if(IsValid(Blox_Actor))
		{
			//Checks if map has level in it 
			if(!BloxLevelMap.Contains(Blox_Actor->Level))
			{
				BloxLevelMap.Add(Blox_Actor->Level, TArray<ABloxActor*>());
			}
			
			BloxLevelMap[Blox_Actor->Level].Add(Blox_Actor);
		}
	}
}

void ACustomGameMode::FillZoneMap()
{
	//Get all zone actors in scene
	TArray<AActor*> ZoneActors_Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AZonePlaceableActor::StaticClass(), ZoneActors_Found);
	int ZoneCount = 0;
	int ValidZoneCount = 0;
	for(auto* actors: ZoneActors_Found)
	{
		AZonePlaceableActor* Zone_Actor = Cast<AZonePlaceableActor>(actors);

		if(IsValid(Zone_Actor))
		{
			ZoneLevelMap.Add(Zone_Actor->Level, Zone_Actor);

			//ZoneLevelArray.FindOrAdd()
			//https://forums.unrealengine.com/t/filling-a-tmap-with-an-array/335411

			ValidZoneCount++;
		}
		ZoneCount++;
	}

	UE_LOG(LogTemp, Warning, TEXT("Zone Count: %d"), ZoneCount);
	UE_LOG(LogTemp, Warning, TEXT("Valid Zone Count: %d"), ValidZoneCount);
}

void ACustomGameMode::InitLevelActors()
{

	//Iterate through every blox in the level and set its original zone pin to its zone level counterpart transform
	int LevelAmount = ZoneLevelMap.Num();
	//UE_LOG(LogTemp, Warning, TEXT("Level Amount: %d"), LevelAmount);
	
	for(int i = 1; i <= LevelAmount; i++)
	{
		for (auto Blox_Actor : BloxLevelMap.FindRef(i))
		{
			if(!IsValid(Blox_Actor))
				continue;

			AZonePlaceableActor* Zone_Actor = ZoneLevelMap.FindRef(i);

			if(!IsValid(Zone_Actor))
				continue;
			
			if(Blox_Actor->Level == Zone_Actor->Level)
			{
				Blox_Actor->OriginalZoneTransform = Zone_Actor->GetActorTransform();
				Blox_Actor->ActivePin = nullptr;

				//Set the relative transform, applied to actors transform on tick to keep it in position with the pin
				Blox_Actor->RelativeTransform = Blox_Actor->StaticMeshComponent->GetComponentTransform();
				Blox_Actor->RelativeTransform.SetTranslation(Blox_Actor->StaticMeshComponent->GetComponentLocation() - Zone_Actor->GetActorTransform().GetLocation());
				
				Blox_Actor->BIsTracking = true;
			}

			//Fill level Zone with the amount of gem blox it has
			if(Blox_Actor->bIsGemBlox)
			{
				Zone_Actor->GemBloxAmount++;
			}
		}
	}
}

void ACustomGameMode::CheckWinState(float DeltaTime)
{
	if(BActiveLevel)
	{
		if(ActiveLevelGemBloxHit >= ActiveLevelGemBloxAmount)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("You Win"));
			BIsWinState = true;
			BActiveLevel = false;
		}
		else if(ActiveLevelThrowAmount >= ActiveLevelThrowAmountLimit)
		{
			
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("You Lose"));
			if(CheckLose)
			{
				BIsLoseState = true;
				BActiveLevel = false;
				CheckLose = false;
			}
			
			//need it to wait then check again
			
			timer += DeltaTime;
			if(timer >= 2)
			{
				timer = 0;
				CheckLose = true;
			}
			
		}
	}
}

void ACustomGameMode::SpawnLevel(int NewLevelNum)
{
	ActiveLevelThrowAmount = 0;
	
	for (auto OldBlox_Actor : BloxLevelMap.FindRef(LevelNum))
	{
		if(!IsValid(OldBlox_Actor))
			continue;
			
		OldBlox_Actor->MoveActorToOrigin();
	}

	for (auto NewBlox_Actor : BloxLevelMap.FindRef(NewLevelNum))
	{
		if(!IsValid(NewBlox_Actor))
			continue;
			
		if(ZoneLevelMap.Contains(NewLevelNum))
		{
			NewBlox_Actor->SetUpLevel(ZonePinComponent, ZoneLevelMap.FindRef(NewBlox_Actor->Level)->GetActorTransform().GetLocation());
			NewBlox_Actor->BIsTracking = false;
			ActiveLevelGemBloxAmount = ZoneLevelMap.FindRef(NewBlox_Actor->Level)->GemBloxAmount;
			ActiveLevelThrowAmountLimit = ZoneLevelMap.FindRef(NewBlox_Actor->Level)->LevelThrowAmountLimit;
		}
	}

	ActiveLevelGemBloxHit = 0;
	LevelNum = NewLevelNum;
	BActiveLevel = true;
}

void ACustomGameMode::ResetZone()
{
	if(BZoneSpawned)
	{
		for (auto OldBlox_Actor : BloxLevelMap.FindRef(LevelNum))
		{
			if(!IsValid(OldBlox_Actor))
				continue;
			
			OldBlox_Actor->MoveActorToOrigin();
		}

		//Destroy(SpawnedActor);
		SpawnedActor->Destroy();
		SpawnedActor = nullptr;

		BActiveLevel = false;
		BZoneSpawned = false;
		BZoneLocked = false;

		ARManager->SetTracking(true);
	}
}

void ACustomGameMode::StartPlaneTracking()
{
	//spawn an instance of hello ar manager
	ARManager = GetWorld()->SpawnActor<AHelloARManager>();
}
