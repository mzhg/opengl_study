#pragma once
#include "LegacyApplication.h"
#include "Geometry.h"

namespace jet
{
	namespace util
	{
		class SimpleScene : public LegacyApplication
		{
		public:
			SimpleScene();
			~SimpleScene();

		protected:

			void initialize();
			void update(float elpsedTime);

		private:
			Geometry* m_pBoxNode;
			ShapePtr  m_pBox;
		};
	}
}

