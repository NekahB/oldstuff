// Fill out your copyright notice in the Description page of Project Settings.


#include "WindArea.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "TornadoCPP.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AWindArea::AWindArea(){

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = RootPoint;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	CollisionBox->SetupAttachment(GetRootComponent());
	//BoxCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	//CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWindArea::OnOverlapBegin);
	 
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(GetRootComponent());

}

void AWindArea::OnConstruction(const FTransform& Transf) {
//	Arrow->SetWorldRotation(ForceDirection);
}


// Called when the game starts or when spawned
void AWindArea::BeginPlay(){

	Super::BeginPlay();


	PawnRef = Cast<ATornadoCPP>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	ForceDirection = Arrow->GetComponentRotation();
}

// Called every frame
void AWindArea::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (PawnRef != nullptr) {
		ATornadoCPP* TornadoRef = Cast<ATornadoCPP>(PawnRef);
		if (IsOverlappingActor(PawnRef)) {

			ForceDirectionFinal = UKismetMathLibrary::ComposeRotators(ForceDirection, ForceDirectionOffset * windTurbulence);
			TornadoRef->OutsideForceTarget = ForceDirectionFinal.Vector() * windForce * windForceMultiplier;

			bStartTicking = true;

			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::Printf(TEXT("wind on, force : %f"), windForce * windForceMultiplier));
		}
		else {
			if (bStartTicking) {
				TornadoRef->OutsideForceTarget = FVector{ 0.f };
				bStartTicking = false;
				//PrimaryActorTick.bCanEverTick = false;
			}
		}
	}
}
/*
void AWindArea::OnOverlapBegin(
	class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	//GEngine->UEngine::AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("overlap"));
	ATornadoCPP* TornadoRef = Cast<ATornadoCPP>(PawnRef);
	if (OtherActor == PawnRef) {
		PrimaryActorTick.bCanEverTick = true;
	}
}
*/