// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineSign.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASplineSign::ASplineSign()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Path = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	Path->SetupAttachment(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (DefaultMesh.Succeeded())
	{
		LineMesh = DefaultMesh.Object;
	}
}

void ASplineSign::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RefreshPath(true);
}

// Called when the game starts or when spawned
void ASplineSign::BeginPlay()
{
	Super::BeginPlay();
	DriveState.bLatched = bStartOn;
	bLit = bStartOn;
	RefreshPath(true);
}

// Called every frame
void ASplineSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RefreshPath(false);
}

bool ASplineSign::ShouldTickIfViewportsOnly() const
{
	return true;
}

void ASplineSign::OnSwitchCommand_Implementation(AActor* Source, EFloorSwitchMode Mode, bool bPressed)
{
	SetLit(DriveState.Apply(Source, Mode, bPressed));
}

void ASplineSign::SetLit(bool bNewLit)
{
	bLit = bNewLit;
	ApplyColor();
}

bool ASplineSign::IsShowingOn() const
{
	const UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
	{
		return bLit;
	}

	return bStartOn;
}

FVector ASplineSign::GetCollisionFloorPoint(const AActor* Actor) const
{
	FVector Point = Actor->GetActorLocation();

	TInlineComponentArray<UPrimitiveComponent*> Primitives;
	Actor->GetComponents(Primitives);

	bool bFoundCollision = false;
	float FloorZ = Point.Z;

	for (const UPrimitiveComponent* Primitive : Primitives)
	{
		if (!IsValid(Primitive) || Primitive->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
		{
			continue;
		}

		FloorZ = bFoundCollision ? FMath::Min(FloorZ, Primitive->Bounds.GetBox().Min.Z) : Primitive->Bounds.GetBox().Min.Z;
		bFoundCollision = true;
	}

	if (bFoundCollision)
	{
		Point.Z = FloorZ;
	}

	return Point;
}

void ASplineSign::RefreshPath(bool bForceRebuild)
{
	if (!Path)
	{
		return;
	}

	if (Path->GetNumberOfSplinePoints() < 2)
	{
		Path->ClearSplinePoints(false);
		Path->AddSplinePoint(FVector::ZeroVector, ESplineCoordinateSpace::Local, false);
		Path->AddSplinePoint(FVector(100.f, 0.f, 0.f), ESplineCoordinateSpace::Local, true);
	}

	bool bEndpointsMoved = false;

	if (IsValid(StartActor))
	{
		const FVector LocalStart = GetActorTransform().InverseTransformPosition(GetCollisionFloorPoint(StartActor));
		if (!Path->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local).Equals(LocalStart, 0.1f))
		{
			Path->SetLocationAtSplinePoint(0, LocalStart, ESplineCoordinateSpace::Local, true);
			bEndpointsMoved = true;
		}
	}

	const int32 LastPoint = Path->GetNumberOfSplinePoints() - 1;
	if (LastPoint > 0 && IsValid(EndActor))
	{
		const FVector LocalEnd = GetActorTransform().InverseTransformPosition(GetCollisionFloorPoint(EndActor));
		if (!Path->GetLocationAtSplinePoint(LastPoint, ESplineCoordinateSpace::Local).Equals(LocalEnd, 0.1f))
		{
			Path->SetLocationAtSplinePoint(LastPoint, LocalEnd, ESplineCoordinateSpace::Local, true);
			bEndpointsMoved = true;
		}
	}

	const int32 NeededSegments = Path->GetNumberOfSplinePoints() - 1;
	const bool bSegmentCountChanged = LineSegments.Num() != NeededSegments;
	const bool bMeshMissing = NeededSegments > 0 && (LineSegments.Num() == 0 || !IsValid(LineSegments[0]) || LineSegments[0]->GetStaticMesh() != LineMesh);

	if (bForceRebuild || bSegmentCountChanged || bMeshMissing)
	{
		RebuildMeshes();
		return;
	}

	if (bEndpointsMoved)
	{
		UpdateSegmentTransforms();
	}
}

void ASplineSign::RebuildMeshes()
{
	TArray<USplineMeshComponent*> ExistingSegments;
	GetComponents(ExistingSegments);
	for (USplineMeshComponent* Segment : ExistingSegments)
	{
		if (IsValid(Segment))
		{
			Segment->DestroyComponent();
		}
	}
	LineSegments.Reset();

	if (!Path || !LineMesh || !GetWorld())
	{
		return;
	}

	const int32 NumPoints = Path->GetNumberOfSplinePoints();
	for (int32 Index = 0; Index < NumPoints - 1; ++Index)
	{
		USplineMeshComponent* Segment = NewObject<USplineMeshComponent>(this);
		Segment->SetStaticMesh(LineMesh);
		Segment->SetMobility(EComponentMobility::Movable);
		Segment->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		Segment->SetForwardAxis(ESplineMeshAxis::X, false);
		Segment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Segment->SetCastShadow(false);
		Segment->SetupAttachment(Path);
		Segment->RegisterComponentWithWorld(GetWorld());
		LineSegments.Add(Segment);
	}

	UpdateSegmentTransforms();
	ApplyColor();
}

void ASplineSign::UpdateSegmentTransforms()
{
	if (!Path)
	{
		return;
	}

	const FVector2D Thickness(LineThickness, LineThickness);
	const int32 NumPoints = Path->GetNumberOfSplinePoints();

	for (int32 Index = 0; Index < LineSegments.Num() && Index < NumPoints - 1; ++Index)
	{
		USplineMeshComponent* Segment = LineSegments[Index];
		if (!IsValid(Segment))
		{
			continue;
		}

		const FVector StartPos = Path->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::Local);
		const FVector StartTangent = Path->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::Local);
		const FVector EndPos = Path->GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = Path->GetTangentAtSplinePoint(Index + 1, ESplineCoordinateSpace::Local);

		Segment->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, false);
		Segment->SetStartScale(Thickness, false);
		Segment->SetEndScale(Thickness, true);
	}
}

void ASplineSign::ApplyColor()
{
	if (!LineMaterial)
	{
		return;
	}

	if (!LineMaterialInstance || LineMaterialInstance->Parent != LineMaterial)
	{
		LineMaterialInstance = UMaterialInstanceDynamic::Create(LineMaterial, this);
	}

	if (!LineMaterialInstance)
	{
		return;
	}

	LineMaterialInstance->SetVectorParameterValue(ColorParameterName, IsShowingOn() ? ColorOn : ColorOff);

	for (USplineMeshComponent* Segment : LineSegments)
	{
		if (IsValid(Segment))
		{
			Segment->SetMaterial(0, LineMaterialInstance);
			Segment->MarkRenderStateDirty();
		}
	}
}
