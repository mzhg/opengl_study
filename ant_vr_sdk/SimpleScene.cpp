#include "SimpleScene.h"

namespace jet
{
	namespace util
	{
		static const std::string BOX_NAME = "BoxNode";

		SimpleScene::SimpleScene()
		{
			
		}


		SimpleScene::~SimpleScene()
		{
			delete m_pBoxNode;
		}

		void SimpleScene::initialize()
		{
			m_pBox = ShapePtr(new Box(Box::Mode::TRIANGLES));
			m_pBoxNode = new Geometry(BOX_NAME, m_pBox);
			m_pRoot->attachChild(m_pBoxNode);
		}

		void SimpleScene::update(float elpsedTime)
		{

		}
	}
}
