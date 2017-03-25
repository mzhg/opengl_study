#pragma once
#include "Node.h"

namespace jet
{
	namespace util
	{
		/**
		* BatchNode holds geometries that are a batched version of all the geometries that are in its sub scenegraph.
		* There is one geometry per different material in the sub tree.
		* The geometries are directly attached to the node in the scene graph.
		* Usage is like any other node except you have to call the {@link #batch()} method once all the geometries have been attached to the sub scene graph and their material set
		* (see todo more automagic for further enhancements)
		* All the geometries that have been batched are set to not be rendered - {@link CullHint} is left intact.
		* The sub geometries can be transformed as usual, their transforms are used to update the mesh of the geometryBatch.
		* Sub geoms can be removed but it may be slower than the normal spatial removing
		* Sub geoms can be added after the batch() method has been called but won't be batched and will just be rendered as normal geometries.
		* To integrate them in the batch you have to call the batch() method again on the batchNode.
		*
		* TODO normal or tangents or both looks a bit weird
		* TODO more automagic (batch when needed in the updateLogicalState)
		* @author Nehon
		*/
		class BatchNode : public Node
		{
		public:
			BatchNode();
			~BatchNode();
		};
	}
}

