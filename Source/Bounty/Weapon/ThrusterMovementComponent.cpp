// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrusterMovementComponent.h"


UThrusterMovementComponent::EHandleBlockingHitResult UThrusterMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
    Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
    return EHandleBlockingHitResult::AdvanceNextSubstep;
}

void UThrusterMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
    // Rockets should not stop; only explode when their collisionBox detects a hit
}
