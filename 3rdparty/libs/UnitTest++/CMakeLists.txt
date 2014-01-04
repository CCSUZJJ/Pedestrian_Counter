cmake_minimum_required(VERSION 2.8)

include("../../../cmake/OptionsCommon.cmake")
include("../../../cmake/Options${PORT}.cmake")

traf_set_public_headers("UnitTest++")

set(SRC_COMMON 
            "${TRAF_INCLUDE_PUBLIC}/AssertException.h"
            "${TRAF_INCLUDE_PUBLIC}/CheckMacros.h"
            "${TRAF_INCLUDE_PUBLIC}/Checks.h"
            "${TRAF_INCLUDE_PUBLIC}/Config.h"
            "${TRAF_INCLUDE_PUBLIC}/CurrentTest.h"
            "${TRAF_INCLUDE_PUBLIC}/DeferredTestReporter.h"
            "${TRAF_INCLUDE_PUBLIC}/DeferredTestResult.h"
            "${TRAF_INCLUDE_PUBLIC}/ExecuteTest.h"
            "${TRAF_INCLUDE_PUBLIC}/MemoryOutStream.h"
            "${TRAF_INCLUDE_PUBLIC}/ReportAssert.h"
            "${TRAF_INCLUDE_PUBLIC}/Test.h"
            "${TRAF_INCLUDE_PUBLIC}/TestDetails.h"
            "${TRAF_INCLUDE_PUBLIC}/TestList.h"
            "${TRAF_INCLUDE_PUBLIC}/TestMacros.h"
            "${TRAF_INCLUDE_PUBLIC}/TestReporter.h"
            "${TRAF_INCLUDE_PUBLIC}/TestReporterStdout.h"
            "${TRAF_INCLUDE_PUBLIC}/TestResults.h"
            "${TRAF_INCLUDE_PUBLIC}/TestRunner.h"
            "${TRAF_INCLUDE_PUBLIC}/TestSuite.h"
            "${TRAF_INCLUDE_PUBLIC}/TimeConstraint.h"
            "${TRAF_INCLUDE_PUBLIC}/TimeHelpers.h"
            "${TRAF_INCLUDE_PUBLIC}/UnitTest++.h"
            "${TRAF_INCLUDE_PUBLIC}/XmlTestReporter.h"
            "src/AssertException.cpp"
            "src/Checks.cpp"
            "src/CurrentTest.cpp"
            "src/ReportAssert.cpp"
            "src/Test.cpp"
            "src/TestDetails.cpp"
            "src/TestList.cpp"
            "src/TestReporter.cpp"
            "src/TestResults.cpp"
            "src/TestRunner.cpp"
            "src/DeferredTestReporter.cpp"
            "src/DeferredTestResult.cpp"
            "src/MemoryOutStream.cpp"
            "src/TestReporterStdout.cpp"
            "src/TimeConstraint.cpp"
            "src/XmlTestReporter.cpp"
		   )

set(SRC_WIN32_COMMON
           "${TRAF_INCLUDE_PUBLIC}/Win32/TimeHelpers.h"
            "src/Win32/TimeHelpers.cpp"
            )

set(SRC_LINUX_COMMON
            "${TRAF_INCLUDE_PUBLIC}/Posix/SignalTranslator.h"
            "${TRAF_INCLUDE_PUBLIC}/Posix/TimeHelpers.h"
            "src/Posix/TimeHelpers.cpp"
            "src/Posix/SignalTranslator.cpp"
            )

set(SRC_TARGET 3rdpartyUnitTest++)

traf_start_library()
traf_end_library()

traf_include_public_headers("UnitTest++")
