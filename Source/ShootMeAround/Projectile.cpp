#include "ShootMeAround.h"
#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
	// Use a sphere as a simple collision representation.
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereCollision->InitSphereRadius(5.0f);
	SphereCollision->BodyInstance.SetCollisionProfileName("Projectile");
	SphereCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit); // Set up a notification for when this component hits something blocking

	SphereCollision->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	SphereCollision->CanCharacterStepUpOn = ECB_No;

	// Set as root component.
	RootComponent = SphereCollision;

	// Set visual mesh
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set particle system
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement.
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

// Called when the game starts or when spawned.

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
		RootComponent = ParticleSystem;
		SphereCollision->DestroyComponent();
		VisualMesh->DestroyComponent();
		ProjectileMovement->DestroyComponent();
		ParticleSystem->Deactivate();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AProjectile::DeleteObject, 5.0f, false);
	}
	else if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics()) // Only add impulse and destroy projectile if we hit a physics
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
}

void AProjectile::DeleteObject()
{
	Destroy();
}
