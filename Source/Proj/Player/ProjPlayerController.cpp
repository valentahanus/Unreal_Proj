// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjPlayerController.h"

#include "PlayerCharacter.h"
#include "ProjHUD.h"
#include "WeaponComponent.h"


void AProjPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetPawn() && GetPawn()->IsA<APlayerCharacter>())
	{
		APlayerCharacter* MyPlayer = Cast<APlayerCharacter>(GetPawn());

		MyPlayer->WeaponComponent->OnWeaponSelected.BindUObject(this, &AProjPlayerController::OnPlayerWeaponSelected);
	}
}

void AProjPlayerController::OnPlayerWeaponSelected(EWeapon WeaponIndex)
{
	if (GetHUD() && GetHUD()->IsA<AProjHUD>())
	{
		//AGameMode * MyGameMode = Cast(UGameplayStatics::GetGameMode(this)); 
		//MyHUD * hud = Cast(MyGameMode->HUDClass.GetDefaultObject());

		//ConstructorHelpers::FObjectFinder<UUserWidget>testWidget(TEXT("WidgetBlueprint'/Game/UMG/TestHUD.TestHUD'"));
		//testUMG = testWidget.Object;

		AProjHUD* ProjHUD = Cast<AProjHUD>(GetHUD());
		ProjHUD->OnWeaponSelected(WeaponIndex);
	}
}
