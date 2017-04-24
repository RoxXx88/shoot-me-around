#include "ShootMeAround.h"
#include "ShootMeAroundCharacter.h"

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#include "Components/BillboardComponent.h"

#include "Engine/StaticMeshActor.h"

#include "ComponentController.h"

AComponentController::AComponentController()
{
	PrimaryActorTick.bCanEverTick = true;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));

	RightTranslation = 0.f;
	FrontTranslation = 0.f;
}

void AComponentController::AddBullet(AActor* NewBullet)
{
	OriginalBulletInstances.Add(NewBullet);

	if (ActivateCopy)
	{
		for (ArrayOfTranslatedObject &ToAddBullet : CopiedBulletInstances)
		{
			AActor* CopiedBullet = SpawnCopy(NewBullet, ToAddBullet.Value);
			ToAddBullet.Key.Add(CopiedBullet);
		}
	}

}

void AComponentController::BeginPlay()
{
	Super::BeginPlay();

	FillOriginalObjectsArray();

	if (ActivateCopy)
	{
		CreateCopiedInstances();
	}
}


void AComponentController::FillOriginalObjectsArray()
{
	FConstPlayerControllerIterator PlayerControllers = GetWorld()->GetPlayerControllerIterator();

	for (int PlayerControllerIndex = 0; PlayerControllerIndex < NumPlayers; ++PlayerControllerIndex, ++PlayerControllers)
	{
		TAutoWeakObjectPtr<APlayerController> ActualPlayer = *PlayerControllers;
		ACharacter* PlayerCharacter = ActualPlayer->GetCharacter();
		OriginalCharacterInstances.Add(PlayerCharacter);
	}

	for (AActor *Object : Objects)
	{
		OriginalObjectInstances.Add(Object);
	}
}

void AComponentController::CreateCopiedInstances()
{
	RightTranslation = World->GetComponentsBoundingBox().GetSize().Y;
	FrontTranslation = World->GetComponentsBoundingBox().GetSize().X;

	//Right  

	CreateCopiedInstancesWithTranslation(0, RightTranslation);

	//Left  

	CreateCopiedInstancesWithTranslation(0, -RightTranslation);

	//Front  

	CreateCopiedInstancesWithTranslation(FrontTranslation, 0);

	//Behind  

	CreateCopiedInstancesWithTranslation(-FrontTranslation, 0);

	//Front-Right  

	CreateCopiedInstancesWithTranslation(FrontTranslation, RightTranslation);

	//Front-Left  

	CreateCopiedInstancesWithTranslation(FrontTranslation, -RightTranslation);

	//Behind-Right  

	CreateCopiedInstancesWithTranslation(-FrontTranslation, RightTranslation);

	//Behind-Left  

	CreateCopiedInstancesWithTranslation(-FrontTranslation, -RightTranslation);


}

void AComponentController::CreateCopiedInstancesWithTranslation(float XTranslation, float YTranslation)
{

	FActorSpawnParameters WorldParameters;
	WorldParameters.Template = World;
	AStaticMeshActor * TranslatedWorld = GetWorld()->SpawnActor<class AStaticMeshActor>(World->GetClass(), WorldParameters);
	TranslatedWorld->SetMobility(EComponentMobility::Movable);

	FVector WorldLocationToTranslate = TranslatedWorld->GetActorLocation();
	WorldLocationToTranslate.X += XTranslation;
	WorldLocationToTranslate.Y += YTranslation;
	TranslatedWorld->SetActorLocation(WorldLocationToTranslate);

	ArrayOfTranslatedObject TranslatedCopiedObjectInstances;
	ArrayOfTranslatedCharacter TranslatedCopiedCharacterInstances;
	ArrayOfTranslatedObject TranslatedCopiedBulletInstances;

	TranslatedCopiedObjectInstances.Value = TranslatedWorld->GetActorLocation() - World->GetActorLocation();
	TranslatedCopiedCharacterInstances.Value = TranslatedWorld->GetActorLocation() - World->GetActorLocation();
	TranslatedCopiedBulletInstances.Value = TranslatedWorld->GetActorLocation() - World->GetActorLocation();

	for (AActor* Object : OriginalObjectInstances)
	{
		TranslatedCopiedObjectInstances.Key.Add(SpawnCopy(Object, TranslatedCopiedObjectInstances.Value, true));
	}

	for (AActor* Character : OriginalCharacterInstances)
	{
		AActor* ToAdd = Cast<AActor>(SpawnCopy(Character, TranslatedCopiedCharacterInstances.Value));
		TranslatedCopiedCharacterInstances.Key.Add(Cast<ACharacter>(ToAdd));
	}

	CopiedObjectInstances.Add(TranslatedCopiedObjectInstances);
	CopiedCharacterInstances.Add(TranslatedCopiedCharacterInstances);
	CopiedBulletInstances.Add(TranslatedCopiedBulletInstances);
}

