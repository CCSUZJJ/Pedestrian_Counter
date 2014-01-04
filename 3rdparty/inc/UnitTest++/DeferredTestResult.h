#ifndef UNITTEST_DEFERREDTESTRESULT_H
#define UNITTEST_DEFERREDTESTRESULT_H

#include <string>
#include <vector>

namespace UnitTest
{

struct DeferredTestResult
{
	DeferredTestResult();
    DeferredTestResult(char const* suite, char const* test);
    ~DeferredTestResult();

    std::string suiteName;
    std::string testName;
    std::string failureFile;

    typedef std::pair< int, std::string > Failure;
    typedef std::vector< Failure > FailureVec;
    FailureVec failures;

    std::string benchmarkFile;
    typedef std::pair< int, std::vector<float> > Benchmark;
    Benchmark benchmark;

    float timeElapsed;
	bool failed;
    bool benchmarked;
};

}

#endif //UNITTEST_DEFERREDTESTRESULT_H
