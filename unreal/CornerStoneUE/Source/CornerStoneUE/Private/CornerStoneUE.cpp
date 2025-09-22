// Copyright Epic Games, Inc. All Rights Reserved.

#include "CornerStoneUE.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "FCornerStoneUEModule"

FCornerStoneUEModule& FCornerStoneUEModule::Get() {
	return FModuleManager::LoadModuleChecked<FCornerStoneUEModule>(
		"CornerStoneUE");
}

bool FCornerStoneUEModule::IsAvailable() {
	return FModuleManager::Get().IsModuleLoaded("CornerStoneUE");
}

void FCornerStoneUEModule::StartupModule()
{
	// changed this default value;
	Init(4);

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("CornerStoneUE")->GetBaseDir();

}

void FCornerStoneUEModule::ShutdownModule()
{
	Shutdown();
}

bool FCornerStoneUEModule::Init(std::size_t threads) {
  if (inited_.load()) return true;
  if (!sdk_.Init(threads)) return false;
  inited_.store(true);
  return true;
}

void FCornerStoneUEModule::Shutdown() {
  if (!inited_.load()) return;
  sdk_.Shutdown();
  inited_.store(false);
}

cornerstone::TaskId FCornerStoneUEModule::Request(TFunction<void()> fn) {
  if (!inited_.load()) return 0;
  return sdk_.Request([g = MoveTemp(fn)]() { g(); });
}

bool FCornerStoneUEModule::CancelById(cornerstone::TaskId id) {
  if (!inited_.load()) return false;
  return sdk_.CancelById(id);
}

void FCornerStoneUEModule::CancelAll() {
  if (!inited_.load()) return;
  sdk_.CancelAll();
}

bool FCornerStoneUEModule::IsRunning() const noexcept {
  return sdk_.IsRunning();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCornerStoneUEModule, CornerStoneUE)
