// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwitchInterface.h"
#include "Door.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class FPSATCPP_API ADoor : public AActor, public ISwitchInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoor();

	virtual void OnSwitchCommand_Implementation(AActor* Source, EFloorSwitchMode Mode, bool bPressed) override;

	UFUNCTION(BlueprintCallable, Category="Door")
	void SetOpen(bool bNewOpen);

	UFUNCTION(BlueprintCallable, Category="Door")
	void ToggleOpen();

	UFUNCTION(BlueprintPure, Category="Door")
	bool IsOpen() const { return bOpen; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<USceneComponent> DoorPivot;
	
	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<UBoxComponent> DoorBlocker;

	//Yaw 
	UPROPERTY(EditAnywhere, Category="Door")
	float OpenAngle = 90.f;

	UPROPERTY(EditAnywhere, Category="Door", meta=(ClampMin="1.0"))
	float OpenSpeed = 120.f;

	//Is Oppen at Start
	UPROPERTY(EditAnywhere, Category="Door")
	bool bStartOpen = false;

	FRotator ClosedRelativeRotation = FRotator::ZeroRotator;

	bool bOpen = false;
	bool bHasReceivedCommand = false;
	FSwitchDriveState DriveState;


};
