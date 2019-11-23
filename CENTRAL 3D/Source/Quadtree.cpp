#include "Globals.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "QuadTree.h"

#include "mmgr/mmgr.h"

// --- All child indexes ---
#define NET 0
#define SET 1
#define SWT 2
#define NWT 3
#define NEB 4
#define SEB 5
#define SWB 6
#define NWB 7

// --- Max items before subdividing ---
#define QUADTREE_MAX_ITEMS 10
#define QUADTREE_MIN_SIZE 10.0f 


QuadtreeNode::QuadtreeNode(const AABB& box) : box(box)
{
	parent = childs[NET] = childs[SET] = childs[SWT] = childs[NWT] = childs[NEB] = childs[SEB] = childs[SWB] = childs[NWB] = nullptr;
}


QuadtreeNode::~QuadtreeNode()
{
	for (int i = 0; i < 8; ++i)
	{
		if (childs[i] != nullptr) 
			delete(childs[i]);
	}
}

bool QuadtreeNode::IsLeaf() const
{
	return childs[0] == nullptr;
}

void QuadtreeNode::Insert(GameObject* go)
{
	if (IsLeaf() == true &&
		(objects.size() < QUADTREE_MAX_ITEMS ||
		(box.HalfSize().LengthSq() <= QUADTREE_MIN_SIZE * QUADTREE_MIN_SIZE)))
		objects.push_back(go);
	else
	{
		if (IsLeaf() == true)
			CreateChilds();

		objects.push_back(go);
		RedistributeChilds();
	}
}

void QuadtreeNode::Erase(GameObject * go)
{
	std::list<GameObject*>::iterator it = std::find(objects.begin(), objects.end(), go);
	if (it != objects.end())
		objects.erase(it);

	if (IsLeaf() == false)
	{
		for (int i = 0; i < 8; ++i)
			childs[i]->Erase(go);
	}
}

/*
		----------- MaxPoint
		| NW | NE |
		|---------|
		| SW | SE |
		-----------
MinPoint
*/

void QuadtreeNode::CreateChilds()
{
	// NorthEast - TOP
	childs[NET] = new QuadtreeNode(box);
	childs[NET]->CreateNode(NET);

	// SouthEast - TOP
	childs[SET] = new QuadtreeNode(box);
	childs[SET]->CreateNode(SET);

	// SouthWest - TOP
	childs[SWT] = new QuadtreeNode(box);
	childs[SWT]->CreateNode(SWT);

	// NorthWest - TOP
	childs[NWT] = new QuadtreeNode(box);
	childs[NWT]->CreateNode(NWT);


	// NorthEast - BOT
	childs[NEB] = new QuadtreeNode(box);
	childs[NEB]->CreateNode(NEB);

	// SouthEast - BOT
	childs[SEB] = new QuadtreeNode(box);
	childs[SEB]->CreateNode(SEB);

	// SouthWest - BOT
	childs[SWB] = new QuadtreeNode(box);
	childs[SWB]->CreateNode(SWB);

	// NorthWest - BOT
	childs[NWB] = new QuadtreeNode(box);
	childs[NWB]->CreateNode(NWB);


}

void QuadtreeNode::CreateNode(uint index)
{
	//Index positions from top view
	//0 1
	//2 3

	float3 minPoint, maxPoint;
	minPoint.y = this->box.minPoint.y;
	maxPoint.y = this->box.maxPoint.y;

	minPoint.x = (index / 2) == 1 ? this->box.minPoint.x : (this->box.maxPoint.x + this->box.minPoint.x) / 2;
	maxPoint.x = (index / 2) == 1 ? (this->box.maxPoint.x + this->box.minPoint.x) / 2 : this->box.maxPoint.x;

	minPoint.z = index % 2 == 0 ? this->box.minPoint.z : (this->box.maxPoint.z + this->box.minPoint.z) / 2;
	maxPoint.z = index % 2 == 0 ? (this->box.maxPoint.z + this->box.minPoint.z) / 2 : this->box.maxPoint.z;
	box = AABB(minPoint, maxPoint);
}

void QuadtreeNode::RedistributeChilds()
{
	// --- Redistribute all objects ---
	for (std::list<GameObject*>::iterator it = objects.begin(); it != objects.end();)
	{
		GameObject* go = *it;

		AABB new_box(go->GetOBB().MinimalEnclosingAABB());

		// --- Distribute this new gameobject onto the childs ---
		bool intersects[8];
		for (int i = 0; i < 8; ++i)
			intersects[i] = childs[i]->box.Intersects(new_box);

		if (intersects[0] && intersects[1] && intersects[2] && intersects[3]
			&& intersects[4] && intersects[5] && intersects[6] && intersects[7])
			++it; // if it hits all childs, better to just keep it here
		else
		{
			it = objects.erase(it);
			for (int i = 0; i < 8; ++i)
			{
				if (intersects[i])
				{
					childs[i]->Insert(go);
					break;
				}
			}
		}
	}
}

void QuadtreeNode::CollectObjects(std::vector<GameObject*>& objects) const
{
	for (std::list<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++it)
		objects.push_back(*it);

	for (int i = 0; i < 8; ++i)
		if (childs[i] != nullptr) childs[i]->CollectObjects(objects);
}

void QuadtreeNode::CollectObjects(std::map<float, GameObject*>& objects, const float3& origin) const
{
	for (std::list<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++it)
	{
		ComponentTransform* transform = (*it)->GetComponent<ComponentTransform>(Component::ComponentType::Transform);

		float dist = origin.DistanceSq(transform->GetGlobalPosition());
		objects[dist] = *it;
	}

	for (int i = 0; i < 8; ++i)
		if (childs[i] != nullptr) childs[i]->CollectObjects(objects, origin);
}

void QuadtreeNode::CollectBoxes(std::vector<const QuadtreeNode*>& nodes) const
{
	nodes.push_back(this);

	for (int i = 0; i < 8; ++i)
		if (childs[i] != nullptr) childs[i]->CollectBoxes(nodes);
}


// ---------------------------------------------------------------------

Quadtree::Quadtree()
{}

Quadtree::~Quadtree()
{
	Clear();
}


void Quadtree::SetBoundaries(const AABB& box)
{
	Clear();
	root = new QuadtreeNode(box);
}

void Quadtree::Insert(GameObject* go)
{
	if (root != nullptr)
	{
		if (go->GetOBB().MinimalEnclosingAABB().Intersects(root->box))
			root->Insert(go);
	}
}

void Quadtree::Erase(GameObject * go)
{
	if (root != nullptr)
		root->Erase(go);
}

void Quadtree::Clear()
{
	delete root;
}

void Quadtree::CollectBoxes(std::vector<const QuadtreeNode*>& nodes) const
{
	if (root != nullptr)
		root->CollectBoxes(nodes);
}

void Quadtree::CollectObjects(std::vector<GameObject*>& objects) const
{
	if (root != nullptr)
		root->CollectObjects(objects);
}

void Quadtree::CollectObjects(std::map<float, GameObject*>& objects, const float3& origin) const
{
	if (root != nullptr)
		root->CollectObjects(objects, origin);
}
