// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Actor.h"
#include "InteractRushEInterface.h"
#include "SwitchInterface.h"
#include "Interact_switch.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class FPSATCPP_API AInteract_switch : public AActor, public IInteractRushEInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteract_switch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Interact")
	TObjectPtr<UBoxComponent> InteractBounds;

	UPROPERTY(VisibleAnywhere, Category = "Interact")
	TObjectPtr<UStaticMeshComponent> ButtonMesh;

	UPROPERTY(VisibleAnywhere, Category = "Interact")
	TObjectPtr<UMaterialInstanceDynamic> ButtonMaterial;

	UPROPERTY(EditAnywhere, Category = "Interact")
	FLinearColor OffColor = FLinearColor(0.10f, 0.11f, 0.13f);
	UPROPERTY(EditAnywhere, Category = "Interact")
	FLinearColor OnColor = FLinearColor(1.0f, 0.42f, 0.05f);

	UPROPERTY(EditAnywhere, Category = "Interact",meta=(MustImplement="/Script/FPSatCPP.SwitchInterface"))
	TArray<TObjectPtr<AActor>> Target;

	UPROPERTY(EditAnywhere, Category = "Interact")
	EFloorSwitchMode PressMode = EFloorSwitchMode::Toggle;

	UPROPERTY(EditAnywhere, Category = "Interact", meta=(DisplayName="一定時間オン"))
	bool bTimedOn = true;

	UPROPERTY(EditAnywhere, Category = "Interact", meta=(DisplayName="オン秒数", ClampMin="0.1", EditCondition="bTimedOn"))
	float OnSeconds = 5.f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
	void OnSwitched(bool On);

	void SendCommand(bool pressed);
	void SendCommand(bool pressed, EFloorSwitchMode Mode);
	void BeginTimedOn();
	void EndTimedOn();

	void UpdateButton();

	bool lit = false;
	FTimerHandle OnTimerHandle;
		
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnInteractRushE_Implementation(AActor* Interactor, bool bPressed) override;

	virtual void OnConstruction(const FTransform& Transform) override;

};
