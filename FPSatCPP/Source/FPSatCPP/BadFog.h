// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BadFog.generated.h"

class UBoxComponent;

UCLASS()
class FPSATCPP_API ABadFog : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABadFog();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Fog")
	TObjectPtr<UBoxComponent> FogVolume;

	UFUNCTION()
	void OnFogBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bRetryStarted = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
