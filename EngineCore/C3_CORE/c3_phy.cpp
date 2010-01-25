#include "c3_phy.h"
#include "RenderSystem.h"
#include "IORead.h"

C3Motion::C3Motion()
{
	dwBoneCount = 0;
	dwFrames = 0;
	nFrame = 0;
	//lpFrames = 0;

	dwKeyFrames = 0;
	lpKeyFrame = 0;

	matrix = 0;
	nFrame = 0;
	dwMorphCount = 0;
	lpMorph = 0;
}

C3Motion::~C3Motion()
{
	for (uint32 kk = 0; kk < dwKeyFrames; kk++)
	{
		S_DELS (lpKeyFrame[kk].matrix);
	}
	S_DELS (lpKeyFrame);

	S_DELS (matrix);
	S_DELS (lpMorph);
}

C3Motion* C3Motion::New(IOReadBase& file)
{
	C3Motion* lpMotion = new C3Motion;
	if(lpMotion->Load(file))
	{
		return lpMotion;
	}
	S_DEL(lpMotion);
	return NULL;
}
bool C3Motion::Load(IOReadBase& file)
{
	// bone
	file.Read(&dwBoneCount, sizeof(uint32));
	// matrix
	file.Read(&dwFrames, sizeof(uint32));

	matrix = new Matrix[dwBoneCount];
	for (uint32 n = 0; n < dwBoneCount; n++)
	{
		matrix[n].unit();
	}

	// 是否有关键帧
	uint32 kf;
	file.Read(&kf, sizeof(uint32));
	kf=(kf<<24)+((kf&0xFF00)<<8)+((kf&0xFF0000)>>8)+(kf>>24);
	if ('KKEY' == kf)
	{
		file.Read(&dwKeyFrames, sizeof(uint32));
		lpKeyFrame = new C3KeyFrame[dwKeyFrames];

		for (uint32 kk = 0; kk < dwKeyFrames; kk++)
		{
			file.Read(&lpKeyFrame[kk].pos, sizeof(uint32));
			lpKeyFrame[kk].matrix = new Matrix[dwBoneCount];

			file.Read(lpKeyFrame[kk].matrix,
				sizeof (Matrix)*dwBoneCount);

			for (uint32 bb = 0; bb < dwBoneCount; bb++)
			{
// 				lpKeyFrame[kk].matrix[bb]._41 *= 0;
// 				lpKeyFrame[kk].matrix[bb]._42 *= 0;
// 				lpKeyFrame[kk].matrix[bb]._43 *= 0;
			}
		}
	}
	else if ('ZKEY' == kf)
	{	// 用运动分量购置运动矩阵, 减小数据冗余信息
		file.Read(&dwKeyFrames, sizeof (uint32));
		lpKeyFrame = new C3KeyFrame[dwKeyFrames];

		for (uint32 kk = 0; kk < dwKeyFrames; kk++)
		{
			uint16 wPos = 0;
			file.Read(&wPos,sizeof (uint16));// 注意这儿也减小了节省 2 个字节. :)

			lpKeyFrame[kk].pos = (uint32)wPos;

			lpKeyFrame[kk].matrix = new Matrix[dwBoneCount];


			struct DIV_INFO {
				Quaternion	quaternion;	// 旋转数据
				float x, y, z;				// 位移数据
			};

			DIV_INFO mx[256];	// 骨骼数不可能超过256
			file.Read(&mx, sizeof (DIV_INFO)*dwBoneCount);

			for (uint32 bb = 0; bb < dwBoneCount; bb++)
			{
				lpKeyFrame[kk].matrix[bb].unit();
				lpKeyFrame[kk].matrix[bb].quaternionRotate(mx[bb].quaternion);

				lpKeyFrame[kk].matrix[bb]._41 = mx[bb].x;
				lpKeyFrame[kk].matrix[bb]._42 = mx[bb].y;
				lpKeyFrame[kk].matrix[bb]._43 = mx[bb].z;
				lpKeyFrame[kk].matrix[bb]._44 = 1.0f;
			}			
		}
	}
	else
	{
		file.Seek(-4, SEEK_CUR);

		dwKeyFrames = dwFrames;
		lpKeyFrame = new C3KeyFrame[dwFrames];

		for (uint32 kk = 0; kk < dwFrames; kk++)
		{
			lpKeyFrame[kk].pos = kk;
			lpKeyFrame[kk].matrix = new Matrix[dwBoneCount];
		}
		for (uint32 bb = 0; bb < dwBoneCount; bb++)
		{
			for (uint32 kk = 0; kk < dwFrames; kk++)
			{
				file.Read(lpKeyFrame[kk].matrix[bb],
					sizeof (Matrix));
// 				lpKeyFrame[kk].matrix[bb]._41 *= 0;
// 				lpKeyFrame[kk].matrix[bb]._42 *= 0;
// 				lpKeyFrame[kk].matrix[bb]._43 *= 0;
			}
		}
	}

	// morph
	file.Read(&dwMorphCount, sizeof (uint32));

	lpMorph = new float[dwMorphCount * dwFrames];
	file.Read(lpMorph,
			sizeof (float)*
			dwMorphCount * dwFrames);
	return true;
}

