#include "Pch.h"
#include "EngineCore.h"
#include "SimpleScene.h"
#include "Camera2.h"

void SimpleScene::ListNodes(DrawBatch2& batch)
{
	const FrustumPlanes& frustum = camera->GetFrustum();
	for(SceneNode2* node : nodes)
	{
		if(frustum.SphereToFrustum(node->))
	}
}

void SimpleScene::Update(float dt)
{
	for(SceneNode2* node : nodes)
	{
		if(node->GetMeshInstance())
			node->GetMeshInstance()->Update(dt);
	}
}