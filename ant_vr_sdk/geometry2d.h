#pragma once

namespace jet
{
	namespace util
	{
		template<typename Type>
		struct Rectangle2D
		{
			Type X, Y;
			Type Width, Height;

			Rectangle2D():
				X(0), Y(0),
				Width(0), Height(0)
			{}

			Rectangle2D(Type x, Type y, Type w, Type h):
				X(x), Y(y), Width(w), Height(h){}
		};

		typedef Rectangle2D<int> Rectangle2i;
		typedef Rectangle2D<float> Rectangle2f;
		typedef Rectangle2D<unsigned int> Rectangle2ui;
	}
}