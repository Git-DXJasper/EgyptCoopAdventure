
#include "Transporter.h"

#include "PressurePlate.h"
#include "CollectableKey.h"

UTransporter::UTransporter()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	
	MoveTime = 3.0f;
	ActivatedTrigeerCount = 0;

	ArePointsSet = false;
	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();

}


void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	if(OwnerIsTriggerActor)
	{
		TriggerActors.Add(GetOwner());
	}

	for(AActor* TA : TriggerActors)
	{
		APressurePlate* PressurePlateActor = Cast<APressurePlate>(TA);
		if(PressurePlateActor)
		{
			PressurePlateActor->OnActivated.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
			PressurePlateActor->OnDeactivated.AddDynamic(this, &UTransporter::OnTriggerActorDeactivated);
			continue;
		}

		ACollectableKey* KeyActor = Cast<ACollectableKey>(TA);
		if(KeyActor)
		{
			KeyActor->OnCollected.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
			
		}
	}
	
}


// Called every frame
void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(TriggerActors.Num() > 0)
	{
		AllTriggerActorsTriggered = ActivatedTrigeerCount >= TriggerActors.Num();
		if(AllTriggerActorsTriggered)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString("All trigger actors triggered!"));
		}
	}
	
	AActor* MyOwner = GetOwner();
	if(MyOwner && MyOwner->HasAuthority() && ArePointsSet)
	{
		FVector CurrentLocation = MyOwner->GetActorLocation();
		float Speed = FVector::Distance(StartPoint, EndPoint)/ MoveTime;
		FVector TargetLocation = AllTriggerActorsTriggered ? EndPoint : StartPoint;
		if(!CurrentLocation.Equals(TargetLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
			MyOwner->SetActorLocation(NewLocation);
		}
	}
}

void UTransporter::SetPoints(FVector Point1, FVector Point2)
{
	if(Point1.Equals(Point2)) return;

	StartPoint = Point1;
	EndPoint = Point2;
	ArePointsSet = true;
}


void UTransporter::OnTriggerActorActivated()
{
	ActivatedTrigeerCount++;
	FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTrigeerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, Msg);
}

void UTransporter::OnTriggerActorDeactivated()
{
	ActivatedTrigeerCount--;
	FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTrigeerCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, Msg);
}
