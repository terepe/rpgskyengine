#pragma once
#include "Vec2d.h"
#include "Vec3d.h"
#include <map>
#include <vector>

// block B - animations
struct ModelAnimation
{
	unsigned long animID;
	unsigned long timeStart;
	unsigned long timeEnd;

	float moveSpeed;

	unsigned long loopType;
	unsigned long flags;
	unsigned long d1;
	unsigned long d2;
	unsigned long playSpeed;  // note: this can't be play speed because it's 0 for some models

	Vec3D boxA, boxB;
	float rad;

	short s[2];
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
