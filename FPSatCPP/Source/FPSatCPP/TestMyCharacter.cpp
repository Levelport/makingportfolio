// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GimmikCube.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
	bool TraceForGimmikCube(UWorld* World, AActor* IgnoredActor, const FVector& Start, const FVector& End, FHitResult& OutHit)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(GrabTrace), false, IgnoredActor);
		if (World->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params))
		{
			return Cast<AGimmikCube>(OutHit.GetActor()) != nullptr;
		}

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

		FHitResult PhysicsHit;
		if (!World->LineTraceSingleByObjectType(PhysicsHit, Start, End, ObjectParams, Params))
		{
			return false;
		}

		if (!Cast<AGimmikCube>(PhysicsHit.GetActor()))
		{
			return false;
		}

		OutHit = PhysicsHit;
		return true;
	}

	UPrimitiveComponent* PrepareGrabbableBody(AActor* Actor, UPrimitiveComponent* HitComponent)
	{
		auto EnablePhysics = [](UPrimitiveComponent* Component)
		{
			Component->SetMobility(EComponentMobility::Movable);
			Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Component->SetSimulatePhysics(true);
			Component->WakeAllRigidBodies();
		};

		if (HitComponent && HitComponent->GetOwner() == Actor)
		{
			if (!HitComponent->IsSimulatingPhysics())
			{
				EnablePhysics(HitComponent);
			}
			if (HitComponent->IsSimulatingPhysics())
			{
				return HitComponent;
			}
		}

		TInlineComponentArray<UPrimitiveComponent*> Components;
		Actor->GetComponents(Components);
		for (UPrimitiveComponent* Component : Components)
		{
			if (Component && Component->IsSimulatingPhysics())
			{
				return Component;
			}
		}

		for (UPrimitiveComponent* Component : Components)
		{
			if (!Component || !Component->IsA<UStaticMeshComponent>())
			{
				continue;
			}

			EnablePhysics(Component);
			if (Component->IsSimulatingPhysics())
			{
				return Component;
			}
		}

		return nullptr;
	}
}

ATestMyCharacter::ATestMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->InitialPushForceFactor = 120.f;
	GetCharacterMovement()->PushForceFactor = 2000.f;
	GetCharacterMovement()->bScalePushForceToVelocity = true;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandle->bInterpolateTarget = false;
	PhysicsHandle->bSoftLinearConstraint = true;
	PhysicsHandle->bSoftAngularConstraint = true;
	PhysicsHandle->LinearStiffness = 1800.f;
	PhysicsHandle->LinearDamping = 400.f;
	PhysicsHandle->AngularStiffness = 2400.f;
	PhysicsHandle->AngularDamping = 500.f;
}

void ATestMyCharacter::EnsureGrabInput(APlayerController* PlayerController)
{
	if (!PlayerController || !PlayerController->IsLocalPlayerController())
	{
		return;
	}

	if (!GrabMappingContext)
	{
		UInputAction* ClickAction = NewObject<UInputAction>(this);
		ClickAction->ValueType = EInputActionValueType::Boolean;
		GrabAction = ClickAction;

		GrabMappingContext = NewObject<UInputMappingContext>(this, TEXT("GrabMapping"));
		const FKey Keys[] =
		{
			EKeys::LeftMouseButton,
			// EKeys::E,
			// EKeys::RightMouseButton,
		};
		for (const FKey& Key : Keys)
		{
			FEnhancedActionKeyMapping& Mapping = GrabMappingContext->MapKey(ClickAction, Key);
			Mapping.Triggers.Add(NewObject<UInputTriggerDown>(GrabMappingContext));
		}
	}

	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			Subsystem->AddMappingContext(GrabMappingContext, 1);
		}
	}
}

void ATestMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	EnsureGrabInput(Cast<APlayerController>(GetController()));

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (GrabAction)
		{
			EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &ATestMyCharacter::ToggleGrab);
		}
	}
}

