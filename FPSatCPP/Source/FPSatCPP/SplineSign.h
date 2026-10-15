// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwitchInterface.h"
#include "SplineSign.generated.h"

class USplineComponent;
class USplineMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class FPSATCPP_API ASplineSign : public AActor, public ISwitchInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASplineSign();

	virtual void OnConstruction(const FTransform& Transform) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	virtual void OnSwitchCommand_Implementation(EFloorSwitchMode Mode, bool bPressed) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="SplineSign")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category="SplineSign")
	TObjectPtr<USplineComponent> Path;

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="start"))
	TObjectPtr<AActor> StartActor;

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="end"))
	TObjectPtr<AActor> EndActor;

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="mesh"))
	TObjectPtr<UStaticMesh> LineMesh;

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="mat"))
	TObjectPtr<UMaterialInterface> LineMaterial;

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="collor"))
	FName ColorParameterName = TEXT("Color");

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="on_col"))
	FLinearColor ColorOn = FLinearColor(0.1f, 0.85f, 1.f);

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="off_col"))
	FLinearColor ColorOff = FLinearColor(0.02f, 0.02f, 0.02f);

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="thic", ClampMin="0.001"))
	float LineThickness = 0.02f;

	UPROPERTY(EditAnywhere, Category="SplineSign", meta=(DisplayName="IsOnFromStart"))
	bool bStartOn = false;

	FVector GetCollisionFloorPoint(const AActor* Actor) const;
	void RefreshPath(bool bForceRebuild);
	void RebuildMeshes();
	void UpdateSegmentTransforms();
	void ApplyColor();
	void SetLit(bool bNewLit);
	bool IsShowingOn() const;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> LineSegments;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> LineMaterialInstance;

	bool bLit = false;
};
