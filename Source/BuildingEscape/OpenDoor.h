// Copyright Svetliaka92

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"

#include "OpenDoor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	float InitialYaw = 0.f;
	float InitialPitch = 0.f;
	float InitialRoll = 0.f;

	float CurrentYaw = 0.f;
	float CurrentPitch = 0.f;
	float CurrentRoll = 0.f;

	float TargetYaw = 0.f;
	float TargetPitch = 0.f;
	float TargetRoll = 0.f;

	FVector InitialLocation = FVector();
	FVector CurrentLocation = FVector();
	FVector TargetLocation = FVector();

	UPROPERTY(EditAnywhere)
	bool IsOpenedByRotation = true; // false for translation

	UPROPERTY(EditAnywhere)
	float OpenByPitch = 0.f;

	UPROPERTY(EditAnywhere)
	float OpenByYaw = 0.f;

	UPROPERTY(EditAnywhere)
	float OpenByRoll = 0.f;

	UPROPERTY(EditAnywhere)
	FVector OpenByTranslation = FVector();

	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY()
	UAudioComponent* AudioComponent = nullptr;

	UPROPERTY(EditAnywhere)
	float DoorOpenSpeed = 45;

	UPROPERTY(EditAnywhere)
	float DoorCloseSpeed = 45;

	UPROPERTY(EditAnywhere)
	float DoorCloseDelay = 2.f;

	UPROPERTY(EditAnywhere)
	float MassThresholdMin = 50.f;

	UPROPERTY(EditAnywhere)
	float MassThresholdMax = 300.f;

	void FindAudioComponent();
	void OpenDoorByRotation(float DeltaTime);
	void OpenDoorByTranslation(float DeltaTime);
	void CloseDoorByRotation(float DeltaTime);
	void CloseDoorByTranslation(float DeltaTime);
	float TotalMassOfActors() const;
	void PressurePlateLog() const;

	float DoorLastOpened = 0.f;
	bool OpenDoorSoundPlayed = false;
	bool CloseDoorSoundPlayed = true;
};
