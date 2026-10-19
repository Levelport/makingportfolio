// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FirstPersonThingController.generated.h"

class UInputMappingContext;

/**
 * 
 */
UCLASS()
class FPSATCPP_API AFirstPersonThingController : public APlayerController
{
	GENERATED_BODY()

public:
	AFirstPersonThingController();

protected:

	/** Mapping contexts that enable the possessed pawn's move and jump actions. */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	/** Mapping contexts for mouse look. */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> MobileExcludedMappingContexts;

	virtual void SetupInputComponent() override;
	
};