bool C3Motion::Save(char *lpName, bool bNew)
{
	FILE *file = fopen (lpName, bNew ? "w+b" : "r+b");
	if (!file)
		return false;
	fseek (file, 0, SEEK_END);

	// phy
	ChunkHeader chunk;
	chunk.dwChunkID = 'MOTI';
	chunk.dwChunkSize = 0;
	fwrite (&chunk, sizeof (chunk), 1, file);

	// bone
	fwrite (&dwBoneCount, sizeof (uint32), 1, file);
	chunk.dwChunkSize += sizeof (uint32);

	// matrix
	fwrite (&dwFrames, sizeof (uint32), 1, file);
	chunk.dwChunkSize += sizeof (uint32);

	// key 标志
	uint32 kf = 'KKEY';

	fwrite (&kf, sizeof (uint32), 1, file);
	chunk.dwChunkSize += sizeof (uint32);

	fwrite (&dwKeyFrames,
			 sizeof (uint32),
			 1,
			 file);
	chunk.dwChunkSize += sizeof (uint32);

	for (uint32 kk = 0; kk < dwKeyFrames; kk++)
	{
		fwrite (&lpKeyFrame[kk].pos,
				 sizeof (uint32),
				 1,
				 file);
		chunk.dwChunkSize += sizeof (uint32);

		fwrite (lpKeyFrame[kk].matrix,
				 sizeof (Matrix),
				 dwBoneCount,
				 file);
		chunk.dwChunkSize += sizeof (Matrix) * dwBoneCount;
	}

	// morph
	fwrite (&dwMorphCount, sizeof (uint32), 1, file);
	chunk.dwChunkSize += sizeof (uint32);

	fwrite (lpMorph,
			 sizeof (float),
			 dwMorphCount * dwFrames,
			 file);
	chunk.dwChunkSize += sizeof (float) * dwMorphCount * dwFrames;

	fseek (file, -(int)(chunk.dwChunkSize + sizeof (chunk)), SEEK_CUR);
	fwrite (&chunk, sizeof (chunk), 1, file);
	fseek (file, 0, SEEK_END);

	fclose (file);
	return true;
}

void C3Motion::GetMatrix(uint32 dwBone, Matrix *lpMatrix)
{
	// alpha
	int sindex = -1;
	int eindex = -1;
	for (int n = 0; n < (int)dwKeyFrames; n++)
	{
		if (lpKeyFrame[n].pos <= (uint32)nFrame)
		{
			if (sindex == -1 || n > sindex)
				sindex = n;
		}
		if (lpKeyFrame[n].pos > (uint32)nFrame)
		{
			if (eindex == -1 || n < eindex)
				eindex = n;
		}
	}
	if (sindex == -1 && eindex > -1)
	{
		*lpMatrix = lpKeyFrame[eindex].matrix[dwBone];
	}
	else if (sindex > -1 && eindex == -1)
	{
		*lpMatrix = lpKeyFrame[sindex].matrix[dwBone];
	}
	else if (sindex > -1 && eindex > -1)
	{
		*lpMatrix = lpKeyFrame[sindex].matrix[dwBone];// + 
			//((float)(nFrame - lpKeyFrame[sindex].pos) / (float)(lpKeyFrame[eindex].pos - lpKeyFrame[sindex].pos)) * 
			//(lpKeyFrame[eindex].matrix[dwBone] - lpKeyFrame[sindex].matrix[dwBone]);
	}
}

