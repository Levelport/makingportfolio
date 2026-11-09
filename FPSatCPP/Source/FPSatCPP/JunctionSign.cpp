// Fill out your copyright notice in the Description page of Project Settings.


#include "JunctionSign.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AJunctionSign::AJunctionSign()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Board = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Board"));
	Board->SetupAttachment(SceneRoot);
	Board->SetMobility(EComponentMobility::Movable);
	Board->SetSimulatePhysics(false);
	Board->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Board->SetCollisionObjectType(ECC_WorldDynamic);
	Board->SetCollisionResponseToAllChannels(ECR_Block);
	Board->CanCharacterStepUpOn = ECB_No;
	Board->SetRelativeScale3D(FVector(0.08f, 1.4f, 1.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoardMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (BoardMesh.Succeeded())
	{
		Board->SetStaticMesh(BoardMesh.Object);
	}

	CountText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountText"));
	CountText->SetupAttachment(SceneRoot);
	CountText->SetRelativeLocation(FVector(6.f, 0.f, 0.f));
	CountText->SetHorizontalAlignment(EHTA_Center);
	CountText->SetVerticalAlignment(EVRTA_TextCenter);
	CountText->SetWorldSize(TextWorldSize);
	CountText->SetTextRenderColor(TextColor.ToFColor(true));
	CountText->SetCastShadow(false);
	CountText->SetText(FText::FromString(TEXT("0")));

	static ConstructorHelpers::FObjectFinder<UFont> CountFont(TEXT("/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField"));
	if (CountFont.Succeeded())
	{
		CountText->SetFont(CountFont.Object);
	}
}

void AJunctionSign::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateSign();
}

// Called when the game starts or when spawned
void AJunctionSign::BeginPlay()
{
	Super::BeginPlay();
	UpdateSign();
}

// Called every frame
void AJunctionSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJunctionSign::OnJunctionCount_Implementation(int32 OnCount, int32 RequiredCount)
{
	const int32 Remaining = FMath::Max(0, RequiredCount - OnCount);
	ShowRemaining(Remaining);
}

void AJunctionSign::UpdateSign()
{
	if (Board)
	{
		if (!BoardMaterial)
		{
			if (UMaterialInterface* Base = LoadObject<UMaterialInterface>
				(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
			{
				BoardMaterial = UMaterialInstanceDynamic::Create(Base, this);
			}
		}

		if (BoardMaterial)
		{
			Board->SetMaterial(0, BoardMaterial);
			const FLinearColor BoardColor(0.02f, 0.02f, 0.02f);
			BoardMaterial->SetVectorParameterValue(TEXT("Color"), BoardColor);
			BoardMaterial->SetVectorParameterValue(TEXT("BaseColor"), BoardColor);
		}
	}

	if (CountText)
	{
		CountText->SetWorldSize(TextWorldSize);
		CountText->SetTextRenderColor(TextColor.ToFColor(true));
		CountText->SetHorizontalAlignment(EHTA_Center);
		CountText->SetVerticalAlignment(EVRTA_TextCenter);
	}
}

void AJunctionSign::ShowRemaining(int32 Remaining)
{
	UpdateSign();
	if (CountText)
	{
		CountText->SetText(FText::FromString(FString::FromInt(Remaining)));
	}
}
