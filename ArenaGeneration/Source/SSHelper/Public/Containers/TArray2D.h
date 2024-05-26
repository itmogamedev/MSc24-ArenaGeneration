// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

template <typename T>
class SSHELPER_API TArray2D
{
public:
	void Init(const T& Element, int32 XSize, int32 YSize)
	{
		SizeX = XSize;
		SizeY = YSize;
		InnerArray.Init(Element, XSize * YSize);
	}

	FORCEINLINE T& Get(const int32 X, const int32 Y) { return InnerArray[(X * SizeX) + Y]; }

	void Set(const int32 X, const int32 Y, const T& Item) { InnerArray[(X * SizeX) + Y] = Item; };

	void ForEach(void (*Func)(const int32 D1, const int32 D2, const T& Item))
	{
		for (int32 i = 0; i < InnerArray.Num(); ++i)
		{
			int Y = i / SizeX;
			int X = i % SizeY;
			Func(Y, X, InnerArray[i]);
		}
	}

private:
	int32 SizeX = 0;
	int32 SizeY = 0;
	TArray<T> InnerArray;
};
