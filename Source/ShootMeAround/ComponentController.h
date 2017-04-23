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
	UPROPERTY(Category = "Walls", EditAnywhere, BlueprintReadWrite)
	AActor* NegativeX;
	UPROPERTY(Category = "Walls", EditAnywhere, BlueprintReadWrite)
	AActor* PositiveY;
	UPROPERTY(Category = "Walls", EditAnywhere, BlueprintReadWrite)
	AActor* NegativeY;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent* Billboard;

	UFUNCTION(BlueprintCallable, Category = "Bullets")
	void AddBullet(AActor* NewBullet);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	void FillOriginalObjectsArray();

	void TeleportObjectsIfEscaping();
	void TeleportIfEscaping(AActor *Object);

	void UpdateTriggerLimit();

	float RightTranslation;
	float FrontTranslation;

	TArray<AActor*> OriginalObjectInstances;
	TArray<ACharacter*> OriginalCharacterInstances;
	TArray<AActor*> OriginalBulletInstances;
};