C3Phy::C3Phy()
{
	lpName = 0;

	dwBlendCount = 0;

	dwNVecCount = 0;		// 顶点数(普通顶点)
	dwAVecCount = 0;		// 顶点数(透明顶点)
	lpVB = 0;			// 顶点池(普通顶点/透明顶点)
	vb = 0;

	dwNTriCount = 0;		// 多边形数(普通多边形)
	dwATriCount = 0;		// 多边形数(透明多边形)
	lpIB = 0;			// 索引池(普通多边形/透明多边形)
	ib = 0;

	nTex = -1;

	lpMotion = 0;

	fA = fR = fG = fB = 1.0f;
	bDraw = true;
	dwTexRow = 1;
	uvstep.x = 0;
	uvstep.y = 0;

	InitMatrix.unit();
}
C3Phy::~C3Phy()
{
	S_DELS (lpName);
	S_DELS (lpVB);
	S_DEL (vb);
	S_DELS (lpIB);
	S_DEL (ib);

	S_DELS (Key.lpAlphas);
	S_DELS (Key.lpDraws);
	S_DELS (Key.lpChangeTexs);
}

C3Phy* C3Phy::New(IOReadBase& file, bool bTex)
{
	C3Phy* lpPhy = new C3Phy;
	if(lpPhy->Load(file, bTex))
	{
		return lpPhy;
	}
	S_DEL(lpPhy);
	return NULL;
}

