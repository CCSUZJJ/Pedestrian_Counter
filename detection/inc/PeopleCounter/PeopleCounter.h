#include "DetAlgoI.h"

class PeopleCounter : public IDetAlgo {
	public:
        PeopleCounter(){}

		bool ReadSample(OpenCvFrame& InputFrame) override;

		bool Run() override;

		bool Configure(const SFrameWorkConfig& Config) override;

		void SetFieldCounter(unsigned int FieldCounter) override;

		bool HasWork() override;

		void ProcessPerformanceAnalyse() override;

		void FinishGroupPerformanceAnalyse() override;

		void FinishPerformanceAnalyse() override;

		void SetVideoName(const std::string& Path, const std::string& VideoName,
                          unsigned int FrameRate, CvSize& FrameSize, bool CreateVideo = false) override;
};