AActor* AComponentController::SpawnCopy(AActor* OriginalObject, FVector Translation, bool SetTemplate)
{
	if (OriginalObject == nullptr) return nullptr;

	FActorSpawnParameters ObjectParameters;
	AActor* TranslatedObject;
	if (SetTemplate)
	{
		ObjectParameters.Template = OriginalObject;
		TranslatedObject = GetWorld()->SpawnActor<class AActor>(OriginalObject->GetClass(), ObjectParameters);
	}
	else
	{
		TranslatedObject = GetWorld()->SpawnActor<class AActor>(OriginalObject->GetClass(), OriginalObject->GetActorTransform(), ObjectParameters);
	}

	FVector ObjectLocationToTranslate = TranslatedObject->GetActorLocation();
	ObjectLocationToTranslate += Translation;
	TranslatedObject->SetActorLocation(ObjectLocationToTranslate);

	return TranslatedObject;
}

void AComponentController::TranslateObject(AActor* ToTranslate, AActor* OriginalObject, const FVector &TranslateValue)
{
	if (ToTranslate != nullptr && ToTranslate->IsValidLowLevel())
	{
		if (OriginalObject->IsValidLowLevel())
		{
			FTransform TransformToTranslate = OriginalObject->GetActorTransform();
			TransformToTranslate.SetLocation(TransformToTranslate.GetLocation() - TranslateValue);
			ToTranslate->SetActorTransform(TransformToTranslate);
		}
	}
}

void AComponentController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RightTranslation = World->GetComponentsBoundingBox().GetSize().Y;
	FrontTranslation = World->GetComponentsBoundingBox().GetSize().X;

	TeleportObjectsIfEscaping();

	UpdateTriggerLimit();

	if (ActivateCopy)
	{
		UpdateCopyTransform();
	}
}

void AComponentController::UpdateCopyTransform()
{
	int OriginalObjectCounter = 0;

	for (ArrayOfTranslatedObject ObjectsToTranslate : CopiedObjectInstances)
	{

		OriginalObjectCounter = 0;

		for (AActor* ToTranslate : ObjectsToTranslate.Key)
		{
			TranslateObject(ToTranslate, OriginalObjectInstances[OriginalObjectCounter], ObjectsToTranslate.Value);

			++OriginalObjectCounter;
		}

	}

	for (ArrayOfTranslatedCharacter CharacterToTranslate : CopiedCharacterInstances)
	{
		OriginalObjectCounter = 0;

		for (ACharacter* ToTranslate : CharacterToTranslate.Key)
		{
			TranslateObject(ToTranslate, OriginalCharacterInstances[OriginalObjectCounter], CharacterToTranslate.Value);

			ToTranslate->GetMovementComponent()->Velocity = OriginalCharacterInstances[OriginalObjectCounter]->GetMovementComponent()->Velocity;

			++OriginalObjectCounter;
		}

	}

	for (ArrayOfTranslatedObject BulletToTranslate : CopiedBulletInstances)
	{

		OriginalObjectCounter = 0;

		for (AActor* ToTranslate : BulletToTranslate.Key)
		{
			TranslateObject(ToTranslate, OriginalBulletInstances[OriginalObjectCounter], BulletToTranslate.Value);

			++OriginalObjectCounter;
		}

	}
}

