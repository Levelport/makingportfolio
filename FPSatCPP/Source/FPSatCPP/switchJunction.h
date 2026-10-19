// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwitchInterface.h"
#include "switchJunction.generated.h"

UCLASS()
class FPSATCPP_API AswitchJunction : public AActor, public ISwitchInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AswitchJunction();

	virtual void OnSwitchCommand_Implementation(AActor* Source, EFloorSwitchMode Mode, bool bPressed) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** How many switches must be on before Targets are turned on. */
	UPROPERTY(EditAnywhere, Category="Switch", meta=(ClampMin="1"))
	int32 RequiredCount = 2;

	/** Actors that implement SwitchInterface. This junction does not know what they are. */
	UPROPERTY(EditAnywhere, Category="Switch", meta=(MustImplement="/Script/FPSatCPP.SwitchInterface"))
	TArray<TObjectPtr<AActor>> Targets;

	void RefreshOutput();

	TMap<TWeakObjectPtr<AActor>, bool> SourceOn;
	bool bOutputOn = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
