// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwitchInterface.h"
#include "Switch.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;
class UMaterialInstanceDynamic;

/**
 * 
 */
UCLASS()
class FPSATCPP_API ASwitch : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASwitch();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Switch")
	TObjectPtr<UBoxComponent> InteractBounds;

	UPROPERTY(VisibleAnywhere, Category="Switch")
	TObjectPtr<UStaticMeshComponent> Plate;

	UPROPERTY(VisibleAnywhere, Category="Switch")
	TObjectPtr<UBoxComponent> PlateTrigger;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PlateMaterial;

	UPROPERTY(EditAnywhere, Category="Switch")
	FLinearColor OffColor = FLinearColor(0.10f, 0.11f, 0.13f);

	UPROPERTY(EditAnywhere, Category="Switch")
	FLinearColor OnColor = FLinearColor(1.0f, 0.42f, 0.05f);

	UPROPERTY(EditAnywhere, Category="Switch", meta=(MustImplement="/Script/FPSatCPP.SwitchInterface"))
	TArray<TObjectPtr<AActor>> Targets;

	//switch type
	UPROPERTY(EditAnywhere, Category="Switch")
	EFloorSwitchMode PressMode = EFloorSwitchMode::WhilePressed;

	UFUNCTION(BlueprintImplementableEvent, Category="Switch")
	void OnSwitched(bool bNowOn);

	UFUNCTION()
	void OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SendCommand(bool bPressed);

	bool CountsAsWeight(AActor* OtherActor, UPrimitiveComponent* OtherComp) const;
	int32 NumWeights() const;
	void PlacePlate();
	void UpdatePlate();

	TSet<TWeakObjectPtr<AActor>> Weights;

public:
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
