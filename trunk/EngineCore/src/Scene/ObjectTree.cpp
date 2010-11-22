#include "ObjectTree.h"

ObjectTree::ObjectTree()
{
	pChild=NULL;
}

ObjectTree::~ObjectTree()
{
	clearObjects();
	if (pChild)
	{
		delete[] pChild;
	}
}

const BBox& ObjectTree::getBBox()const
{
	return bbox;
}

void ObjectTree::clearObjects()
{
	for (DEQUE_MAPOBJ::iterator it = m_setObjet.begin();
		it != m_setObjet.end(); ++it)
	{
		(*it)->release();
	}
	m_setObjet.clear();
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].clearObjects();
		}
	}
}

//void ObjectTree::setupObjectBBox()
//{
//	if (pChild)
//	{
//		if (pChild[0].m_setObjet.size()>0)
//		{
//			pChild[0].setupObjectBBox();
//			bboxObject=pChild[0].bboxObject;
//			if (pChild[1].m_setObjet.size()>0)
//			{
//				pChild[1].setupObjectBBox();
//				bboxObject+=pChild[1].bboxObject;
//			}
//		}
//		else if (pChild[1].m_setObjet.size()>0)
//		{
//			pChild[1].setupObjectBBox();
//			bboxObject=pChild[1].bboxObject;
//		}
//	}
//	else
//	{
//		if (m_setObjet.size()>0)
//		{
//			bboxObject=m_setObjet[0]->GetBBox();
//		}
//		for (std::vector<CMapObj*>::iterator it=m_setObjet.begin();it!=m_setObjet.end();it++)
//		{
//			BBox box = (*it)->GetBBox();
//			bboxObject+=box;
//		}
//	}
//}

void ObjectTree::getObjectsByBBox(const BBox& box,const std::vector<CMapObj*>& setSrcObject, std::vector<CMapObj*>& setDestObject)
{
	for (std::vector<CMapObj*>::const_iterator it=setSrcObject.begin();it!=setSrcObject.end();it++)
	{
		if (box.crossVertex((*it)->getPos()))
		{
			setDestObject.push_back(*it);
		}
	}
}

//void ObjectTree::getObjectTreesByFrustum(const CFrustum& frustum, std::vector<ObjectTree*>& setObjectTree)
//{
//	if (0==m_setObjet.size())
//	{
//		return;
//	}
//	CrossRet crossRet = frustum.CheckAABBVisible(bboxObject);
//	if (cross_include == crossRet)
//	{
//		setObjectTree.push_back(this);
//	}
//	else if (cross_cross == crossRet)
//	{
//		if (pChild)
//		{
//			for (size_t i=0;i<2;++i)
//			{
//				pChild[i].getObjectTreesByFrustum(frustum, setObjectTree);
//			}
//		}
//		else
//		{
//			setObjectTree.push_back(this);
//		}
//	}
//}

void ObjectTree::getObjects(DEQUE_MAPOBJ& setObject)
{
	setObject.insert(setObject.end(), m_setObjet.begin(), m_setObjet.end());
	for (size_t i=0;i<8;++i)
	{
		if(pChild != NULL)
			pChild[i].getObjects(setObject);
	}
}
void ObjectTree::getObjectsByPos(Vec3D vPos, DEQUE_MAPOBJ& setObject)
{
	for (DEQUE_MAPOBJ::iterator it=m_setObjet.begin();it!=m_setObjet.end();it++)
	{
		if ((*it)->getPos()==vPos)
		{
			setObject.push_back(*it);
		}
	}
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].getObjectsByPos(vPos,setObject);
		}
	}
}

void ObjectTree::getObjectsByCell(Pos2D posCell, DEQUE_MAPOBJ& setObject)
{
	for (DEQUE_MAPOBJ::iterator it=m_setObjet.begin();it!=m_setObjet.end();it++)
	{
		Pos2D posMyCell=(*it)->getCellPos();
		if (posMyCell==posCell)
		{
			setObject.push_back(*it);
		}
	}
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			pChild[i].getObjectsByCell(posCell,setObject);
		}
	}
}

void ObjectTree::getObjectsByFrustum(const CFrustum& frustum, DEQUE_MAPOBJ& setObject)
{
	CrossRet crossRet = frustum.CheckAABBVisible(bbox);
	if (cross_include == crossRet)
	{
		getObjects(setObject);
	}
	else if (cross_cross == crossRet)
	{
		for (DEQUE_MAPOBJ::iterator it=m_setObjet.begin();it!=m_setObjet.end();it++)
		{
			CrossRet crossRet = frustum.CheckAABBVisible((*it)->getBBox());
			if (cross_exclude != crossRet)
			{
				setObject.push_back((*it));
			}
		}
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				pChild[i].getObjectsByFrustum(frustum, setObject);
			}
		}
	}
}

