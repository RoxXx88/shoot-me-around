

#include "ShootMeAround.h"
#include "ShootMeAroundGameViewportClient.h"


bool UShootMeAroundGameViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	if (IgnoreInput() || bGamepad || Key.IsMouseButton())
	{
		return Super::InputKey(Viewport, ControllerId, Key, EventType, AmountDepressed, bGamepad);
	}
	else
	{
		// Propagate keyboard events to all players
		bool bRetVal = false;
		if (Key.ToString().Equals("LeftShift") || Key.ToString().Equals("LeftControl") || Key.ToString().Equals("W") || Key.ToString().Equals("A") || Key.ToString().Equals("S") || Key.ToString().Equals("D"))
		{
			bRetVal = Super::InputKey(Viewport, 0, Key, EventType, AmountDepressed, bGamepad) || bRetVal;
		}
		else
			if (Key.ToString().Equals("K") || Key.ToString().Equals("M") || Key.ToString().Equals("Up") || Key.ToString().Equals("Left") || Key.ToString().Equals("Down") || Key.ToString().Equals("Right"))
			{
				bRetVal = Super::InputKey(Viewport, 1, Key, EventType, AmountDepressed, bGamepad) || bRetVal;
			}

		return bRetVal;
	}
}