// Called every frame
void ATestMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHeldObject();
}

void ATestMyCharacter::ToggleGrab()
{
	if (HeldBody)
	{
		GrabEnd();
		return;
	}

	GrabStart();
}

void ATestMyCharacter::GrabStart()
{
	if (!PhysicsHandle || HeldBody || !GetWorld())
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	FVector Start;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(Start, ViewRotation);
	const FVector End = Start + ViewRotation.Vector() * GrabTraceDistance;

	FHitResult Hit;
	if (!TraceForGimmikCube(GetWorld(), this, Start, End, Hit))
	{
		return;
	}

	UPrimitiveComponent* Body = PrepareGrabbableBody(Hit.GetActor(), Hit.GetComponent());
	if (!Body)
	{
		return;
	}

	if (FVector::Dist(Start, Body->GetComponentLocation()) > ReleaseDistance)
	{
		return;
	}

	const FQuat ViewQuat = ViewRotation.Quaternion();
	HeldRelativeRotation = ViewQuat.Inverse() * Body->GetComponentQuat();
	Body->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	HeldBody = Body;
	PhysicsHandle->GrabComponentAtLocationWithRotation(Body, NAME_None, Body->GetComponentLocation(), Body->GetComponentRotation());

	if (PhysicsHandle->GetGrabbedComponent())
	{
		SetActorTickEnabled(true);
		UpdateHeldObject();
	}
	else
	{
		HeldBody = nullptr;
	}
}

void ATestMyCharacter::GrabEnd()
{
	if (PhysicsHandle)
	{
		PhysicsHandle->ReleaseComponent();
	}

	HeldBody = nullptr;
	SetActorTickEnabled(false);
}

bool ATestMyCharacter::IsHoldPathClear(const FVector& From, const FVector& To) const
{
	UWorld* World = GetWorld();
	if (!World || FVector::DistSquared(From, To) <= FMath::Square(4.f))
	{
		return true;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(GrabWall), false, this);
	if (HeldBody)
	{
		Params.AddIgnoredActor(HeldBody->GetOwner());
	}

	FHitResult WallHit;
	return !World->LineTraceSingleByChannel(WallHit, From, To, ECC_Visibility, Params);
}

void ATestMyCharacter::UpdateHeldObject()
{
	if (!PhysicsHandle || !IsValid(HeldBody))
	{
		GrabEnd();
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	if (FVector::Dist(ViewLocation, HeldBody->GetComponentLocation()) > ReleaseDistance)
	{
		GrabEnd();
		return;
	}

	const FVector Forward = ViewRotation.Vector();
	const FVector Up = FRotationMatrix(ViewRotation).GetUnitAxis(EAxis::Z);
	FVector Desired = ViewLocation + Forward * HoldDistance - Up * HoldDrop;

	if (const UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		const float Clearance = Capsule->GetScaledCapsuleRadius() + FMath::Max(20.f, HeldBody->Bounds.SphereRadius) + 8.f;
		const FVector FromCapsule = Desired - Capsule->GetComponentLocation();
		if (FromCapsule.SizeSquared() < FMath::Square(Clearance) && !FromCapsule.IsNearlyZero())
		{
			Desired = Capsule->GetComponentLocation() + FromCapsule.GetSafeNormal() * Clearance;
		}
	}

	const bool bReachable = IsHoldPathClear(ViewLocation, Desired) && IsHoldPathClear(HeldBody->GetComponentLocation(), Desired);
	if (!bReachable)
	{
		GrabEnd();
		return;
	}

	if (PhysicsHandle->GetGrabbedComponent() != HeldBody)
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HeldBody,
			NAME_None,
			HeldBody->GetComponentLocation(),
			HeldBody->GetComponentRotation());
	}

	const FQuat TargetRotation = ViewRotation.Quaternion() * HeldRelativeRotation;
	PhysicsHandle->SetTargetLocationAndRotation(Desired, TargetRotation.Rotator());
}
