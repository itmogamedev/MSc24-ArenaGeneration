#pragma once

UENUM(BlueprintType)
enum class ELineOrientations : uint8
{
	ELO_Collinear UMETA(DisplayName = "Collinear"),
	ELO_Clockwise UMETA(DisplayName = "Clockwise"),
	ELO_CounterClockwise UMETA(DisplayName = "CounterClockwise")
};