bool C3Phy::Load(IOReadBase& file, bool bTex)
{
	// 读入名称
	uint32 temp;
	file.Read (&temp, sizeof (unsigned long));
	lpName = new char[temp + 1];
	file.Read (lpName, temp);
	lpName[temp] = '\0';

	// blend
	file.Read (&dwBlendCount, sizeof (uint32));

	// vec count
	file.Read (&dwNVecCount, sizeof (uint32));
	file.Read (&dwAVecCount, sizeof (uint32));


	CRenderSystem& R = GetRenderSystem();

	// 创建输出 vb
	vb = R.GetHardwareBufferMgr().CreateVertexBuffer (dwNVecCount + dwAVecCount, sizeof (PhyOutVertex),
		CHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);


	// 读取顶点
	lpVB = new PhyVertex[dwNVecCount + dwAVecCount];
	file.Read(lpVB,
			sizeof (PhyVertex)*
			(dwNVecCount + dwAVecCount));

	PhyOutVertex *vertex;
	vertex = (PhyOutVertex*)vb->lock (CHardwareBuffer::HBL_NORMAL);

	for (uint32 n = 0; n < dwNVecCount + dwAVecCount; n++)
	{
		vertex[n].p = lpVB[n].pos[0]* 0.01f;
		vertex[n].color = 0xFFFFFFFF;
		vertex[n].t = lpVB[n].t;
		//float y = lpVB[n].pos[0].y;
		//lpVB[n].pos[0].y = -lpVB[n].pos[0].z;
		//lpVB[n].pos[0].z = y;
// 		lpVB[n].pos[0].x *= 0.001;
// 		lpVB[n].pos[0].y *= 0.001;
// 		lpVB[n].pos[0].z *= 0.001;
	}

	vb->unlock ();


	// tri count
	file.Read (&dwNTriCount, sizeof (uint32));
	file.Read (&dwATriCount, sizeof (uint32));
	// tri
	ib = R.GetHardwareBufferMgr().CreateIndexBuffer ((dwNTriCount + dwATriCount)* 3);

	uint16 *tri = (uint16 *)ib->lock (CHardwareBuffer::HBL_NORMAL);
	file.Read(tri, sizeof (uint16)* (dwNTriCount + dwATriCount) * 3);

	/*
	// 优化
	PrimitiveGroup *nl, *al;
	unsigned short ns, as;

	SetListsOnly (true);

	if (dwNTriCount > 0)
	{
		if (!(tri[0] == tri[1] ||
			 tri[1] == tri[2] ||
			 tri[2] == tri[0]))
		{
			GenerateStrips ((const unsigned short*)tri,
							 dwNTriCount * 3,
							 &nl,
							 &ns);
			CopyMemory (tri,
						 nl->indices,
						 nl->numIndices * sizeof (uint16));
			S_DELS (nl);
		}
		else
		{
			//MessageBox (g_hWnd, lpName, "file erro", MB_OK);
		}
	}
	if (dwATriCount > 0)
	{
		if (!(tri[0] == tri[1] ||
			 tri[1] == tri[2] ||
			 tri[2] == tri[0]))
		{
			GenerateStrips ((const unsigned short*)(tri + dwNTriCount * 3),
							 dwATriCount * 3,
							 &al,
							 &as);

			CopyMemory (tri + dwNTriCount * 3,
						 al->indices,
						 al->numIndices * sizeof (uint16));
			S_DELS (al);
		}
		else
		{
			//MessageBox (g_hWnd, lpName, "file erro", MB_OK);
		}
	}
	*/

	ib->unlock ();

	// 读入贴图
	file.Read (&temp, sizeof (uint32));
	char szTemp[255]={0};
	file.Read(szTemp,temp);
	strTexName = szTemp;
	// 创建贴图
	if (bTex)
	{
		nTex = GetRenderSystem().GetTextureMgr().RegisterTexture(strTexName, 1);
		if (nTex == -1)
			return false;
	}

	// bounding box
	file.Read (&bbox, sizeof (BBox));

	Matrix	InitMatrix;
	file.Read (&InitMatrix, sizeof (Matrix));

	// 用户自定义消息
	file.Read (&dwTexRow, sizeof (uint32));

	file.Read (&Key.dwAlphas, sizeof (uint32));
	Key.lpAlphas = new C3Frame[Key.dwAlphas];
	file.Read(Key.lpAlphas, sizeof (C3Frame)*Key.dwAlphas);

	file.Read (&Key.dwDraws, sizeof (uint32));
	Key.lpDraws = new C3Frame[Key.dwDraws];
	file.Read(Key.lpDraws, sizeof (C3Frame)* Key.dwDraws);

	file.Read (&Key.dwChangeTexs, sizeof (uint32));
	Key.lpChangeTexs = new C3Frame[Key.dwChangeTexs];
	file.Read(Key.lpChangeTexs, sizeof (C3Frame)* Key.dwChangeTexs);

	char flag[4];
	// 12-20-2002 : STEP
	file.Read(flag, sizeof (char)*4);

	if (flag[0] == 'S' &&
		 flag[1] == 'T' &&
		 flag[2] == 'E' &&
		 flag[3] == 'P')
	{
		file.Read (&uvstep.x, sizeof (float));
		file.Read (&uvstep.y, sizeof (float));
	}
	else
	{
		file.Move(-4) ;
	}

	SetColor(1, 1, 1, 1);
	return true;
}


bool C3Phy::Save(char *lpName, bool bNew)
{
	return true;
}

void C3Phy::Prepare()
{
	CRenderSystem& R = GetRenderSystem();
	R.SetTextureColorOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
	R.SetTextureAlphaOP(0, TBOP_MODULATE, TBS_TEXTURE, TBS_DIFFUSE);
	R.SetTextureColorOP(1, TBOP_DISABLE);
	R.SetTextureAlphaOP(1, TBOP_DISABLE);
	
}

