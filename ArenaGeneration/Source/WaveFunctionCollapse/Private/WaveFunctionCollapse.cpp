#include "WaveFunctionCollapse.h"

DEFINE_LOG_CATEGORY(WaveFunctionCollapse);

#define LOCTEXT_NAMESPACE "FWaveFunctionCollapse"

void FWaveFunctionCollapse::StartupModule()
{
	UE_LOG(WaveFunctionCollapse, Warning, TEXT("WaveFunctionCollapse module has been loaded"));
}

void FWaveFunctionCollapse::ShutdownModule()
{
	UE_LOG(WaveFunctionCollapse, Warning, TEXT("WaveFunctionCollapse module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWaveFunctionCollapse, WaveFunctionCollapse)