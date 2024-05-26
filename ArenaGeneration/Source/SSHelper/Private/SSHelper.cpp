#include "SSHelper.h"

DEFINE_LOG_CATEGORY(SSHelper);

#define LOCTEXT_NAMESPACE "FSSHelper"

void FSSHelper::StartupModule()
{
	UE_LOG(SSHelper, Warning, TEXT("SSHelper module has been loaded"));
}

void FSSHelper::ShutdownModule()
{
	UE_LOG(SSHelper, Warning, TEXT("SSHelper module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSSHelper, SSHelper)