// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class FPSATCPP_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	ADoor();

	UFUNCTION(BlueprintCallable, Category="Door")
	void SetOpen(bool bNewOpen);

	UFUNCTION(BlueprintCallable, Category="Door")
	void ToggleOpen();

	UFUNCTION(BlueprintPure, Category="Door")
	bool IsOpen() const { return bOpen; }

	virtual void Tick(float DeltaTime) override;

protected:
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

};
