#include "AnimMgr.h"

AnimManager::AnimManager(ModelAnimation *anim) {
	AnimIDSecondary = -1;
	AnimIDMouth = -1;
	anims = anim;
	AnimParticles = false;

	Count = 1;
	PlayIndex = 0;
	CurLoop = 0;
	Speed = 1.0f;
	mouthSpeed = 1.0f;
	Paused = false;

	Frame = 0;
	TotalFrames = 0;

	if (anims != NULL)
	{ 
	//	Set(0, 0, 0);
	}
}

AnimManager::~AnimManager() {
	anims = NULL;
}


void AnimManager::AddAnim(uint32 id, short loops)
{
	if (Count > 3)
		return;

	animList[Count].AnimID = id;
	animList[Count].Loops = loops;
	Count++;
}

// void AnimManager::Set(short index, const std::string& strAnimName, short loops)
// {
// 	// error check, we currently only support 4 animations.
// 	if (index > 3)
// 		return;
// 
// 	animList[index].AnimID = strAnimName;
// 	animList[index].Loops = loops;
// 
// 
// 	// Just an error check for our "auto animate"
// 	if (index == 0)
// 	{
// 		if (Count == 0)
// 		{
// 			Count = 1;
// 		}
// 		PlayIndex = index;
// 		Frame = anims[id].timeStart;
// 		TotalFrames = anims[id].timeEnd - anims[id].timeStart;
// 	}
// }

void AnimManager::Play() {
	PlayIndex = 0;
	//if (Frame == 0 && PlayID == 0) {
		CurLoop = animList[PlayIndex].Loops;
		Frame = anims[animList[PlayIndex].AnimID].timeStart;
		TotalFrames = anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart;
	//}

	Paused = false;
	AnimParticles = false;
}

void AnimManager::Stop()
{
	Paused = true;
	PlayIndex = 0;
	Frame = anims[animList[0].AnimID].timeStart;
	CurLoop = animList[0].Loops;
}

void AnimManager::Pause(bool force) {
	if (Paused && force == false) {
		Paused = false;
		AnimParticles = !Paused;
	} else {
		Paused = true;
		AnimParticles = !Paused;
	}
}

void AnimManager::Next()
{
	if(CurLoop == 1) // 进入下一动画
	{
		PlayIndex++;
		if (PlayIndex >= Count)
		{
			Stop();
			return;
		}
		CurLoop = animList[PlayIndex].Loops;
	}
	else if(CurLoop > 1) // 进入当前动画的下一循环
	{
		CurLoop--;
	}
	// 帧移动到动画最开始处
	Frame = anims[animList[PlayIndex].AnimID].timeStart;
}

void AnimManager::Prev()
{
	if(CurLoop >= animList[PlayIndex].Loops)
	{
		PlayIndex--;

		if (PlayIndex < 0)
		{
			Stop();
			return;
		}

		CurLoop = animList[PlayIndex].Loops;
	}
	else if(CurLoop < animList[PlayIndex].Loops)
	{
		CurLoop++;
	}

	Frame = anims[animList[PlayIndex].AnimID].timeEnd;
}

int AnimManager::Tick(int time)
{
	if (Paused)
	{
		return 1;
	}
	if((Count < PlayIndex))
		return -1;

	globalTime += time;

	for (int i=0; i<ANIM_TYPE_MAX; i++)
	{
		m_AnimsInfo[i].Tick(time);
	}

	// 
	Frame += uint32(time*Speed);
	if (Frame >= anims[animList[PlayIndex].AnimID].timeEnd)
	{
		Next();
		return 1;
	}
	else if (Frame < anims[animList[PlayIndex].AnimID].timeStart)
	{
		Prev();
		return 1;
	}

	return 0;
}

const ModelAnimation& AnimManager::getCurrentAnim()
{
	return anims[animList[PlayIndex].AnimID];
}

uint32 AnimManager::GetFrameCount()
{
	return (anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart);
}


void AnimManager::NextFrame()
{
	//AnimateParticles();
	int id = animList[PlayIndex].AnimID;
	Frame += int((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = int((anims[id].timeEnd - anims[id].timeStart) / 60);

	Pause(true);
}

void AnimManager::PrevFrame()
{
	//AnimateParticles();
	int id = animList[PlayIndex].AnimID;
	Frame -= int((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = int((anims[id].timeEnd - anims[id].timeStart) / 60) * -1;

	Pause(true);
}

void AnimManager::SetFrame(uint32 f)
{
	//TimeDiff = f - Frame;
	Frame = f;
}

int AnimManager::GetTimeDiff()
{
	int t = TimeDiff;
	TimeDiff = 0;
	return t;
}

void AnimManager::SetTimeDiff(int i)
{
	TimeDiff = i;
}

void AnimManager::Clear() {
	Stop();
	Paused = true;
	PlayIndex = 0;
	Count = 0;
	CurLoop = 0;
	Frame = 0;
}