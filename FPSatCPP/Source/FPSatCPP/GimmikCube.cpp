// Fill out your copyright notice in the Description page of Project Settings.


#include "GimmikCube.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AGimmikCube::AGimmikCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetUseCCD(true);
	Mesh->SetMassOverrideInKg(NAME_None, 8.f, true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
}

// Called when the game starts or when spawned
void AGimmikCube::BeginPlay()
{
	Super::BeginPlay();

	if (!Mesh)
	{
		return;
	}

	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetUseCCD(true);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetMassOverrideInKg(NAME_None, 8.f, true);
	Mesh->OnComponentHit.RemoveDynamic(this, &AGimmikCube::OnMeshHit);
	Mesh->OnComponentHit.AddDynamic(this, &AGimmikCube::OnMeshHit);
	Mesh->WakeAllRigidBodies();
}

void AGimmikCube::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Mesh || bKicking || KickCooldown > 0.f || KickDistance <= 0.f || !Cast<APawn>(OtherActor))
	{
		return;
	}

	if (FMath::Abs(Hit.ImpactNormal.Z) > 0.6f)
	{
		return;
	}

	TInlineComponentArray<UPhysicsHandleComponent*> Handles;
	OtherActor->GetComponents(Handles);
	for (const UPhysicsHandleComponent* Handle : Handles)
	{
		if (Handle && Handle->GetGrabbedComponent() == Mesh)
		{
			return;
		}
	}

	FVector Direction = GetActorLocation() - OtherActor->GetActorLocation();
	Direction.Z = 0.f;
	if (!Direction.Normalize())
	{
		return;
	}

	KickOrigin = GetActorLocation();
	KickDirection = Direction;
	bKicking = true;

	const float KickSpeed = KickDistance / 0.2f;
	const FVector Velocity = Mesh->GetPhysicsLinearVelocity();
	Mesh->SetPhysicsLinearVelocity(FVector(Direction.X * KickSpeed, Direction.Y * KickSpeed, Velocity.Z));
}

// Called every frame
void AGimmikCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (KickCooldown > 0.f)
	{
		KickCooldown = FMath::Max(0.f, KickCooldown - DeltaTime);
		if (Mesh)
		{
			const FVector Velocity = Mesh->GetPhysicsLinearVelocity();
			Mesh->SetPhysicsLinearVelocity(FVector(0.f, 0.f, Velocity.Z));
		}
		return;
	}

	if (!bKicking || !Mesh)
	{
		return;
	}

	const FVector Delta = GetActorLocation() - KickOrigin;
	const float Traveled = FVector::DotProduct(FVector(Delta.X, Delta.Y, 0.f), KickDirection);
	if (Traveled < KickDistance)
	{
		return;
	}

	const FVector Stop = KickOrigin + KickDirection * KickDistance;
	FVector Location = GetActorLocation();
	Location.X = Stop.X;
	Location.Y = Stop.Y;
	SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector Velocity = Mesh->GetPhysicsLinearVelocity();
	Mesh->SetPhysicsLinearVelocity(FVector(0.f, 0.f, Velocity.Z));
	bKicking = false;
	KickCooldown = 0.45f;
}

