#include "Tornado_MovementComponent.h"

void UTornado_MovementComponent::TickComponent(float DT, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction){
    Super::TickComponent(DT, TickType, ThisTickFunction);

    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DT)){return;}
    //if (!UpdatedComponent || ShouldSkipUpdate(DT)){return;}

    FVector NextMovement = ConsumeInputVector()*DT;
	FRotator NextRotation = UpdatedComponent->GetComponentRotation();
    //if (!NextMovement.IsNearlyZero()){
    FHitResult Hit;
    SafeMoveUpdatedComponent(NextMovement, NextRotation, true, Hit);
    ResolvePenetration(GetPenetrationAdjustment(Hit), Hit, NextRotation);
    if (Hit.IsValidBlockingHit()){
        SlideAlongSurface(NextMovement, 1.f - Hit.Time, Hit.Normal, Hit);
    }
    //}
};