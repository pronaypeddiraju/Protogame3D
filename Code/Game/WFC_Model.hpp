#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Array2D.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <stack>

//------------------------------------------------------------------------------------------------------------------------------
class Model
{
public:
	Model(int width, int height);

	void	Startup();
	bool	Observe();

private:
	//wave
	Array2D<bool>	m_waveArray;		
	uint			m_waveRowSize = 0;
	uint			m_waveColSize = 0;
	
	//propagation
	Array2D<std::vector<int>>	m_propagatorBlocks;
	Array2D<std::vector<int>>	m_compatibleBlocks;
	std::vector<int>			m_observedBlocks;
	int							m_numPropagationBlocks = 0;

	//Stack
	std::stack<int>	m_stack;

	//Generator Variables
	int					m_randomSeed = 0;
	IntVec2				m_modelDimensions = IntVec2::ZERO;
	int					m_modelTotalBlocks = 0;
	int					m_numWeights = 0;
	bool				m_isPeriodic = false;
	std::vector<double>	m_weights;
	std::vector<double>	m_weightLogWeights;

	std::vector<int>	m_sumOfOnes;
	double				m_sumOfWeights;
	double				m_sumOfWeightLogWeights;
	double				m_startingEntropy;

	std::vector<double>	m_sumsOfWeights;
	std::vector<double>	m_sumsOfWeightLogWeights;
	std::vector<double>	m_entropies;
};