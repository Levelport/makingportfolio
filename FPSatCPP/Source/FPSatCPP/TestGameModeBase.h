// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestGameModeBase.generated.h"

class UTesterWidget;

/**
 * First person test game mode.
 * Spawns the test character and the tester widget only.
 */
UCLASS()
class FPSATCPP_API ATestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATestGameModeBase();

protected:

	UPROPERTY(EditAnywhere, Category="Test")
	TSubclassOf<UTesterWidget> TesterWidgetClass;

	UPROPERTY()
	TObjectPtr<UTesterWidget> TesterWidget;

	virtual void BeginPlay() override;

};
