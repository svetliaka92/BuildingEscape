// Copyright Svetliaka92

#include "OpenDoor.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	TargetYaw = InitialYaw + OpenByYaw;

	InitialPitch = GetOwner()->GetActorRotation().Pitch;
	CurrentPitch = InitialPitch;
	TargetPitch = InitialPitch + OpenByPitch;

	InitialRoll = GetOwner()->GetActorRotation().Roll;
	CurrentRoll = InitialRoll;
	TargetRoll = InitialRoll + OpenByRoll;

	InitialLocation = GetOwner()->GetActorLocation();
	CurrentLocation = InitialLocation;
	TargetLocation = InitialLocation + OpenByTranslation;

	FindAudioComponent();
	PressurePlateLog();
}

void UOpenDoor::PressurePlateLog() const
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Warning, TEXT("The actor %s has an OpenDoor component on it, but no pressure plate"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float TotalMass = TotalMassOfActors();
	if (TotalMass >= MassThresholdMin
		&& TotalMass <= MassThresholdMax)
	{
		if (IsOpenedByRotation)
			OpenDoorByRotation(DeltaTime);
		else
			OpenDoorByTranslation(DeltaTime);

		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		float currentTime = GetWorld()->GetTimeSeconds();
		if (currentTime - DoorLastOpened >= DoorCloseDelay)
		{
			if (IsOpenedByRotation)
				CloseDoorByRotation(DeltaTime);
			else
				CloseDoorByTranslation(DeltaTime);
		}
	}
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Audio Component found on actor: %s"), *GetOwner()->GetName());
	}
}

void UOpenDoor::OpenDoorByRotation(float DeltaTime)
{
	CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentPitch = GetOwner()->GetActorRotation().Pitch;
	CurrentRoll = GetOwner()->GetActorRotation().Roll;

	FRotator OpenDoor(CurrentPitch, CurrentYaw, CurrentRoll);

	OpenDoor.Yaw = FMath::FInterpConstantTo(CurrentYaw, TargetYaw, DeltaTime, DoorOpenSpeed);
	OpenDoor.Pitch = FMath::FInterpConstantTo(CurrentPitch, TargetPitch, DeltaTime, DoorOpenSpeed);
	OpenDoor.Roll = FMath::FInterpConstantTo(CurrentRoll, TargetRoll, DeltaTime, DoorOpenSpeed);
	GetOwner()->SetActorRelativeRotation(OpenDoor);

	CloseDoorSoundPlayed = false;
	if (AudioComponent && !OpenDoorSoundPlayed)
	{
		AudioComponent->Play();
		OpenDoorSoundPlayed = true;
	}
}

void UOpenDoor::OpenDoorByTranslation(float DeltaTime)
{
	CurrentLocation = GetOwner()->GetActorLocation();
	FVector OpenDoor = CurrentLocation;

	OpenDoor.X = FMath::FInterpConstantTo(CurrentLocation.X, TargetLocation.X, DeltaTime, DoorOpenSpeed);
	OpenDoor.Y = FMath::FInterpConstantTo(CurrentLocation.Y, TargetLocation.Y, DeltaTime, DoorOpenSpeed);
	OpenDoor.Z = FMath::FInterpConstantTo(CurrentLocation.Z, TargetLocation.Z, DeltaTime, DoorOpenSpeed);

	GetOwner()->SetActorLocation(OpenDoor);
}

void UOpenDoor::CloseDoorByRotation(float DeltaTime)
{
	CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentPitch = GetOwner()->GetActorRotation().Pitch;
	CurrentRoll = GetOwner()->GetActorRotation().Roll;

	FRotator CloseDoor(InitialPitch, InitialYaw, InitialRoll);

	CloseDoor.Yaw = FMath::FInterpConstantTo(CurrentYaw, InitialYaw, DeltaTime, DoorOpenSpeed);
	CloseDoor.Pitch = FMath::FInterpConstantTo(CurrentPitch, InitialPitch, DeltaTime, DoorOpenSpeed);
	CloseDoor.Roll = FMath::FInterpConstantTo(CurrentRoll, InitialRoll, DeltaTime, DoorOpenSpeed);
	GetOwner()->SetActorRelativeRotation(CloseDoor);

	OpenDoorSoundPlayed = false;
	if (AudioComponent && !CloseDoorSoundPlayed)
	{
		AudioComponent->Play();
		CloseDoorSoundPlayed = true;
	}
}

void UOpenDoor::CloseDoorByTranslation(float DeltaTime)
{
	CurrentLocation = GetOwner()->GetActorLocation();
	FVector CloseDoor = InitialLocation;

	CloseDoor.X = FMath::FInterpConstantTo(CurrentLocation.X, InitialLocation.X, DeltaTime, DoorOpenSpeed);
	CloseDoor.Y = FMath::FInterpConstantTo(CurrentLocation.Y, InitialLocation.Y, DeltaTime, DoorOpenSpeed);
	CloseDoor.Z = FMath::FInterpConstantTo(CurrentLocation.Z, InitialLocation.Z, DeltaTime, DoorOpenSpeed);

	GetOwner()->SetActorLocation(CloseDoor);
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	if (PressurePlate)
	{
		TArray<AActor*> OverlappingActors;
		PressurePlate->GetOverlappingActors(OUT OverlappingActors);

		for (AActor* Actor : OverlappingActors)
		{
			TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}

	return TotalMass;
}
