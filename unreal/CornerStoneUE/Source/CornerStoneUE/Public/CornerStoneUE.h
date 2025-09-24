// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include <atomic>
#include <cstddef>
#include "src/cornerstonesdkcore.h"

class CORNERSTONEUE_API FCornerStoneUEModule : public IModuleInterface
{
public:
  static FCornerStoneUEModule& Get();
  static bool IsAvailable();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	bool Init(std::size_t threads);
	void Shutdown();

  // wrappers
  cornerstone::TaskId Request(TFunction<void()> fn);
  template <class F, class Cb>
  cornerstone::TaskId RequestWithResult(F f, Cb cb) {
    return sdk_.RequestWithResult(f, cb);
  }
  bool CancelById(cornerstone::TaskId id);
  void CancelAll();
  bool IsRunning() const noexcept;

private:
  std::atomic<bool> inited_{ false };
  cornerstone::CornerstoneSdkCore sdk_;
};
