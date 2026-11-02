// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "junctionSignInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UjunctionSignInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPSATCPP_API IjunctionSignInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Junction")
	void OnJunctionCount(int32 OnCount, int32 RequiredCount);
};
