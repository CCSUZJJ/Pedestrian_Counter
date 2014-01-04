#include "DeferredTestResult.h"
#include "Config.h"

namespace UnitTest
{

DeferredTestResult::DeferredTestResult()
	: suiteName("")
	, testName("")
	, failureFile("")
    , benchmarkFile("")
    , benchmark()
	, timeElapsed(0.0f)
	, failed(false)
    , benchmarked(false)
{
}

DeferredTestResult::DeferredTestResult(char const* suite, char const* test)
	: suiteName(suite)
	, testName(test)
	, failureFile("")
    , benchmarkFile("")
    , benchmark()
	, timeElapsed(0.0f)
	, failed(false)
    , benchmarked(false)
{
}

DeferredTestResult::~DeferredTestResult()
{
}

}
