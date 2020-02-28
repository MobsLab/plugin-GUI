#ifndef POSITIONMOBSNEURALNETS_H
#define POSITIONMOBSNEURALNETS_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "tensorflow/cc/client/client_session.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/tensor.h"

#include <tensorflow/core/public/session.h>
#include <tensorflow/core/protobuf/meta_graph.pb.h>
#pragma GCC diagnostic pop

#include <ProcessorHeaders.h>

namespace OnlineDecoding {
	struct DecodingResults
	{
		DecodingResults(double& inX, double& inY, double& inStdX, double& inStdY);
		DecodingResults(std::vector<tensorflow::Tensor>* netWorkOutput);
		const void* getValues() {return values;}
		const double X;
		const double Y;
		const double stdX;
		const double stdY;
		const std::vector<std::vector<double>> positionProba;
		double stdDev();
	private:
		double values[3];
		static std::vector<std::vector<double>> positionProbaAsArray(std::vector<tensorflow::Tensor>* networkOutput);
	};
}

class PositionDecoderNetwork
{
private:
	void loadDecoderGraph(std::string *pathToGraph, std::string *checkpointPath);
	void loadConcatenatorGraph(int nNodes);
	void addConcatenatorNode(int graph);

	std::vector<std::vector<uint>> channels;

	tensorflow::Scope                                             scope = tensorflow::Scope::NewRootScope();
	tensorflow::Status                                            status;
	ScopedPointer<tensorflow::Session>                            mainSession;
	ScopedPointer<tensorflow::ClientSession>                      concatSession;

	std::vector<int>                                              nSpikes;
	std::vector<tensorflow::ClientSession::FeedType>              concatFeedDict;
	std::vector<std::vector<tensorflow::Tensor>>                  spikeTensors;
	std::vector<std::vector<tensorflow::Output>>                  spikePlaceholders;
	std::vector<std::vector<tensorflow::Output>>                  concatenators;

	std::vector<std::pair<std::string, tensorflow::Tensor>>       sessionInput;
	std::vector<tensorflow::Tensor>                               sessionOutput;
	std::vector<std::string>                                      sessionOutputTensorName;

public:
	PositionDecoderNetwork(std::string path, std::vector<std::vector<uint>> inChannels);
	void addSpike (std::vector<std::vector<float>>& data, int group);
	void clearOutput();
	OnlineDecoding::DecodingResults* inferPosition(float windowLength);
};

#endif // POSITIONMOBSNEURALNETS_H