ObjectTree* ObjectTree::getNodeByAABB(const BBox& box)
{
	CrossRet crossRet = bbox.checkAABBVisible(box);
	if (cross_include == crossRet)
	{
		if (pChild)
		{
			for (size_t i=0;i<8;++i)
			{
				ObjectTree* pNode = pChild[i].getNodeByAABB(box);
				if (pNode)
				{
					return pNode;
				}
			}
		}
		return this;
	}
	return NULL;
}

bool ObjectTree::addObject(CMapObj* pObject)
{
	ObjectTree* pNode = getNodeByAABB(pObject->getBBox());
	if (pNode)
	{
		pNode->m_setObjet.push_back(pObject);
		return true;
	}
	return false;
}

bool ObjectTree::delObject(CMapObj* pObject)
{
	if(eraseObject(pObject))
	{
		pObject->release();	// del self
		return true;
	}
	return false;
}

bool ObjectTree::eraseObject(CMapObj* pObject)
{
	DEQUE_MAPOBJ::iterator it = std::find( m_setObjet.begin( ), m_setObjet.end( ), pObject );
	if(it!=m_setObjet.end())
	{
		m_setObjet.erase(it);
		return true;
	}
	else if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			if (pChild[i].eraseObject(pObject))
			{
				return true;
			}
		}
	}
	return false;
}

bool ObjectTree::updateObject(CMapObj* pObject)
{
	if(eraseObject(pObject))
	{
		addObject(pObject);
		return true;
	}
	return false;
}

ObjectTree* ObjectTree::find(CMapObj* pObject)
{
	DEQUE_MAPOBJ::iterator it = std::find( m_setObjet.begin( ), m_setObjet.end( ), pObject );
	if(it!=m_setObjet.end())
	{
		return this;
	}
	else if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			ObjectTree* pParentObjectTree = pChild[i].find(pObject);
			if (pParentObjectTree)
			{
				return pParentObjectTree;
			}
		}
	}
	return NULL;
}

void ObjectTree::create(const BBox& box, size_t size)
{
	bbox = box;
	if (size>0)
	{
		size--;
		if (pChild)
		{
			delete[] pChild;
		}
		pChild = new ObjectTree[8];
		BBox childBoxs[8];
		for (size_t i=0;i<8;++i)
		{
			childBoxs[i] = bbox;
		}
		Vec3D vMiddle = (box.vMax+box.vMin)*0.5f;
		childBoxs[0].vMin.z=vMiddle.z;
		childBoxs[1].vMin.z=vMiddle.z;
		childBoxs[2].vMin.z=vMiddle.z;
		childBoxs[3].vMin.z=vMiddle.z;

		childBoxs[4].vMax.z=vMiddle.z;
		childBoxs[5].vMax.z=vMiddle.z;
		childBoxs[6].vMax.z=vMiddle.z;
		childBoxs[7].vMax.z=vMiddle.z;

		childBoxs[0].vMin.y=vMiddle.y;
		childBoxs[1].vMin.y=vMiddle.y;
		childBoxs[4].vMin.y=vMiddle.y;
		childBoxs[5].vMin.y=vMiddle.y;

		childBoxs[2].vMax.y=vMiddle.y;
		childBoxs[3].vMax.y=vMiddle.y;
		childBoxs[6].vMax.y=vMiddle.y;
		childBoxs[7].vMax.y=vMiddle.y;


		childBoxs[0].vMin.x=vMiddle.x;
		childBoxs[2].vMin.x=vMiddle.x;
		childBoxs[4].vMin.x=vMiddle.x;
		childBoxs[6].vMin.x=vMiddle.x;

		childBoxs[1].vMax.x=vMiddle.x;
		childBoxs[3].vMax.x=vMiddle.x;
		childBoxs[5].vMax.x=vMiddle.x;
		childBoxs[7].vMax.x=vMiddle.x;

		for (size_t i=0;i<8;++i)
		{
			pChild[i].create(childBoxs[i], size);
		}
	}
}

void ObjectTree::process()
{
	for (DEQUE_MAPOBJ::iterator it=m_setObjet.begin();it!=m_setObjet.end();it++)
	{
		//(*it)->Process(NULL);
	}
	if (pChild)
	{
		for (size_t i=0;i<8;++i)
		{
			//pChild[i].process();
		}
	}
}