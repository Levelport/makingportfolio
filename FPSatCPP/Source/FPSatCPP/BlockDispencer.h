// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwitchInterface.h"
#include "BlockDispencer.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class FPSATCPP_API ABlockDispencer : public AActor, public ISwitchInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlockDispencer();

	virtual void OnSwitchCommand_Implementation(AActor* Source, EFloorSwitchMode Mode, bool bPressed) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Dispencer")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category="Dispencer")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, Category="Dispencer", meta=(DisplayName="DropPlace"))
	TObjectPtr<USceneComponent> DropPoint;

	UPROPERTY(EditAnywhere, Category="Dispencer", meta=(DisplayName="DropThing"))
	TSoftClassPtr<AActor> CubeClass;

	UPROPERTY()
	TObjectPtr<AActor> SpawnedCube;

	void DropCube();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
