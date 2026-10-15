// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SwitchInterface.generated.h"

UENUM(BlueprintType)
enum class EFloorSwitchMode : uint8
{
	WhilePressed UMETA(DisplayName="押している間"),
	Toggle UMETA(DisplayName="押すたびに切り替え"),
	StayOn UMETA(DisplayName="オンにする"),
	TurnOff UMETA(DisplayName="オフにする")
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class USwitchInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPSATCPP_API ISwitchInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Switch")
	void OnSwitchCommand(EFloorSwitchMode Mode, bool bPressed);
};
