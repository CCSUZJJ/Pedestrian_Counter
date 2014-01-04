#include "TestReporterStdout.h"
#include <cstdio>
#include <sstream>

#include "TestDetails.h"

namespace UnitTest {

void TestReporterStdout::ReportFailure(TestDetails const& details, char const* failure)
{
#if defined(__APPLE__) || defined(__GNUG__)
    char const* const errorFormat = "%s:%d: error: Failure in %s: %s\n";
#else
    char const* const errorFormat = "%s(%d): error: Failure in %s: %s\n";
#endif

	using namespace std;
    printf(errorFormat, details.filename, details.lineNumber, details.testName, failure);
}

void TestReporterStdout::ReportBenchmark(const TestDetails &details, const std::vector<float> &Durations)
{
#if defined(__APPLE__) || defined(__GNUG__)
    char const* const benchmarkFormat = "%s:%d: info: Benchmark in %s: %s\n";
#else
    char const* const benchmarkFormat = "%s(%d): info: Benchmark in %s: %s\n";
#endif

    float Avg = 0.0f;
    std::ostringstream BenchmarkText;
    BenchmarkText << "repeats=" << Durations.size() << " timings=";
    for(auto& Duration : Durations)
    {
        BenchmarkText << Duration << " ";
        Avg += Duration;
    }
    if(!Durations.empty())
    {
        Avg /= Durations.size();
    }
    BenchmarkText << "average=" << Avg;


    std::printf(benchmarkFormat, details.filename, details.lineNumber, details.testName, BenchmarkText.str().c_str());
}

void TestReporterStdout::ReportTestStart(TestDetails const& /*test*/)
{
	// uncomment to see which test is running
	// useful if the test executable hangs in a thread
	/*if (test.suiteName)
		printf("%s::", test.suiteName);
	printf("%s\n", test.testName);
	fflush(stdout);*/
}

void TestReporterStdout::ReportTestFinish(TestDetails const& /*test*/, float)
{
}

void TestReporterStdout::ReportSummary(int const totalTestCount, int const failedTestCount,
                                       int const failureCount, float secondsElapsed)
{
	using namespace std;

    if (failureCount > 0)
        printf("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
    else
        printf("Success: %d tests passed.\n", totalTestCount);

    printf("Test time: %.2f seconds.\n", secondsElapsed);
}

}
