// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/VisualChildActorComponent.h"

void UVisualChildActorComponent::CreateChildActor()
{
#if WITH_EDITOR
	if (GetOwner())
	{
		if (ULevel* Level = GetOwner()->GetLevel())
		{
			if (UWorld* World = Level->GetWorld())
			{
				if (World->WorldType == EWorldType::EditorPreview)
	            {
	                Super::CreateChildActor();
	            }
			}
		}
	}
#endif
}
