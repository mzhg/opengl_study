#pragma once
#include "Node.h"

namespace jet
{
	namespace util
	{
		class GeometryGroupNode : public Node
		{
		public:
			GeometryGroupNode(const std::string& name) : Node(name){}

			/**
			* Called by {@link Geometry geom} to specify that its world transform
			* has been changed.
			*
			* @param geom The Geometry whose transform changed.
			*/
			virtual void onTransformChange(class Geometry* pGeom) = 0;

			/**
			* Called by {@link Geometry geom} to specify that its
			* {@link Geometry#setMaterial(com.jme3.material.Material) material}
			* has been changed.
			*
			* @param geom The Geometry whose material changed.
			*
			* @throws UnsupportedOperationException If this implementation does
			* not support dynamic material changes.
			*/
			virtual void onMaterialChange(class Geometry * pGeom) = 0;

			/**
			* Called by {@link Geometry geom} to specify that its
			* {@link Geometry#setMesh(com.jme3.scene.Mesh) mesh}
			* has been changed.
			*
			* This is also called when the geometry's
			* {@link Geometry#setLodLevel(int) lod level} changes.
			*
			* @param geom The Geometry whose mesh changed.
			*
			* @throws UnsupportedOperationException If this implementation does
			* not support dynamic mesh changes.
			*/
			virtual void onMeshChange(class Geometry * pGeom) = 0;

			/**
			* Called by {@link Geometry geom} to specify that it
			* has been unassociated from its <code>GeoemtryGroupNode</code>.
			*
			* Unassociation occurs when the {@link Geometry} is
			* {@link Spatial#removeFromParent() detached} from its parent
			* {@link Node}.
			*
			* @param geom The Geometry which is being unassociated.
			*/
			virtual void onGeometryUnassociated(class Geometry * pGeom) = 0;

			virtual ~GeometryGroupNode();
		};
	}
}

