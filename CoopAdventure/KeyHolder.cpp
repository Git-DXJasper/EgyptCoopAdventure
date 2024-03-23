
#include "KeyHolder.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AKeyHolder::AKeyHolder()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp")); 
	SetRootComponent(RootComp);

	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	KeyMesh->SetupAttachment(RootComp);
	KeyMesh->SetIsReplicated(true);
	KeyMesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	ColumnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ColumnMesh"));
	ColumnMesh->SetupAttachment(RootComp);
	ColumnMesh->SetIsReplicated(true);
	ColumnMesh->SetCollisionProfileName(FName("BlockAllDynamic"));

	KeyMeshRotationSpeed = 100.0f;

}

void AKeyHolder::BeginPlay()
{
	Super::BeginPlay();

	KeyMesh->SetVisibility(false);
	
}

void AKeyHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	KeyMesh->AddRelativeRotation(FRotator(0.0f, KeyMeshRotationSpeed * DeltaTime, 0.0f));

}

void AKeyHolder::ActivateKeyMesh()
{
	KeyMesh->SetVisibility(true);
}

