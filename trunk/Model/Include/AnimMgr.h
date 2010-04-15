#pragma once

#include "animated.h"
#include "modelheaders.h"


// This will be our animation manager
// instead of using a STL vector or list or table, etc.  
// Decided to just limit it upto 4 animations to loop through - for experimental testing.
// The second id and loop count will later be used for being able to have a primary and secondary animation.

// Currently, this is more of a "Wrapper" over the existing code
// but hopefully over time I can remove and re-write it so this is the core.
struct AnimInfo {
	short Loops;
	uint32 AnimID;
};

enum ANIMTYPE
{
	ANIM_TYPE_MOUTH,		// 计算口型动画
	ANIM_TYPE_SECONDARY,	// 上半身动画
	ANIM_TYPE_MAX,
};

struct AnimNode
{
	AnimNode():CurLoop(-1),uFrame(0),timeStart(0),timeEnd(0),uTotalFrames(0),fSpeed(0){}
	short	CurLoop;			// Current loop that we're upto.
	uint32	timeStart;
	uint32	timeEnd;
	uint32	uFrame;
	uint32	uTotalFrames;
	float	fSpeed;
	virtual int Tick(uint32 uElapsedTime)=0;
	virtual int next()
	{
		uTotalFrames = timeEnd - timeStart;
		uFrame -= uTotalFrames;
		if (CurLoop==1)
		{
			return 1;
		}
		else if(CurLoop>1)
		{
			CurLoop--;
			return 0;
		}
	}

	virtual int prev()
	{
		uTotalFrames = timeEnd - timeStart;
		uFrame += uTotalFrames;
		if (CurLoop==1)
		{
			return -1;
		}
		else if(CurLoop>1)
		{
			CurLoop++;
			return 0;
		}
	}
};

struct SingleAnimNode:public AnimNode
{
	
	//int		nAnimID;

	virtual int Tick(uint32 uElapsedTime)
	{
		uFrame += uint32(uElapsedTime*fSpeed);
		if (uFrame >= timeEnd)
		{
			return next();
		}
		else if (uFrame < timeStart)
		{
			return prev();
		}
		return 0;
	}
};

struct ListAnimNode:public AnimNode
{
	short nPlayIndex;		// Current animation index we're upto
	std::vector<AnimNode*>	setNodes;
	virtual int Tick(uint32 uElapsedTime)
	{
		timeStart = 0;
		timeEnd = setNodes.size();
		if (setNodes.size()>uFrame)
		{
			uFrame+=setNodes[uFrame]->Tick(int(uElapsedTime*fSpeed));
			if (uFrame >= timeEnd)
			{
				return next();
			}
			else if (uFrame < timeStart)
			{
				return prev();
			}
		}
	}
};

struct ParallelAnimNode:public AnimNode
{
	std::vector<AnimNode*>	setNodes;
	virtual int Tick(uint32 uElapsedTime)
	{
		for (size_t i=0;i<setNodes.size();++i)
		{
			setNodes[i]->Tick(int(uElapsedTime*fSpeed));
		}
	}
};

struct MyAnimInfo
{
	MyAnimInfo():nAnimID(-1),uFrame(0),uTotalFrames(0),fSpeed(0){}
	int		nAnimID;
	uint32	timeStart;
	uint32	timeEnd;
	uint32	uFrame;
	uint32	uTotalFrames;
	float	fSpeed;
	void Tick(uint32 uElapsedTime)
	{
		if (-1 < nAnimID)
		{
			uFrame += uint32(uElapsedTime*fSpeed);
			if (uFrame >= timeEnd)
			{
				uTotalFrames = timeEnd - timeStart;
				uFrame -= uTotalFrames;
			}
			else if (uFrame < timeStart)
			{
				uTotalFrames = timeEnd - timeStart;
				uFrame += uTotalFrames;
			}
		}
	}
};

class AnimManager
{
	ModelAnimation *anims;

	bool Paused;
	bool AnimParticles;

	AnimInfo animList[4];

	// 当前帧
	uint32 Frame;		// Frame number we're upto in the current animation
	uint32 TotalFrames;

	// 口型帧
	int AnimIDSecondary;
	// 口型帧
	uint32 FrameSecondary;

	// 口型动画ID
	int AnimIDMouth;
	// 口型帧
	uint32 FrameMouth;

	short Count;			// Total index of animations
	short PlayIndex;		// Current animation index we're upto
	short CurLoop;			// Current loop that we're upto.

	int TimeDiff;			// Difference in time between each frame

	float Speed;			// The speed of which to multiply the time given for Tick();
	float mouthSpeed;

	MyAnimInfo	m_AnimsInfo[ANIM_TYPE_MAX];
public:
	AnimManager(ModelAnimation *anim);
	~AnimManager();

	void AddAnim(uint32 id, short loop); // Adds an animation to our array.
	//void Set(short index, uint32 id, short loop); // sets one of the 4 existing animations and changes it (not really used currently)

	void SetSecondary(int id) {
		AnimIDSecondary = id;
		FrameSecondary = anims[id].timeStart;
	}
	void ClearSecondary() { AnimIDSecondary = -1; }
	int GetSecondaryID() { return AnimIDSecondary; }
	uint32 GetSecondaryFrame() { return FrameSecondary; }

	// For independent mouth movement.
	void SetMouth(int id) {
		AnimIDMouth = id;
		FrameMouth = anims[id].timeStart;
	}
	void ClearMouth() { AnimIDMouth = -1; }
	int GetMouthID() { return AnimIDMouth; }
	uint32 GetMouthFrame() { return FrameMouth; }
	void SetMouthSpeed(float speed) {
		mouthSpeed = speed;
	}

	void Play(); // Players the animation, and reconfigures if nothing currently inputed
	void Stop(); // Stops and resets the animation
	void Pause(bool force = false); // Toggles 'Pause' of the animation, use force to pause the animation no matter what.

	void Next(); // Plays the 'next' animation or loop
	void Prev(); // Plays the 'previous' animation or loop

	int Tick(int time);

	const ModelAnimation& getCurrentAnim();
	uint32 GetFrameCount();
	uint32 GetFrame() {return Frame;}
	void SetFrame(uint32 f);
	void SetSpeed(float speed) {Speed = speed;}
	float GetSpeed() {return Speed;}

	void PrevFrame();
	void NextFrame();

	void Clear();
	void Reset() { Count = 0; }

	bool IsPaused() { return Paused; }
	bool IsParticlePaused() { return !AnimParticles; }
	void AnimateParticles() { AnimParticles = true; }

	uint32 GetAnim() { return animList[PlayIndex].AnimID; }

	int GetTimeDiff();
	void SetTimeDiff(int i);
};