// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonThingController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"

AFirstPersonThingController::AFirstPersonThingController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultContext(TEXT("/Game/Input/IMC_Default.IMC_Default"));
	if (DefaultContext.Succeeded())
	{
		DefaultMappingContexts.Add(DefaultContext.Object);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MouseLookContext(TEXT("/Game/Input/IMC_MouseLook.IMC_MouseLook"));
	if (MouseLookContext.Succeeded())
	{
		MobileExcludedMappingContexts.Add(MouseLookContext.Object);
	}
}

void AFirstPersonThingController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalPlayerController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
	{
		Subsystem->AddMappingContext(CurrentContext, 0);
	}

	for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
	{
		Subsystem->AddMappingContext(CurrentContext, 0);
	}
}
