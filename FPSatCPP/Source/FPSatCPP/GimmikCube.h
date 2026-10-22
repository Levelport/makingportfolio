// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GimmikCube.generated.h"

class UStaticMeshComponent;

UCLASS()
class FPSATCPP_API AGimmikCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGimmikCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Gimmik")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gimmik", meta=(ClampMin="0.0", ForceUnits="cm"))
	float KickDistance = 50.f;

	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	bool bKicking = false;
	float KickCooldown = 0.f;
	FVector KickOrigin = FVector::ZeroVector;
	FVector KickDirection = FVector::ForwardVector;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
