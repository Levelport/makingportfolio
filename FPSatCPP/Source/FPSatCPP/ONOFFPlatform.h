// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwitchInterface.h"
#include "ONOFFPlatform.generated.h"

class UStaticMeshComponent;

UCLASS()
class FPSATCPP_API AONOFFPlatform : public AActor, public ISwitchInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AONOFFPlatform();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void OnSwitchCommand_Implementation(AActor* Source, EFloorSwitchMode Mode, bool bPressed) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Platform")
	TObjectPtr<UStaticMeshComponent> Platform;

	// ON OFF Reverce
	UPROPERTY(EditAnywhere, Category="Platform", meta=(DisplayName="ReverceSignal"))
	bool bStartOn = false;

	void ApplyFromSignal();
	void ApplyGone(bool bGone);

	bool bGone = false;
	bool bHasReceivedCommand = false;
	FSwitchDriveState DriveState;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
