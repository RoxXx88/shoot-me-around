#pragma once

#include "GameFramework/Actor.h"
#include "ComponentController.generated.h"

struct FVector;
template<typename InElementType, typename InAllocator> class TArray;
template<typename InKeyType, typename InValueType>  class TPair;
class AActor;
class UBillboardComponent;
class PlayerController;
class AStaticMeshActor;

typedef FVector TranslateFromOrigin;
typedef TPair<TArray<AActor*>, TranslateFromOrigin> ArrayOfTranslatedObject;
typedef TPair<TArray<ACharacter*>, TranslateFromOrigin> ArrayOfTranslatedCharacter;

UCLASS(Blueprintable, BlueprintType)
class SHOOTMEAROUND_API AComponentController : public AActor
{
	GENERATED_BODY()

public:

	enum EventType
	{
		START_FIRE,
		STOP_FIRE,
		CROUCH,
		UNCROUCH
	};

	static const float MAX_TIMER_UPDATE_COPY;

	UPROPERTY(Category = "Components", VisibleAnywhere, BlueprintReadOnly)
	UBillboardComponent *Billboard;

	UPROPERTY(Category = "World", EditAnywhere, BlueprintReadWrite)
	AStaticMeshActor* World;

	UPROPERTY(Category = "Players", EditAnywhere, BlueprintReadWrite)
	int NumberOfPlayers;

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

	UFUNCTION(BlueprintCallable, Category = "Bullets")
	void AddBullet(AActor* NewBullet);


	UFUNCTION(BlueprintCallable, Category = "CharacterInput")
	void StartFire(ACharacter* Which);
	UFUNCTION(BlueprintCallable, Category = "CharacterInput")
	void StopFire(ACharacter* Which);
	UFUNCTION(BlueprintCallable, Category = "CharacterInput")
	void Crouch(ACharacter* Which);
	UFUNCTION(BlueprintCallable, Category = "CharacterInput")
	void UnCrouch(ACharacter* Which);

private:

	float RightTranslation;
	float FrontTranslation;

	TArray<AActor*> OriginalObjectInstances;
	TArray<ArrayOfTranslatedObject> CopiedObjectInstances;

	TArray<ACharacter*> OriginalCharacterInstances;
	TArray<ArrayOfTranslatedCharacter> CopiedCharacterInstances;

	TArray<AActor*> OriginalBulletInstances;
	TArray<ArrayOfTranslatedObject> CopiedBulletInstances;


public:

	AComponentController();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

private:

	void UpdateCopyTransform();

	void FillOriginalObjectsArray();

	void CreateCopiedInstances();
	void CreateCopiedInstancesWithTranslation(float XTranslation, float YTranslation);
	AActor* SpawnCopy(AActor* OriginalObject, FVector Translation, bool SetTemplate = false);

	void TeleportObjectsIfEscaping();
	void TeleportIfEscaping(AActor *Object);

	void TranslateObject(AActor* ToTranslate, AActor* OriginalObject, const FVector &TranslateValue);

	void SimulateEventOnCopy(ACharacter* Original, EventType Event);

};
