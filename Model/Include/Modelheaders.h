#pragma once
#include "Common.h"
#include "Vec2d.h"
#include "Vec3d.h"


// block B - animations
struct ModelAnimation {
	uint32 animID;
	uint32 timeStart;
	uint32 timeEnd;

	float moveSpeed;

	uint32 loopType;
	uint32 flags;
	uint32 d1;
	uint32 d2;
	uint32 playSpeed;  // note: this can't be play speed because it's 0 for some models

	Vec3D boxA, boxB;
	float rad;

	int16 s[2];
};


struct TexGroup
{
	std::vector<std::string> strTexs;
	std::vector<int> Texs;
};

struct ModelSkin
{
	std::map<int, int> RepTex;// <ChannelID, TexID>
};
