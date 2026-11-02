// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "junctionSignInterface.h"
#include "JunctionSign.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UMaterialInstanceDynamic;

UCLASS()
class FPSATCPP_API AJunctionSign : public AActor, public IjunctionSignInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJunctionSign();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void OnJunctionCount_Implementation(int32 OnCount, int32 RequiredCount) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Sign")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category="Sign")
	TObjectPtr<UStaticMeshComponent> Board;

	UPROPERTY(VisibleAnywhere, Category="Sign")
	TObjectPtr<UTextRenderComponent> CountText;

	UPROPERTY(EditAnywhere, Category="Sign", meta=(DisplayName="文字色"))
	FLinearColor TextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category="Sign", meta=(DisplayName="文字サイズ", ClampMin="1.0"))
	float TextWorldSize = 70.f;

	void UpdateSign();
	void ShowRemaining(int32 Remaining);

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> BoardMaterial;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
