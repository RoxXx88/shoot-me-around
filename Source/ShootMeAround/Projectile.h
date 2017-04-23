#pragma once

#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class SHOOTMEAROUND_API AProjectile : public AActor
{
	GENERATED_BODY()

public:

	AProjectile();

public:

	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;

public:
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void DeleteObject();

private:
	
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* SphereCollision;

	/** Projectile 3D Mesh */
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class UStaticMeshComponent* VisualMesh;

	/** Projectile particles */
	UPROPERTY(VisibleAnywhere, Category = Particle)
	class UParticleSystemComponent* ParticleSystem;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovement;

	/** Life of the projectile */
	UPROPERTY(EditAnywhere, Category = "Projectile Setup")
	float PlayerDamage = 20.f;

	/** Life of the projectile */
	UPROPERTY(EditAnywhere, Category = "Projectile Setup")
	int NumberOfBounches = 3;

	int BounchesCounter = 0;
};