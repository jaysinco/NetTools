#pragma once
#define UNICODE
#define _UNICODE
#include <winsock2.h>
#include <json.hpp>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <gflags/gflags.h>
#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#undef max
#undef min
