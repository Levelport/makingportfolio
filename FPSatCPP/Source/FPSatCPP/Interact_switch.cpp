// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact_switch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AInteract_switch::AInteract_switch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBounds"));

	SetRootComponent(InteractBounds);

	InteractBounds->SetBoxExtent(FVector(30.f, 30.f, 30.f));
	InteractBounds->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	InteractBounds->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	InteractBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(InteractBounds);
	ButtonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ButtonMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		ButtonMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void AInteract_switch::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateButton();
}

// Called when the game starts or when spawned
void AInteract_switch::BeginPlay()
{
	Super::BeginPlay();
	UpdateButton();
}

// Called every frame
void AInteract_switch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteract_switch::OnInteractRushE_Implementation(AActor* Interactor, bool bPressed)
{
	if (bTimedOn)
	{
		if (bPressed)
		{
			BeginTimedOn();
		}
		return;
	}

	if (!bPressed && PressMode != EFloorSwitchMode::WhilePressed)
	{
		return;
	}

	if (PressMode == EFloorSwitchMode::Toggle)
	{
		lit = !lit;
	}
	else if (PressMode == EFloorSwitchMode::WhilePressed)
	{
		lit = bPressed;
	}
	else if (PressMode == EFloorSwitchMode::StayOn)
	{
		lit = true;
	}
	else if (PressMode == EFloorSwitchMode::TurnOff)
	{
		lit = false;
	}


	UpdateButton();
	SendCommand(bPressed);
}

void AInteract_switch::BeginTimedOn()
{
	lit = true;
	UpdateButton();
	SendCommand(true, EFloorSwitchMode::WhilePressed);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(OnTimerHandle, this, &AInteract_switch::EndTimedOn, OnSeconds, false);
	}
}

void AInteract_switch::EndTimedOn()
{
	lit = false;
	UpdateButton();
	SendCommand(false, EFloorSwitchMode::WhilePressed);
}

void AInteract_switch::SendCommand(bool pressed)
{
	SendCommand(pressed, PressMode);
}

void AInteract_switch::SendCommand(bool pressed, EFloorSwitchMode Mode)
{
	for (AActor* TargetActor : Target)
	{
		if (TargetActor && TargetActor->GetClass()->ImplementsInterface(USwitchInterface::StaticClass()))
		{
			ISwitchInterface::Execute_OnSwitchCommand(TargetActor, this, Mode, pressed);
		}
	}
	OnSwitched(pressed && Mode != EFloorSwitchMode::TurnOff);
}

void AInteract_switch::UpdateButton()
{
	if (!ButtonMesh)
	{
		return;
	}

	if (!ButtonMaterial)
	{	
		if (UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
		{
			ButtonMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			ButtonMesh->SetMaterial(0, ButtonMaterial);
		}
	}

	if (!ButtonMaterial)
	{
		return;
	}

	const FLinearColor ButtonColor = lit ? OnColor : OffColor;
	ButtonMaterial->SetVectorParameterValue("Color", ButtonColor);
	ButtonMaterial->SetVectorParameterValue(TEXT("BaseColor"), ButtonColor);
}