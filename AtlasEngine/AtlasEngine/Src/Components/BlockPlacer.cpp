#include "Components\BlockPlacer.h"
#include "Input\ButtonEvent.h"
#include "Rendering\Camera.h"
#include "Debugging\DebugDraw.h"
#include "ChunkSystems\Block.h"
#include "ChunkSystems\BlockTypes.h"

CBlockPlacer::CBlockPlacer()
	: FBehavior()
	, mActiveType(0)
	, mPlacerRange(4.0f)
	, mShowBox(false)
{
}


CBlockPlacer::~CBlockPlacer()
{
}

void CBlockPlacer::Update()
{
	if (SButtonEvent::GetKeyDown(sf::Keyboard::R))
		ChangeType();


	if (SButtonEvent::GetKeyDown(sf::Keyboard::B))
		mShowBox = !mShowBox;

	if (mShowBox)
	{
		Vector3f CamForward = FCamera::Main->Transform.GetRotation() * -Vector3f::Forward * mPlacerRange;
		Vector3f CamPosition = FCamera::Main->Transform.GetWorldPosition() + CamForward;
		CamPosition = Vector3f{ std::floor(CamPosition.x) + 0.5f, std::floor(CamPosition.y) + 0.5f, std::floor(CamPosition.z) + 0.5f };
		FDebug::Draw::GetInstance().DrawBox(CamPosition, Vector3f{ 1, 1, 1 }, FBlockTypes::GetBlockColor(mActiveType));
	}

	FCamera& MainCamera = *FCamera::Main;

	if (SButtonEvent::GetMouseDown(sf::Mouse::Right))
	{
		Vector3f CamForward = MainCamera.Transform.GetRotation() * -Vector3f::Forward * mPlacerRange;
		Vector3f CamPosition = MainCamera.Transform.GetWorldPosition() + CamForward;
		DestroyBlock(CamPosition);
	}
	else if (SButtonEvent::GetMouseDown(sf::Mouse::Left))
	{
		Vector3f CamForward = MainCamera.Transform.GetRotation() * -Vector3f::Forward * mPlacerRange;
		Vector3f CamPosition = MainCamera.Transform.GetWorldPosition() + CamForward;
		SetBlock(CamPosition, (FBlockTypes::BlockID)mActiveType);
	}
}

void CBlockPlacer::ChangeType()
{
	mActiveType++;

	if (mActiveType == FBlock::AIR_BLOCK_ID)
		mActiveType = 0;
}