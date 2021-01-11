// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindArea.generated.h"

/**
 *
 */
UCLASS()
class MUGENENGINE_API AWindArea : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	AWindArea();
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		class USceneComponent* RootPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UArrowComponent* Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "params")
		float windForce{ 10000.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "params")
		float windTurbulence{ 1.f };


	UPROPERTY(BlueprintReadWrite)
		float windForceMultiplier{ 1.f };  //gradually changed in bp for turbulence effect

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform & Transf) override;

	UPROPERTY(BlueprintReadWrite)
		bool bStartTicking {false};
	
	APawn* PawnRef;

	FRotator ForceDirection{ 0.f };
	FRotator ForceDirectionFinal{ ForceDirection };
	UPROPERTY(BlueprintReadWrite)
		FRotator ForceDirectionOffset{ 0.f };

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//unused
	/*
	virtual void OnOverlapBegin(
		class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult
	);
	*/
};