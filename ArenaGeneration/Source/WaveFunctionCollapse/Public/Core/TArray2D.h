#pragma once

#include "CoreMinimal.h"

template <typename T>
class WAVEFUNCTIONCOLLAPSE_API TArray2D
{
public:
	void Init(const T& Element, int32 D1Size, int32 D2Size)
	{
		SizeD1 = D1Size;
		SizeD2 = D2Size;
		InnerArray.Init(Element, D1Size * D2Size);
	}

	FORCEINLINE T& Get(const int32 D1, const int32 D2) { return InnerArray[(D1 * SizeD2) + D2]; }

	FORCEINLINE void Set(const int32 D1, const int32 D2, const T& Item) { InnerArray[(D1 * SizeD2) + D2] = Item; };
	FORCEINLINE void Empty()
	{
		InnerArray.Empty();
		SizeD1 = 0;
		SizeD2 = 0;
	}

	FORCEINLINE int32 GetD1() const { return SizeD1; }
	FORCEINLINE int32 GetD2() const { return SizeD2; }
	// void ForEach(auto& Func(T& Item))
	// {
	// 	for (int32 i = 0; i < InnerArray.Num(); ++i)
	// 	{
	// 		int Y = i / SizeD1;
	// 		int X = i % SizeD2;
	// 		Func(InnerArray[i]);
	// 	}
	// }

	TArray<T> InnerArray;

private:
	int32 SizeD1 = 0;
	int32 SizeD2 = 0;
};
