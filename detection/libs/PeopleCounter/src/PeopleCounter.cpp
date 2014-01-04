#include "DetAlgoI.h"

class PeopleCounter : public IDetAlgo {
	public:
        PeopleCounter(){}

		bool ReadSample(OpenCvFrame& InputFrame) override{
			std::cout << "Algorithm.ReadSample()" << std::endl;
			return true;
		}

		bool Run() override{
			std::cout << "Algorithm.Run()" << std::endl;
			return true;
		}

		bool Configure(const SFrameWorkConfig& Config) override{
			std::cout << "Algorithm.Configure()" << std::endl;
			return true;
		}

		void SetFieldCounter(unsigned int FieldCounter) override{
			std::cout << "Algorithm.SetFieldCounter()" << std::endl;
		}

		bool HasWork() override{
			std::cout << "Algorithm.HasWork()" << std::endl;
            return true;
		}

		void ProcessPerformanceAnalyse() override{
			std::cout << "Algorithm.ProcessPerformanceAnalyse()" << std::endl;
		}

		void FinishGroupPerformanceAnalyse() override{
			std::cout << "Algorithm.FinishGroupPerformanceAnalyse()" << std::endl;
		}

		void FinishPerformanceAnalyse() override{
			std::cout << "Algorithm.FinishPerformanceAnalyse()" << std::endl;
		}

		void SetVideoName(const std::string& Path, const std::string& VideoName,
						  unsigned int FrameRate, CvSize& FrameSize, bool CreatVideo = false) override{
            std::cout << "Algorithm.SetVideoName()" << std::endl;
		}
};
