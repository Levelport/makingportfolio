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

	//Switchs Needed
	UPROPERTY(EditAnywhere, Category="Switch", meta=(ClampMin="1"))
	int32 RequiredCount = 2;

	UPROPERTY(EditAnywhere, Category="Switch", meta=(MustImplement="/Script/FPSatCPP.SwitchInterface"))
	TArray<TObjectPtr<AActor>> Targets;

	UPROPERTY(EditAnywhere, Category="Switch", meta=(DisplayName="Sign", MustImplement="/Script/FPSatCPP.junctionSignInterface"))
	TArray<TObjectPtr<AActor>> Signs;

	void RefreshOutput();
	void BroadcastCount(int32 OnCount);
	int32 CurrentOnCount() const;

	TMap<TWeakObjectPtr<AActor>, bool> SourceOn;
	bool bOutputOn = false;

public:
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
