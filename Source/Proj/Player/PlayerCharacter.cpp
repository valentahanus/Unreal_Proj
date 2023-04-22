// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

#include "DrawDebugHelpers.h"
#include "PlayerComponent.h"
#include "PlayerReplicationComponent.h"
#include "ProjHUD.h"
#include "WeaponComponent.h"
#include "Components/VisualChildActorComponent.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/PhysGun.h"

bool bDebugEquip = false;
FAutoConsoleVariableRef CVar_DebugEquip(TEXT("Proj.DebugEquip"), bDebugEquip,TEXT("Debug equipment console helper"), ECVF_Cheat);

//static TAutoConsoleVariable<bool> CVar_DebugEquip(TEXT("Proj.DebugEquip"), false,TEXT("Debug equipment console helper"), ECVF_Cheat);

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerReplicationComponent = CreateDefaultSubobject<UPlayerReplicationComponent>(TEXT("PlayerReplicationComponent"));

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterCamera"));
	Camera->SetupAttachment(GetRootComponent());

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(Camera);

	ConstraintDummy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConstraintDummy"));
	ConstraintDummy->SetupAttachment(PhysicsConstraint);

	ItemAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("ItemAnchor"));
	ItemAnchor->SetupAttachment(Camera);

	PhysGunAnchor = CreateDefaultSubobject<UVisualChildActorComponent>(TEXT("PhysGunAnchor"));
	PhysGunAnchor->SetupAttachment(ItemAnchor);
	PhysGunAnchor->SetAutoActivate(false);

	PistolAnchor = CreateDefaultSubobject<UVisualChildActorComponent>(TEXT("PistolAnchor"));
	PistolAnchor->SetupAttachment(ItemAnchor);
	PistolAnchor->SetAutoActivate(false);

	MultiplayerPistolAnchor = CreateDefaultSubobject<UVisualChildActorComponent>(TEXT("MultiplayerPistolAnchor"));
	MultiplayerPistolAnchor->SetupAttachment(GetMesh(), TEXT("hand_r_socket"));
	MultiplayerPistolAnchor->SetAutoActivate(false);

	MultiplayerPhysGunAnchor = CreateDefaultSubobject<UVisualChildActorComponent>(TEXT("MultiplayerPhysGunAnchor"));
	MultiplayerPhysGunAnchor->SetupAttachment(GetMesh(), TEXT("hand_r_socket"));
	MultiplayerPhysGunAnchor->SetAutoActivate(false);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UPlayerComponent*> Components;
	GetComponents<UPlayerComponent>(Components);
	for (UPlayerComponent* Component : Components)
	{
		Component->SetupPlayerComponent(this);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CVar_DebugEquip->GetBool() && GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ENSURE_NOTNULL(PC);
		ENSURE_NOTNULL(PC->GetHUD());

		FString Text;
		
		Text += FString::Printf(TEXT("Name: %s\nWeapon Selected: %i\nLocalNetRole: %s\nRemoteNetRole: %s\nNetMode: %s"),
			*GetName(),
			WeaponComponent->GetSelectedWeaponIndex(),
			*NetRoleToString(GetLocalRole()),
			*NetRoleToString(GetRemoteRole()),
			*MyNetModeToString(GetNetMode())
		);
		
		if (!IsLocallyControlled())
        {
			PC->GetHUD()->AddDebugText(
				Text,
				this,
				-1,
				GetActorLocation(),
				GetActorLocation(),
				FColor::Cyan,
				false,
				true,
				false,
				nullptr,
				1,
				true
			);
        }
        else
        {
        	ENSURE_NOTNULL(Cast<AProjHUD>(PC->GetHUD()));
        	Cast<AProjHUD>(PC->GetHUD())->AddDebugString(Text);
        }
	}
	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Unreal input called functions
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &APlayerCharacter::Fire);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &APlayerCharacter::Run);
	PlayerInputComponent->BindAxis("CameraHorizontal", this, &APlayerCharacter::RotateCharacterHorizontal);
	PlayerInputComponent->BindAxis("CameraVertical", this, &APlayerCharacter::RotateCameraVertical);
	PlayerInputComponent->BindAxis("MoveCharacterForward", this, &APlayerCharacter::MoveCharacterForward);
	PlayerInputComponent->BindAxis("MoveCharacterRight", this, &APlayerCharacter::MoveCharacterRight);
	PlayerInputComponent->BindAction("FirstSlotSelected", IE_Pressed, WeaponComponent, &UWeaponComponent::RequestFirstSlotSelected);
	PlayerInputComponent->BindAction("SecondSlotSelected", IE_Pressed, WeaponComponent, &UWeaponComponent::RequestSecondSlotSelected);
}

void APlayerCharacter::Fire()
{
	WeaponComponent->Fire(Camera->GetComponentRotation());
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

