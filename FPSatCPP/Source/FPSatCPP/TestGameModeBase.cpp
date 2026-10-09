// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGameModeBase.h"
#include "FPSatCPPPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TestMyCharacter.h"
#include "TesterWidget.h"

ATestGameModeBase::ATestGameModeBase()
{
	DefaultPawnClass = ATestMyCharacter::StaticClass();
	PlayerControllerClass = AFPSatCPPPlayerController::StaticClass();
}

void ATestGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (TesterWidgetClass && PlayerController)
	{
		TesterWidget = CreateWidget<UTesterWidget>(PlayerController, TesterWidgetClass);
		if (TesterWidget)
		{
			TesterWidget->AddToViewport(0);
		}
	}
}
