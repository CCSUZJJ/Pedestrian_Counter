#ifndef UNITTEST_TIMECONSTRAINT_H
#define UNITTEST_TIMECONSTRAINT_H

#include "TimeHelpers.h"

namespace UnitTest {

class TestResults;
class TestDetails;

class TimeConstraint
{
public:
    TimeConstraint(int ms, TestDetails const& details);
    ~TimeConstraint();

private:
    void operator=(TimeConstraint const&); 
	TimeConstraint(TimeConstraint const&);

	Timer m_timer;
    TestDetails const& m_details;
	int const m_maxMs;
};

#define UNITTEST_TIME_CONSTRAINT(ms) \
	UnitTest::TestDetails unitTest__timeConstraint__testdetails__(m_details, __LINE__); \
	UnitTest::TimeConstraint unitTest__timeConstraint__(ms, unitTest__timeConstraint__testdetails__)

#define UNITTEST_TIME_CONSTRAINT_EXEMPT() do { m_timeConstraintExempt = true; } while (0)

}

#endif