bool C3Phy::Calculate()
{
	if (!lpMotion)
	{
		return false;
	}
	float alpha;
	if (Key.ProcessAlpha(lpMotion->nFrame, lpMotion->dwFrames, &alpha))
		fA = alpha;

	bool draw;
	if (Key.ProcessDraw(lpMotion->nFrame, &draw))
		bDraw = draw;

	int tex = -1;
	Key.ProcessChangeTex(lpMotion->nFrame, &tex);

	if (!bDraw)
		return true;


	PhyOutVertex *vertex =(PhyOutVertex *) vb->lock (CHardwareBuffer::HBL_NO_OVERWRITE);

	Matrix *bone = new Matrix[lpMotion->dwBoneCount];
	for (uint32 b = 0; b < lpMotion->dwBoneCount; b++)
	{
		Matrix mm;
		lpMotion->GetMatrix(b, &mm);
/*
		MatrixMultiply (&bone[b],
							 &InitMatrix, 
							 &lpMotion->lpFrames[b * lpMotion->dwFrames + 
							 lpMotion->nFrame]);
*/
		bone[b]=mm*InitMatrix;

		//bone[b] = InitMatrix;

		bone[b]=bone[b]*lpMotion->matrix[b];
	}

	// 计算皮肤
	for (uint32 v = 0; v < dwNVecCount + dwAVecCount; v++)
	{
		Vec3D mix = lpVB[v].pos[0];
		// 计算 MORPH
/*		bool zero[3] = { true, true, true };
		for (uint32 m = 0; m < lpMotion->dwMorphCount; m++)
		{
			// x
			if (FloatCmp (lpVB[v].pos[0].x, lpVB[v].pos[m + 1].x) != 
0)
			{
				mix.x += (lpVB[v].pos[m + 1].x - lpVB[v].pos[0].x) * 
lpMotion->lpMorph[lpMotion->dwMorphCount * 
lpMotion->nFrame + m];
			}
			// y
			if (FloatCmp (lpVB[v].pos[0].y, lpVB[v].pos[m + 1].y) != 
0)
			{
				mix.y += (lpVB[v].pos[m + 1].y - lpVB[v].pos[0].y) * 
lpMotion->lpMorph[lpMotion->dwMorphCount * 
lpMotion->nFrame + m];
			}
			// z
			if (FloatCmp (lpVB[v].pos[0].z, lpVB[v].pos[m + 1].z) != 
0)
			{
				mix.z += (lpVB[v].pos[m + 1].z - lpVB[v].pos[0].z) * 
lpMotion->lpMorph[lpMotion->dwMorphCount * 
lpMotion->nFrame + m];
			}
		}

*/


		// 计算骨骼
		Vec3D vec;
		vertex[v].p = Vec3D(0.0f,0.0f,0.0f);

		for (int l = 0; l < 2; l++)
		{
			uint32 index = lpVB[v].index[l];
			float weight = lpVB[v].weight[l];

			if (lpVB[v].weight[l] > 0)
			{

/*				D3DXVec3TransformCoord (&vec,
										 &mix,
										 &InitMatrix);
				D3DXVec3TransformCoord (&vec,
										 &vec,
										 &lpMotion->lpFrames[index * lpMotion->dwFrames + 
										 lpMotion->nFrame]);
				D3DXVec3TransformCoord (&vec,
										 &vec,
										 &lpMotion->matrix[index]);*/

				vec= bone[index]*mix;

				//D3DXVec3Scale (&vec, &vec, weight);
				vertex[v].p+=vec;
				break;
			}
		}

		vertex[v].p = Vec3D(vertex[v].p.x,-vec.z, vec.y);
		vertex[v].p *= 0.01f;

		//接口内部只改变改变x, y
//		D3DXVec2Add ((Vec2D*)&vertex[v],
//					  (Vec2D*)&vertex[v],
//					  &uvstep);
		//要改变u, v
		vertex[v].t += uvstep;

		if (tex > -1)
		{
			float segsize = 1.0f / dwTexRow;
			vertex[v].t.x = lpVB[v].t.x + tex % dwTexRow * segsize;
			vertex[v].t.y = lpVB[v].t.y + tex / dwTexRow * segsize;
		}
	}
	vb->unlock ();

	delete [] bone;

	return true;
}

