// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Animation/AnimInstance.h"

#include "TornadoCPP.generated.h"

#define D(x, time) GEngine->AddOnScreenDebugMessage(-1, time, FColor::Yellow, x);

#define DEBUG false

/*
USTRUCT(BlueprintType) 
struct FInputs1{
	GENERATED_BODY()
	public:
		FInputs1(){
			cameraX = 0.f;	
			cameraY = 0.f;	
			cameraX_GP = 0.f;	
			cameraY_GP = 0.f;	

			moveX = 0;	
			moveY = 0;	
			moveX_GP = 0;	
			moveY_GP = 0;

			speedUp = 0;
		};
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static float cameraX;		//[0, 1]
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static float cameraY;		//[-1, 0]
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static float cameraX_GP;	//[0, 1]
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static float cameraY_GP;	//[0, 1]

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static int	moveX;		//[-1, 1]   forward/backward
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static int	moveY;		//[-1, 1]	left/right
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static int	moveX_GP;	//[-1, 1]
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static int	moveY_GP;	//[-1, 1]
				
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			static int speedUp;		//[0, 1]
} inputs;
*/
UCLASS()
class MUGENENGINE_API ATornadoCPP : public APawn
{
	GENERATED_BODY()

	public:
		// Sets default values for this pawn's properties
		ATornadoCPP();
		/*
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
			class UStaticMeshComponent* MeshCollision;
		*/
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Modle")
			class USkeletalMeshComponent* Rooter;

		//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ModelRoot")
			//class USceneComponent* ModelRoot;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
			class USpringArmComponent* SpringArm;

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
			class UCameraComponent* Camera;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
			class UTornado_MovementComponent* CollidingMovementComponent;


		virtual UPawnMovementComponent* GetMovementComponent() const override;

		//There was a structure, it's gone cuz UE is a dumb shit
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			float input_dodge;		//[0, 1]
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			float input_cameraX;		//[0, 1]
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			float input_cameraY;		//[-1, 0]
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			float input_cameraX_GP;	//[0, 1]
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			float input_cameraY_GP;	//[0, 1]

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			int	input_moveX;		//[-1, 1]   forward/backward
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			int	input_moveY;		//[-1, 1]	left/right
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			int	input_moveX_GP;	//[-1, 1]
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			int	input_moveY_GP;	//[-1, 1]
				
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			int input_speedUp;		//[0, 1]


		/*
		 * DEBUG VARS
		*/

		struct SDEBUG{
			int hitCounts = 0;

			void hitDebug(SDEBUG& debug){
				debug.hitCounts++;
				//D(FString::Printf(TEXT("HitCount: %d"), hitCounts), 5.f);
			}
		}Debug;

