// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/HUD.h"

bool bDebugEquip = false;
FAutoConsoleVariableRef CVar_DebugEquip(TEXT("Proj.DebugEquip"), bDebugEquip,TEXT("Debug equipment console helper"), ECVF_Cheat);

//static TAutoConsoleVariable<bool> CVar_DebugEquip(TEXT("Proj.DebugEquip"), false,TEXT("Debug equipment console helper"), ECVF_Cheat);

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterCamera"));
	Camera->SetupAttachment(GetRootComponent());

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(Camera);

	ConstraintDummy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConstraintDummy"));
	ConstraintDummy->SetupAttachment(PhysicsConstraint);

	ItemAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("ItemAnchor"));
	ItemAnchor->SetupAttachment(Camera);

	PhysGun = CreateDefaultSubobject<UChildActorComponent>(TEXT("PhysGun"));
	PhysGun->SetupAttachment(ItemAnchor);

	Pistol = CreateDefaultSubobject<UChildActorComponent>(TEXT("Pistol"));
	Pistol->SetupAttachment(ItemAnchor);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnWeaponSelected.ExecuteIfBound(0);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Server_SendRotation(GetActorRotation().Yaw, GetActorRotation().Pitch);

	if (CVar_DebugEquip->GetBool())
	{
		if (!IsLocallyControlled())
        {
			ENSURE_NOTNULL(GetController());
			ENSURE_NOTNULL(Cast<APlayerController>(GetController()));
			ENSURE_NOTNULL(Cast<APlayerController>(GetController())->GetHUD());

			// TODO put ensure macros everywhere
			
			// TODO finish add debug text (look inside DrawDebugString sting)
			Cast<APlayerController>(GetController())->GetHUD()->AddDebugText(
				TEXT("test3"),
				0,
				-1,
				GetActorLocation(),
				GetActorLocation(),
				FColor::Cyan,
				/*bSkipOverwriteCheck=*/ true,
				/*bAbsoluteLocation=*/ false,
				/*bKeepAttachedToActor=*/ false,
				nullptr,
				1,
				false
			);
        	DrawDebugString(GetWorld(), GetActorLocation(), "test", nullptr, FColor::Cyan, -1, true, 5);
        }
        else
        {
        	GEngine->AddOnScreenDebugMessage(reinterpret_cast<uint64>(this), -1, FColor::Cyan,"test2");
        }
	}
	
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
	PlayerInputComponent->BindAction("FirstSlotSelected", IE_Pressed, this, &APlayerCharacter::RequestFirstSlotSelected);
	PlayerInputComponent->BindAction("SecondSlotSelected", IE_Pressed, this, &APlayerCharacter::RequestSecondSlotSelected);
}

void APlayerCharacter::OnRep_SelectedWeapon()
{
	switch (SelectedWeapon)
	{
	case 0:
	{
		FirstSlotSelected();
	}
	break;
		
	case 1:
	{
		SecondSlotSelected();
	}
	break;

	default:
	{
		check(false);
	}
	}
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

	//TraceResult.GetActor()->SetActorLocation(TraceLocation);
	
	PhysicsConstraint->SetConstrainedComponents(
		Cast<UPrimitiveComponent>(TraceResult.GetActor()->GetRootComponent()),
		FName(),
		Cast<UPrimitiveComponent>(ConstraintDummy),
		FName()
	);

	PlayPickUpVFX();
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

void APlayerCharacter::Server_SendRotation_Implementation(double Yaw, double Pitch)
{
	SetActorRotation(FRotator(0,Yaw,0));
}

void APlayerCharacter::FirstSlotSelected()
{
	Pistol->SetHiddenInGame(true, true);
	PhysGun->SetHiddenInGame(false, true);
	
	OnWeaponSelected.ExecuteIfBound(0);
}

void APlayerCharacter::SecondSlotSelected()
{
	PhysGun->SetHiddenInGame(true, true);
	Pistol->SetHiddenInGame(false, true);
	
	OnWeaponSelected.ExecuteIfBound(1);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, SelectedWeapon);
}

void APlayerCharacter::Server_RequestWeaponChange_Implementation(uint8 WeaponIndex)
{
	if (WeaponIndex > 1)
	{
		return;
	}

	SelectedWeapon = WeaponIndex;

	if (IsLocallyControlled())
	{
		OnRep_SelectedWeapon();
	}
}

void APlayerCharacter::SelectWeapon(uint8 WeaponIndex)
{
	Server_RequestWeaponChange(WeaponIndex);
}

