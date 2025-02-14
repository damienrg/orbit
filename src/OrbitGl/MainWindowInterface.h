// Copyright (c) 2020 The Orbit Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ORBIT_GL_MAIN_WINDOW_INTERFACE_H_
#define ORBIT_GL_MAIN_WINDOW_INTERFACE_H_

#include <stdint.h>

#include <filesystem>
#include <optional>
#include <string_view>

#include "ClientData/FunctionInfo.h"
#include "ClientData/ModuleData.h"
#include "ClientProtos/capture_data.pb.h"
#include "CodeReport/CodeReport.h"
#include "CodeReport/DisassemblyReport.h"
#include "MetricsUploader/ScopedMetric.h"
#include "OrbitBase/Future.h"
#include "OrbitBase/StopToken.h"
#include "Statistics/Histogram.h"

namespace orbit_gl {

// This abstract base class is an attempt to simplify callbacks
// in OrbitApp and make it easier to refactor things in the future.
//
// OrbitMainWindow and Mocks can derive from this and offer a fixed interface
// to OrbitApp.
class MainWindowInterface {
 public:
  virtual void ShowTooltip(std::string_view message) = 0;
  virtual void ShowWarningWithDontShowAgainCheckboxIfNeeded(
      std::string_view title, std::string_view text,
      std::string_view dont_show_again_setting_key) = 0;

  virtual void ShowSourceCode(
      const std::filesystem::path& file_path, size_t line_number,
      std::optional<std::unique_ptr<orbit_code_report::CodeReport>> code_report,
      orbit_metrics_uploader::ScopedMetric* metric) = 0;
  virtual void ShowDisassembly(const orbit_client_data::FunctionInfo& function_info,
                               const std::string& assembly,
                               orbit_code_report::DisassemblyReport report) = 0;

  enum class CaptureLogSeverity { kInfo, kWarning, kSevereWarning, kError };
  virtual void AppendToCaptureLog(CaptureLogSeverity severity, absl::Duration capture_time,
                                  std::string_view message) = 0;

  virtual void ShowHistogram(const std::vector<uint64_t>* data, const std::string& scope_name,
                             uint64_t scope_id) = 0;

  enum class SymbolErrorHandlingResult { kReloadRequired, kSymbolLoadingCancelled };
  virtual SymbolErrorHandlingResult HandleSymbolError(
      const ErrorMessage& error, const orbit_client_data::ModuleData* module) = 0;

  virtual orbit_base::Future<ErrorMessageOr<void>> DownloadFileFromInstance(
      std::filesystem::path path_on_instance, std::filesystem::path local_path,
      orbit_base::StopToken stop_token) = 0;

  virtual ~MainWindowInterface() = default;
};

}  // namespace orbit_gl

#endif  // ORBIT_GL_MAIN_WINDOW_INTERFACE_H_