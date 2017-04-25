#pragma once

#include "GameFramework/Actor.h"
#include "ComponentController.generated.h"

class PlayerController;

class AStaticMeshActor;
class UBillboardComponent;

UCLASS(Blueprintable, BlueprintType)
class SHOOTMEAROUND_API AComponentController : public AActor
{
	GENERATED_BODY()

public:

	AComponentController();

	UPROPERTY(Category = "World", EditAnywhere, BlueprintReadWrite)
	AStaticMeshActor* World;

	UPROPERTY(Category = "Players", EditAnywhere, BlueprintReadWrite)
	int NumPlayers = 2;

	UPROPERTY(Category = "Objects", EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Objects;

	UPROPERTY(Category = "Walls", EditAnywhere, BlueprintReadWrite)
	AActor* PositiveX;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent* Billboard;

	UPROPERTY(Category = "Camera", EditAnywhere, BlueprintReadWrite)
	int DistanzaCamera;

	UFUNCTION(BlueprintCallable, Category = "Bullets")
	void AddBullet(AActor* NewBullet);

	UFUNCTION(BlueprintCallable, Category = "Fill")
	void FillOriginalObjectsArray();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	void TeleportObjectsIfEscaping();
	void TeleportIfEscaping(AActor *Object);

	void UpdateTriggerLimit();

	float RightTranslation;
	float FrontTranslation;

	TArray<AActor*> OriginalObjectInstances;
	TArray<ACharacter*> OriginalCharacterInstances;
	TArray<AActor*> OriginalBulletInstances;
};