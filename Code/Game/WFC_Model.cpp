#include "Game/WFC_Model.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Model::Model(int width, int height)
{
	m_modelDimensions = IntVec2(width, height);
	m_modelTotalBlocks = width * height;
}

//------------------------------------------------------------------------------------------------------------------------------
void Model::Startup()
{
	//Initialize wave and compatibility data
	m_waveArray = Array2D<bool>(IntVec2(m_modelTotalBlocks, m_numWeights), false);
	m_compatibleBlocks = Array2D<std::vector<int>>(IntVec2(m_modelTotalBlocks, m_numWeights), std::vector<int>(0, 0));
	for (int i = 0; i < m_waveArray.GetWidth(); i++)
	{
		//Set 4 ints in the m_compatibleBlocks array for values
		for (int j = 0; j < m_numWeights; j++)
		{
			m_compatibleBlocks.Set(IntVec2(i, j), {0,0,0,0});
		}
	}

	//Initializing all weight log weights
	for (int weightIndex = 0; weightIndex < m_numWeights; weightIndex++)
	{
		m_weightLogWeights[weightIndex] = m_weights[weightIndex] * log(m_weights[weightIndex]);
		m_sumOfWeights += m_weights[weightIndex];
		m_sumOfWeightLogWeights += m_weightLogWeights[weightIndex];
	}

	m_startingEntropy = log(m_sumOfWeights) - (m_sumOfWeightLogWeights / m_sumOfWeights);

	m_sumOfOnes = std::vector<int>(m_modelTotalBlocks, 0);
	m_sumsOfWeights = std::vector<double>(m_modelTotalBlocks, 0.0);
	m_sumsOfWeightLogWeights = std::vector<double>(m_modelTotalBlocks, 0.0);
	m_entropies = std::vector<double>(m_modelTotalBlocks, 0.0);
}

//------------------------------------------------------------------------------------------------------------------------------
bool Model::Observe()
{
	double min = 1E+3;
	int argmin = -1;

	for (int i = 0; i < m_waveArray.GetWidth(); i++)
	{
		TODO("Make OnBoundary");
		/*
		if (OnBoundary(i % m_modelDimensions.x, i / m_modelDimensions.x))
		{
			continue;
		}
		*/

		int amount = m_sumOfOnes[i];
		if (amount == 0)
		{
			return false;
		}


		double entropy = m_entropies[i];
		if (amount > 1 && entropy <= min)
		{
			double noise = 1E-6 * g_RNG->GetRandomFloatInRange(0.f, FLT_MAX);
			if (entropy + noise < min)
			{
				min = entropy + noise;
				argmin = i;
			}
		}
	}

	if (argmin == -1)
	{
		for (int i = 0; i < m_waveArray.GetWidth(); i++)
		{
			for (int t = 0; t < m_numWeights; t++)
			{
				if (m_waveArray.ContainsCell(IntVec2(i, t)))
				{
					m_observedBlocks[i] = t;
					break;
				}
			}
		}
		
		return true;
	}


	std::vector<double> distribution;
	distribution.resize(sizeof(double) * m_numWeights);
	for (int t = 0; t < m_numWeights; t++)
	{
		distribution[t] = m_waveArray.ContainsCell(IntVec2(argmin, t)) ? m_weights[t] : 0;
	}

	int r = (int)distribution[g_RNG->GetRandomIntInRange(0, (int)distribution.size() - 1)];

	TODO("Fix this part of the WFC code to account for a 2D array of bools");
	//Need to fix this!
	/*
	bool w = m_waveArray.Get(IntVec2(argmin, 0));
	for (int t = 0; t < m_numWeights; t++)
	{
		if (w[t] != (t == r)) 
			Ban(argmin, t);
	}
	*/

	return false;
}