void AComponentController::TeleportObjectsIfEscaping()
{
	for (AActor* Object : OriginalObjectInstances)
	{
		TeleportIfEscaping(Object);
	}

	for (AActor* Object : OriginalCharacterInstances)
	{
		TeleportIfEscaping(Object);
	}

	for (AActor* Object : OriginalBulletInstances)
	{
		TeleportIfEscaping(Object);
	}
}

void AComponentController::TeleportIfEscaping(AActor* Object)
{
	if (Object == nullptr || !Object->IsValidLowLevel())
	{
		return;
	}

	FVector CurrLocation = Object->GetActorLocation();

	// Check top.

	if (CurrLocation.X > FrontTranslation / 2.0f)
	{
		FVector NewLocation = CurrLocation;
		NewLocation.X -= FrontTranslation;

   		Object->SetActorLocation(NewLocation);
	}
	else if (CurrLocation.X < -FrontTranslation / 2.0f) // Check bottom.
	{
		FVector NewLocation = CurrLocation;
		NewLocation.X += FrontTranslation;

		Object->SetActorLocation(NewLocation);
	}

	// Check right.

	if (CurrLocation.Y > RightTranslation / 2.0f)
	{
		FVector NewLocation = CurrLocation;
		NewLocation.Y -= RightTranslation;

		Object->SetActorLocation(NewLocation);
	}
	else if (CurrLocation.Y < -RightTranslation / 2.0f) // Check left.
	{
		FVector NewLocation = CurrLocation;
		NewLocation.Y += RightTranslation;

		Object->SetActorLocation(NewLocation);
	}
}

void AComponentController::SimulateEventOnCopy(ACharacter* Original, EventType Event)
{

	int OriginalObjectCounter = 0;

	for (ACharacter* Searched : OriginalCharacterInstances)
	{
		if (Original == Searched)
		{
			for (ACharacter* ToEvent : CopiedCharacterInstances[OriginalObjectCounter].Key)
			{
				switch (Event)
				{
				case EventType::START_FIRE:
					break;
				case EventType::STOP_FIRE:
					break;
				case EventType::CROUCH:
					break;
				case EventType::UNCROUCH:
					break;
				}
			}
			break;
		}

		++OriginalObjectCounter;
	}
}

void AComponentController::StartFire(ACharacter* Which)
{

	if (ActivateCopy)
	{
		SimulateEventOnCopy(Which, EventType::START_FIRE);
	}

}

void AComponentController::StopFire(ACharacter* Which)
{

	if (ActivateCopy)
	{
		SimulateEventOnCopy(Which, EventType::STOP_FIRE);
	}

}

void AComponentController::Crouch(ACharacter* Which)
{

	if (ActivateCopy)
	{
		SimulateEventOnCopy(Which, EventType::CROUCH);
	}

}

void AComponentController::UnCrouch(ACharacter* Which)
{

	if (ActivateCopy)
	{
		SimulateEventOnCopy(Which, EventType::UNCROUCH);
	}

}

void AComponentController::UpdateTriggerLimit()
{
	RightTranslation = World->GetComponentsBoundingBox().GetSize().Y;
	FrontTranslation = World->GetComponentsBoundingBox().GetSize().X;

	float WidthRadius = FrontTranslation / 2.0f;
	float HeightRadius = RightTranslation / 2.0f;

	FVector WorldLocation = World->GetActorLocation();

	float TriggerLimitZ = PositiveX->GetActorLocation().Z;

	PositiveX->SetActorLocation({ WorldLocation.X + WidthRadius, WorldLocation.Y, TriggerLimitZ });
	PositiveY->SetActorLocation({ WorldLocation.X, WorldLocation.Y + HeightRadius, TriggerLimitZ });

	NegativeX->SetActorLocation({ WorldLocation.X - WidthRadius, WorldLocation.Y, TriggerLimitZ });
	NegativeY->SetActorLocation({ WorldLocation.X, WorldLocation.Y - HeightRadius, TriggerLimitZ });
}