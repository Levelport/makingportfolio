// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	void OnSwitchCommand(AActor* Source, EFloorSwitchMode Mode, bool bPressed);
};

/** One object's on/off, combined from every switch that commands it. */
struct FSwitchDriveState
{
	TSet<TWeakObjectPtr<AActor>> HeldSwitches;
	bool bLatched = false;

	bool IsOn()
	{
		for (auto It = HeldSwitches.CreateIterator(); It; ++It)
		{
			if (!It->IsValid())
			{
				It.RemoveCurrent();
			}
		}

		return bLatched || HeldSwitches.Num() > 0;
	}

	bool Apply(AActor* Source, EFloorSwitchMode Mode, bool bPressed)
	{
		if (Mode == EFloorSwitchMode::WhilePressed)
		{
			if (bPressed && IsValid(Source))
			{
				HeldSwitches.Add(TWeakObjectPtr<AActor>(Source));
			}
			else
			{
				HeldSwitches.Remove(TWeakObjectPtr<AActor>(Source));
			}
		}
		else if (bPressed && Mode == EFloorSwitchMode::Toggle)
		{
			bLatched = !bLatched;
		}
		else if (bPressed && Mode == EFloorSwitchMode::StayOn)
		{
			bLatched = true;
		}
		else if (Mode == EFloorSwitchMode::TurnOff)
		{
			bLatched = false;
			HeldSwitches.Reset();
		}

		return IsOn();
	}
};