bool C3Phy::DrawNormal()
{
	if (!bDraw)
		return true;

	if (dwNTriCount > 0 && fA == 1.0f)
	{
		CRenderSystem& R = GetRenderSystem();

		R.SetDepthBufferFunc(true, true);
		R.SetBlendFunc(false);
		R.SetAlphaTestFunc(false);

		// material
// 		D3DMATERIAL9 material;
// 		ZeroMemory (&material, sizeof (material));
// 
// 		material.Diffuse.a = fA;
// 		material.Diffuse.r = fR;
// 		material.Diffuse.g = fG;
// 		material.Diffuse.b = fB;
// 
// 		material.Ambient.a = fA;
// 		material.Ambient.r = fR;
// 		material.Ambient.g = fG;
// 		material.Ambient.b = fB;
// 
// 		R.SetMaterial (&material);


		// texture
		R.SetTexture(0, nTex);

		Matrix mUnit;
		mUnit.unit();
		//R.SetWorldMatrix(mUnit);
		R.SetFVF (PHY_OUT_VERTEX);

		R.SetStreamSource (0, vb, 0, sizeof (PhyOutVertex));
		R.SetIndices (ib);

		// draw normal
		R.DrawIndexedPrimitive (VROT_TRIANGLE_LIST,
														  0,0,
														  dwNVecCount,
														  0,
														  dwNTriCount);
			return false;
	}
	return true;
}
DLL_EXPORT
bool C3Phy::DrawAlpha(bool bZ, int nAsb, int nAdb)
{
	if (!bDraw)
		return true;

	CRenderSystem& R = GetRenderSystem();
	if ((dwNTriCount > 0 && fA != 1.0f) ||
		 (dwATriCount > 0))
	{
		R.SetDepthBufferFunc(true, bZ);
		R.SetBlendFunc(true);
		R.SetAlphaTestFunc(true, CMPF_GREATER_EQUAL, 0x08);

		// material
// 		D3DMATERIAL9 material;
// 		ZeroMemory (&material, sizeof (material));
// 
// 		material.Diffuse.a = fA;
// 		material.Diffuse.r = fR;
// 		material.Diffuse.g = fG;
// 		material.Diffuse.b = fB;
// 
// 		material.Ambient.a = fA;
// 		material.Ambient.r = fR;
// 		material.Ambient.g = fG;
// 		material.Ambient.b = fB;
// 
// 		R.SetMaterial (&material);

		// texture
		R.SetTexture(0, nTex);

		Matrix mUnit;
		mUnit.unit();
		//R.SetWorldMatrix(mUnit);
		R.SetFVF (PHY_OUT_VERTEX);

		R.SetStreamSource (0, vb, 0, sizeof (PhyOutVertex));

		R.SetIndices(ib);
	}
	if (dwNTriCount > 0 && fA != 1.0f)
	{
		// draw normal
		R.DrawIndexedPrimitive (VROT_TRIANGLE_LIST,
														  0,0,
														  dwNVecCount,
														  0,
														  dwNTriCount);
			return false;
	}
	if (dwATriCount > 0)
	{
		// draw alpha
		R.DrawIndexedPrimitive (VROT_TRIANGLE_LIST,
														  dwNVecCount,
														  0,//????????????????
														  dwAVecCount,
														  dwNTriCount * 3,
														  dwATriCount);
			return false;
	}
	return true;
}

void C3Phy::NextFrame(int nStep)
{
	lpMotion->nFrame = (lpMotion->nFrame + nStep) % lpMotion->dwFrames;
}

void C3Phy::SetFrame(uint32 dwFrame)
{
	if(lpMotion->dwFrames == 0)
	{
		lpMotion->nFrame = 0;
	}
	else
	{
		lpMotion->nFrame = dwFrame % lpMotion->dwFrames;
	}
}

void C3Phy::Muliply(int nBoneIndex, Matrix *matrix)
{
	GetRenderSystem().setWorldMatrix(*matrix);
	//int start, end;
	//if (nBoneIndex == -1)
	//{
	//	start = 0;
	//	end = lpMotion->dwBoneCount;
	//}
	//else
	//{
	//	start = nBoneIndex;
	//	end = start + 1;
	//}
	//for (int n = start; n < end; n++)
	//{
	//	MatrixMultiply (&lpMotion->matrix[n],
	//						 &lpMotion->matrix[n],
	//						 matrix);
	//}
}

void C3Phy::SetColor(float alpha, float red, float green, float blue)
{
	fA = alpha;
	fR = red;
	fG = green;
	fB = blue;
}

void C3Phy::ClearMatrix()
{
	// 清除运动矩阵
	for (uint32 n = 0; n < lpMotion->dwBoneCount; n ++)
	{
		lpMotion->matrix[n].unit();
	}
}

void C3Phy::ChangeTexture(int nChangeTexID, int nChangeTexID2)
{
	nTex = nChangeTexID;
	nTex2 = nChangeTexID2;
}