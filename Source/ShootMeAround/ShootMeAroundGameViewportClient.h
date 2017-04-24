

#pragma once

#include "Engine/GameViewportClient.h"
#include "ShootMeAroundGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTMEAROUND_API UShootMeAroundGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
	
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed = 1.f, bool bGamepad = false) override;
	virtual bool InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	
};
