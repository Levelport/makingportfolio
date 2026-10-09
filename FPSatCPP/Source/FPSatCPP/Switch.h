// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Switch.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EFloorSwitchMode : uint8
{
	WhilePressed UMETA(DisplayName="押している間"),
	Toggle UMETA(DisplayName="押すたびに切り替え"),
	StayOn UMETA(DisplayName="オンにする"),
	TurnOff UMETA(DisplayName="オフにする")
};

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

	UPROPERTY(EditAnywhere, Category="Switch")
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

	int32 OverlappingPawns = 0;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
