#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "Common.h"

#include "interpolation.h"

#include "LumpFile.h"


// global time for global sequences
extern int globalTime;
extern int globalFrame;

enum Interpolations {
	INTERPOLATION_NONE,
	INTERPOLATION_LINEAR,
	INTERPOLATION_HERMITE
};

struct FrameRange
{
	uint32	begin;
	uint32	end;
};

/*
	Generic animated value class:
	T is the data type to animate
*/
template <class T >
class Animated
{
public:
	Animated():
		type(INTERPOLATION_LINEAR),
		seq(-1),
		globals(NULL)
	{
	}
	int type, seq;
	int *globals;

	//std::vector<FrameRange> m_FrameRanges;
	// keyframes
	std::vector<uint32> m_KeyTimes;
	std::vector<T> m_KeyData;

	bool isUsed()const
	{
		return m_KeyData.size()!=0;
	}

	void addValue(unsigned int time,T data)
	{
		m_KeyTimes.push_back(time);
		m_KeyData.push_back(data);
	}

	T getValue(unsigned int time)const
	{
		if (/*type != INTERPOLATION_NONE || */m_KeyData.size()>1)
		{
			FrameRange range;

			// obtain a time value and a data range
			if (seq>-1) {
				if (globals[seq]==0) 
					time = 0;
				else 
					time = globalTime % globals[seq];
				range.begin = 0;
				range.end = uint32(m_KeyData.size()-1);
			} else {
				//range = m_FrameRanges[anim];
				range.begin = 0;
				range.end = uint32(m_KeyData.size()-1);
				time %= m_KeyTimes[m_KeyTimes.size()-1]; // I think this might not be necessary?
			}

 			if (range.begin != range.end)
			{
				size_t t1, t2;
				size_t pos=0;
				for (size_t i=range.begin; i<range.end; i++) //i<=end??
				{
					if (time >= m_KeyTimes[i] && time < m_KeyTimes[i+1])
					{
						pos = i;
						break;
					}
				}
				t1 = m_KeyTimes[pos];
				t2 = m_KeyTimes[pos+1];
				float r = (time-t1)/(float)(t2-t1);

				if (type == INTERPOLATION_LINEAR) 
					return interpolate<T>(r,m_KeyData[pos],m_KeyData[pos+1]);
				else if (type == INTERPOLATION_NONE) 
					return m_KeyData[pos];
				else {
					// INTERPOLATION_HERMITE is only used in cameras afaik?
					// for nonlinear interpolations:
					return interpolateHermite<T>(r,m_KeyData[pos*3],m_KeyData[pos*3+3],
						m_KeyData[pos*3+1],m_KeyData[pos*3+2]);
				}
			}
			else
			{
				return m_KeyData[range.begin];
			}
		} else {
			// default value
			if (m_KeyData.size() == 0)
				return T();
			else
				return m_KeyData[0];
		}
	}

	void fix(T fixfunc(const T))
	{
		for (size_t i=0; i<m_KeyData.size(); i++)
		{
			m_KeyData[i] = fixfunc(m_KeyData[i]);
		}
	}

	void Save(CLumpNode& lump)
	{
		lump.SetInt("type", type);
		lump.SetInt("seq", seq);
		//lump.SetVector("FrameRanges", m_FrameRanges);
		lump.SetVector("Times", m_KeyTimes);
		lump.SetVector("Data", m_KeyData);
	}

	void Load(CLumpNode& lump)
	{
		lump.GetInt("type", type);
		lump.GetInt("seq", seq);
		//lump.getVector("FrameRanges", m_FrameRanges);
		lump.getVector("Times", m_KeyTimes);
		lump.getVector("Data", m_KeyData);
	}

	CLumpNode* Save(CLumpNode& lump, const char* name)
	{
		CLumpNode* pNode = lump.AddNode(name);
		if (pNode)
		{
			Save(*pNode);
		}
		return pNode;
	}

	CLumpNode* Load(CLumpNode& lump, const char* name)
	{
		CLumpNode* pNode = lump.firstChild(name);
		if (pNode)
		{
			Load(*pNode);
		}
		return pNode;
	}
};