		/*
		 * MAIN VARS
		*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int maxHP{5};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int HP{maxHP};
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			float HP_HitDelay{0.f};
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			float HP_SurfaceHitDelay{ 0.f };
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float HP_HitDelayMax{1.f};

		UPROPERTY(BlueprintReadWrite)
			float currentForwardSpeed{0.0f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float forwardSpeedAcc{25000.0f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float forwardSpeedDec{18000.0f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float maxForwardSpeed{25000.0f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float minForwardSpeed{10000.0f};
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float slowDownMultiplyer{1.0f};

		float currentMaxSpeed{0.0f};

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float currentUpDownSpeed{0.0f};

		
		UPROPERTY(BlueprintReadWrite)
			float currentSidewaysSpeed{0.0f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float sidewaysSpeedAcc{23000.0f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float sidewaysSpeedDec{16000.0f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float maxSidewaysSpeed{9000.0f};
		
		UPROPERTY(BlueprintReadOnly)
			float dodgeCurSpeed{0.f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float dodgePushSpeed{50000.f};

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			bool isDodged{false};

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			bool bInterpCamera{false};
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			bool bDead{ false };
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			bool bInputDisable{ false };

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicsAsset)
			UPhysicsAsset* PlayPhysicsAsset;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicsAsset)
			UPhysicsAsset* DestroyedPhysicsAsset;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float InterpCameraSpeedInit{0.2f};
		float InterpCameraSpeed{InterpCameraSpeedInit};

		FRotator CurrentTrunSpeed{0.0f};
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
			FRotator OffsetGroupAdjustment{0.0f};
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			FRotator DirectionMovement{0.0f};
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			FRotator AdjustingDirection{0.0f};

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float	turnSpeed{200.0f*2.f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float	turnSpeedClamp{400.0f};
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float	turnDecSpeed{5.f};	
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float	turnAdjSpeed{5.f};	

		
		FVector OutsideForceTarget{0.f};
		FVector GravityForce{0.f};
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
			FVector OutsideForce{0.f};

		FVector PushDir{0.f};
		float	pushSpeed{0.f};
		float	pushDecMult{1.0f};


		//ANIMATIONS

		UAnimInstance* AnimInstanceRef;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			UAnimMontage* DodgeRMontage;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			UAnimMontage* DodgeLMontage;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			UAnimMontage* DamageMontage;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			UAnimMontage* BoostDamageMontage;


		struct SWiggle {
			float amplitude{140.f*2.f};
			float speed{4000.f};
			float smoothing{0.7f};
			FVector pos{0.f};
		} wiggle;

		struct SCamera {
			float	targetArmLength{630.f*2.7f};
			float	speedFactor{0.f};
			float	distanceFactor{1.f};

			FVector lagSpeed{0.f, 40.f, 50.f};
			FVector offset{0.f};
			FVector targetOffset{0.f, 0.f, 230.f*2.f};

			float	initFOV{80.f};
			float	FOVAdd{100.f-initFOV};
		} camera;

		//General
		float timer{0.0f};

		//Camera params
		UPROPERTY(BlueprintReadOnly, Category = Camera)
			float cameraFOV{0.f};

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		// Called to bind function
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		UFUNCTION(BlueprintCallable)
			FVector OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
		UFUNCTION(BlueprintCallable)
			void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION(BlueprintCallable)
			void Damage(int minushp, bool surface_hit);
		UFUNCTION(BlueprintCallable)
			void PlayMontage(UAnimMontage* montage);
		UFUNCTION(BlueprintCallable)
			void ResetsOnRespawn();

		// UTILS
		UFUNCTION(BlueprintCallable) void ResetAllVelocity(){
			currentForwardSpeed		= {0.0f};
			currentMaxSpeed			= {0.0f};
			currentUpDownSpeed		= {0.0f};
			currentSidewaysSpeed	= {0.0f};

			OffsetGroupAdjustment	= FRotator{0.0f};
			DirectionMovement		= FRotator{0.0f};
			CurrentTrunSpeed		= FRotator{ 0.0f };
			AdjustingDirection		= FRotator{0.0f};

			dodgeCurSpeed	= {0.f};
			pushSpeed		= {0.f};
		};

		UFUNCTION(BlueprintCallable)
			void BlowUpImmediately();



		bool bCtrlsOff = true;
		UFUNCTION(BlueprintCallable) void SetMovement(FRotator Dirrection, float ForwadSpeed){
			//CtrlsOff = false;
			
			ResetAllVelocity();

			DirectionMovement = Dirrection;

			currentForwardSpeed = ForwadSpeed;
			dodgeCurSpeed = { 0.f };
			pushSpeed = { 0.f };

		}

		UFUNCTION(BlueprintCallable) void ToggleCtrls(bool a){
			bCtrlsOff = a;
		};



		// EVENTS
		UFUNCTION(BlueprintImplementableEvent)
			void OnBlownUp();
		UFUNCTION(BlueprintImplementableEvent)
			void OnStartFall();



	private:
		bool bFallDoOnce{true};
		bool bBlownDoOnce{true};

		float tempValCheck{-666.0f};
		float LinearDumping{ 10.f };

		void UpdateComponents(float DT);
		void UpdateMovement(float DT);
		void UpdateCamera(float DT);
		void DeathFall(float DT);
		void BlownOnHit(bool SimPhysics);
		
		void RoughDirectionMovement(float DT);
		void Input_DodgeLogic(float input);
		void DodgeCheck(float DT);

		void Input_DodgeL();
		void Input_DodgeR();
		void Input_ForwardMovement(float input);
		void Input_SidewaysMovement(float input);
		void Input_UpDownMovement(float input);
		void Input_CameraXMovement(float input);
		void Input_CameraYMovement(float input);
		void Input_SpeedUp(float input);

		float AngleDiff(float A, float B){
			return FMath::RadiansToDegrees(FMath::Atan2(
				FMath::Sin(FMath::DegreesToRadians(A-B)),
				FMath::Cos(FMath::DegreesToRadians(A-B))
			));
		}
};