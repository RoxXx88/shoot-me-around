#include "ShootMeAround.h"
#include "Array.h"
#include "Vector.h"
#include "GameFramework/Actor.h"
#include "Containers/ContainersFwd.h"
#include "GameFramework/PlayerController.h"
#include "Components/BillboardComponent.h"
#include "ShootMeAroundCharacter.h"
#include "Engine/StaticMeshActor.h"
#include "ComponentController.h"

const float AComponentController::MAX_TIMER_UPDATE_COPY = 0.0001f;

void AComponentController::AddBullet(AActor * NewBullet)
{
	OriginalBulletInstances.Add(NewBullet);

	for (ArrayOfTranslatedObject &ToAddBullet : CopiedBulletInstances)
	{
		AActor* CopiedBullet = SpawnCopy(NewBullet, ToAddBullet.Value);
		ToAddBullet.Key.Add(CopiedBullet);
	}

}

AComponentController::AComponentController()
{
	PrimaryActorTick.bCanEverTick = true;
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));

	this->SetActorTickInterval(MAX_TIMER_UPDATE_COPY);

}

void AComponentController::BeginPlay()
{
	Super::BeginPlay();

	FillOriginalObjectsArray();
	CreateCopiedInstances();

}

void AComponentController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TeleportObjectsIfEscaping();

	UpdateCopyTransform();

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

void AComponentController::FillOriginalObjectsArray()
{

	FConstPlayerControllerIterator PlayerControllers = GetWorld()->GetPlayerControllerIterator();
	for (int PlayerControllerIndex = 0; PlayerControllerIndex < NumberOfPlayers; ++PlayerControllerIndex)
	{
		TAutoWeakObjectPtr<APlayerController> ActualPlayer = *PlayerControllers;
		ACharacter* PlayerCharacter = ActualPlayer->GetCharacter();
		PlayerCharacter->GetMesh()->MasterPoseComponent;
		OriginalCharacterInstances.Add(PlayerCharacter);
		PlayerControllers++;
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

	if (Object == nullptr) return;

	if (PositiveX->IsOverlappingActor(Object))
	{
		Object->SetActorLocation(Object->GetActorLocation() + FVector(-FrontTranslation, 0, 0));
	}
	else if (NegativeX->IsOverlappingActor(Object))
	{
		Object->SetActorLocation(Object->GetActorLocation() + FVector(FrontTranslation, 0, 0));
	}

	if (PositiveY->IsOverlappingActor(Object))
	{
		Object->SetActorLocation(Object->GetActorLocation() + FVector(0, -RightTranslation, 0));
	}
	else if (NegativeY->IsOverlappingActor(Object))
	{
		Object->SetActorLocation(Object->GetActorLocation() + FVector(0, RightTranslation, 0));
	}
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

	SimulateEventOnCopy(Which, EventType::START_FIRE);

}

void AComponentController::StopFire(ACharacter* Which)
{

	SimulateEventOnCopy(Which, EventType::STOP_FIRE);

}

void AComponentController::Crouch(ACharacter* Which)
{

	SimulateEventOnCopy(Which, EventType::CROUCH);

}

void AComponentController::UnCrouch(ACharacter* Which)
{

	SimulateEventOnCopy(Which, EventType::UNCROUCH);

}


