#include "ShootMeAround.h"
#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	// Use a sphere as a simple collision representation
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereCollision->InitSphereRadius(5.0f);
	SphereCollision->BodyInstance.SetCollisionProfileName("Projectile");
	SphereCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit); // set up a notification for when this component hits something blocking

																		   // Players can't walk on it
	SphereCollision->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	SphereCollision->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = SphereCollision;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = SphereCollision;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->Bounciness = 1.f;
	ProjectileMovement->Friction = 0.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	++BounchesCounter;
	if (BounchesCounter > NumberOfBounches)
	{
		Destroy();
	}
	else if (dynamic_cast<APawn*>(OtherActor))
	{
		FDamageEvent DamageEvent;
		OtherActor->TakeDamage(PlayerDamage, DamageEvent, NULL, this);
		Destroy();
	}
	// Only add impulse and destroy projectile if we hit a physics
	else if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
}