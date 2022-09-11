// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterCamera"));
	Camera->SetupAttachment(GetRootComponent());

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(Camera);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Unreal input called functions
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("PickUp", EInputEvent::IE_Pressed, this, &APlayerCharacter::PickUp);
	PlayerInputComponent->BindAction("Drop", EInputEvent::IE_Pressed, this, &APlayerCharacter::Drop);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &APlayerCharacter::Run);
	PlayerInputComponent->BindAxis("CameraHorizontal", this, &APlayerCharacter::RotateCharacterHorizontal);
	PlayerInputComponent->BindAxis("CameraVertical", this, &APlayerCharacter::RotateCameraVertical);
	PlayerInputComponent->BindAxis("MoveCharacterForward", this, &APlayerCharacter::MoveCharacterForward);
	PlayerInputComponent->BindAxis("MoveCharacterRight", this, &APlayerCharacter::MoveCharacterRight);
}

void APlayerCharacter::PickUp()
{
	FCollisionQueryParams QueryParams = FCollisionQueryParams();
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bIgnoreTouches = true;
	// TODO Add ignore tags here 
	QueryParams.AddIgnoredActor(this);
	
	FHitResult TraceResult;
	FRotator CameraRotation = Camera->GetComponentRotation();
	FVector CameraLocation = Camera->GetComponentLocation();

	double PickupRange = 200;

	FVector CameraDirection = CameraRotation.RotateVector(FVector(1, 0, 0));
	FVector TraceOffset = CameraDirection * PickupRange;
	FVector TraceLocation = CameraLocation + TraceOffset;
	
	bool ValidHit = GetWorld()->LineTraceSingleByChannel(TraceResult, CameraLocation, TraceLocation, PickUpChannel, QueryParams);

	// Action called if it hits something
	if (!TraceResult.GetActor())
	{
		return;
	}

	TraceResult.GetActor()->SetActorLocation(TraceLocation);
	
	PhysicsConstraint->SetConstrainedComponents(
		Cast<UPrimitiveComponent>(TraceResult.GetActor()->GetRootComponent()),
		FName(),
		Cast<UPrimitiveComponent>(Camera),
		FName()
	);
}

void APlayerCharacter::Drop()
{
	PhysicsConstraint->BreakConstraint();
	UE_LOG(LogTemp, Warning, TEXT("Drop"));
}

void APlayerCharacter::Run()
{
	// In work
	UE_LOG(LogTemp, Warning, TEXT("Run activated"));
}

void APlayerCharacter::RotateCharacterHorizontal(float Axis)
{
	FRotator CharacterRotation = this->GetActorRotation();
	
	// Camera rotation yaw equals to axis by this mess
	CharacterRotation.Yaw += Axis * GetWorld()->GetDeltaSeconds() * CharacterRotationSpeed;
	CharacterRotation.Roll = 0;
	
	this->SetActorRotation(CharacterRotation);
}

void APlayerCharacter::RotateCameraVertical(float Axis)
{
	// UE_LOG(LogTemp, Warning, TEXT("%f"), Axis);
	FRotator CameraRotation = Camera->GetComponentRotation();

	// Camera is multiplied by delta-seconds and configurable cameraspeed
	CameraRotation.Pitch += Axis * GetWorld()->GetDeltaSeconds() * CharacterRotationSpeed;
	
	CameraRotation.Pitch = FMath::Clamp(CameraRotation.Pitch, PitchLimits.X, PitchLimits.Y);
	CameraRotation.Roll = 0;
	
	Camera->SetWorldRotation(CameraRotation);
}

void APlayerCharacter::MoveCharacterForward(float Axis)
{
	FRotator CameraRotation = Camera->GetComponentRotation();
	FVector CharacterLocationOffset = CameraRotation.RotateVector(FVector(1, 0, 0));
	CharacterLocationOffset *= Axis * PlayerSpeed * GetWorld()->GetDeltaSeconds();
	
	GetCharacterMovement()->AddInputVector(CharacterLocationOffset);
}

void APlayerCharacter::MoveCharacterRight(float Axis)
{
	FVector Direction = FVector::CrossProduct(FVector(0, 0, 1), Camera->GetComponentTransform().GetRotation().GetForwardVector());
	Direction *= Axis * PlayerSpeed * GetWorld()->GetDeltaSeconds();
	GetCharacterMovement()->AddInputVector(Direction);
}
