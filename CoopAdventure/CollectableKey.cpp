
#include "CollectableKey.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"

#include "Net/UnrealNetwork.h"

#include "CoopAdventureCharacter.h"

#include "KeyHolder.h"

ACollectableKey::ACollectableKey()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp")); 
	SetRootComponent(RootComp);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComp);
	Capsule->SetIsReplicated(true);
	Capsule->SetCollisionProfileName(FName("OverlapAllDynamic"));
	Capsule->SetCapsuleHalfHeight(150.0f);
	Capsule->SetCapsuleRadius(100.f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	CollectAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CollectAudio"));
	CollectAudio->SetupAttachment(RootComp);
	CollectAudio->SetAutoActivate(false);

	RotationSpeed = 100.f;

}

void ACollectableKey::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACollectableKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		// Rotate the static mesh
		Mesh->AddRelativeRotation(FRotator(0.0f, RotationSpeed*DeltaTime, 0.0f));

		TArray<AActor*> OverlappingActors;
		//filter the class, only detect overplapping with character
		Capsule->GetOverlappingActors(OverlappingActors, ACoopAdventureCharacter::StaticClass());

		if(!OverlappingActors.IsEmpty())
		{
			if(!IsCollected)
			{
				IsCollected = true;
				//need to call manually on server
				OnRep_IsCollected();
			}
		}
	}

}

void ACollectableKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACollectableKey, IsCollected);

}

void ACollectableKey::OnRep_IsCollected()
{
	if(HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_IsCollected called from the Server!"));
		if(IsCollected)
		{
			OnCollected.Broadcast();
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_IsCollected called from the Client!"));
	}

	Mesh->SetVisibility(!IsCollected);

	CollectAudio->Play();

	if(IsCollected)
	{
		if(KeyHolderRef)
		{
			KeyHolderRef->ActivateKeyMesh();
		}
	}
}

