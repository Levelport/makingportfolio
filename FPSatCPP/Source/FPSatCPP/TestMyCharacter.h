// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSatCPPCharacter.h"
#include "TestMyCharacter.generated.h"

class UPhysicsHandleComponent;
class UPrimitiveComponent;
class UInputAction;
class UInputMappingContext;
class APlayerController;
struct FInputActionValue;

/** First person character with movement and look only. */
UCLASS()
class FPSATCPP_API ATestMyCharacter : public AFPSatCPPCharacter
{
	GENERATED_BODY()

public:
	ATestMyCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category="Grab")
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> GrabAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> GrabMappingContext;

	UPROPERTY(EditAnywhere, Category="Grab", meta=(ClampMin="50.0"))
	float GrabTraceDistance = 400.f;

	UPROPERTY(EditAnywhere, Category="Grab", meta=(ClampMin="50.0"))
	float HoldDistance = 180.f;

	UPROPERTY(EditAnywhere, Category="Grab", meta=(ClampMin="0.0"))
	float HoldDrop = 28.f;

	UPROPERTY(EditAnywhere, Category="Grab", meta=(ClampMin="100.0"))
	float ReleaseDistance = 450.f;

	void EnsureGrabInput(APlayerController* PlayerController);
	void ToggleGrab();
	void GrabStart();
	void GrabEnd();
	void UpdateHeldObject();
	bool IsHoldPathClear(const FVector& From, const FVector& To) const;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> HeldBody;

	FQuat HeldRelativeRotation = FQuat::Identity;
};
