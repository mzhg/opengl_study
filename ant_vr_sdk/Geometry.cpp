#include "Geometry.h"
#include "SpatialManager.h"

namespace jet
{
	namespace util
	{

		Geometry::~Geometry()
		{
		}

		/**
		* Sets the LOD level to use when rendering the mesh of this geometry.
		* Level 0 indicates that the default index buffer should be used,
		* levels [1, LodLevels + 1] represent the levels set on the mesh
		* with {@link Mesh#setLodLevels(com.jme3.scene.VertexBuffer[]) }.
		*
		* @param lod The lod level to set
		*/
		void Geometry::setLodLevel(int lod)
		{
#if 0
			if (m_pMesh->getNumLodLevels() == 0) {
				throw new IllegalStateException("LOD levels are not set on this mesh");
			}

			if (lod < 0 || lod >= mesh.getNumLodLevels()) {
				throw new IllegalArgumentException("LOD level is out of range: " + lod);
			}
#endif

			m_iLodLevel = lod;

			if (isGrouped())
			{
				m_pGroupNode->onMeshChange(this);
			}
		}

		/**
		* Sets the mesh to use for this geometry when rendering.
		*
		* @param mesh the mesh to use for this geometry
		*
		*/
		void Geometry::setMesh(ShapePtr pMesh)
		{
#if 0
			if (mesh == null) {
				throw new IllegalArgumentException();
			}
#endif
			assert(pMesh.get());

			m_pMesh = pMesh;
			setBoundRefresh();

			if (isGrouped())
			{
				m_pGroupNode->onMeshChange(this);
			}
		}

		/**
		* Sets the material to use for this geometry.
		*
		* @param material the material to use for this geometry
		*/
		void Geometry::setMaterial(MaterialPtr pMaterial)
		{
			m_pMaterial = pMaterial;

			if (isGrouped())
			{
				m_pGroupNode->onMaterialChange(this);
			}
		}

		/**
		* <code>updateWorldBound</code> updates the bounding volume that contains
		* this geometry. The location of the geometry is based on the location of
		* all this node's parents.
		*
		* @see Spatial#updateWorldBound()
		*/
		void Geometry::updateWorldBound()
		{
			Spatial::updateWorldBound();
#if 0
			if (m_pMesh == nullptr) {
				throw new NullPointerException("Geometry: " + getName() + " has null mesh");
			}
#endif
			assert(m_pMesh.get());
			m_pMesh->getBound(m_pWorldBound.get());

#if 0
			if (mesh.getBound() != null)
			{
				if (m_bIgnoreTransform) {
					// we do not transform the model bound by the world transform,
					// just use the model bound as-is
					worldBound = mesh.getBound().clone(worldBound);
				}
				else {
					worldBound = mesh.getBound().transform(worldTransform, worldBound);
				}
			}
#endif

			if (!m_bIgnoreTransform)
			{
				m_pWorldBound->transform(m_WorldTransform.getCombinedMatrix());
			}
		}

		void Geometry::updateWorldTransforms()
		{
			Spatial::updateWorldTransforms();
			computeWorldMatrix();

			if (isGrouped()) {
				m_pGroupNode->onTransformChange(this);
			}

			// geometry requires lights to be sorted
			m_pWorldLights->sort(true);
		}

		void Geometry::updateWorldLightList()
		{
			Spatial::updateWorldLightList();
			// geometry requires lights to be sorted
			m_pWorldLights->sort(true);
		}

		/**
		* Associate this <code>Geometry</code> with a {@link SpatialManager}.
		*
		* Should only be called by the parent {@link Node}.
		*
		* @param node Which {@link GeometryGroupNode} to associate with.
		* @param startIndex The starting index of this geometry in the group.
		*/
		void Geometry::associateWithSpatialManager(class SpatialManager* pNode, int iStartIndex)
		{
			if (isGrouped()) {
				unassociateFromGroupNode();
			}

			m_pGroupNode = pNode;
			m_iStartIndex = iStartIndex;
		}

		/**
		* Removes the {@link GeometryGroupNode} association from this
		* <code>Geometry</code>.
		*
		* Should only be called by the parent {@link GeometryGroupNode}.
		*/
		void Geometry::unassociateFromGroupNode()
		{
			if (m_pGroupNode != nullptr)
			{
				// Once the geometry is removed
				// from the parent, the group node needs to be updated.
				m_pGroupNode->onGeometryUnassociated(this);
				m_pGroupNode = nullptr;

				// change the default to -1 to make error detection easier
				m_iStartIndex = -1;
			}
		}

		void Geometry::setParent(Node* pParent)
		{
			Spatial::setParent(pParent);

			// If the geometry is managed by group node we need to unassociate.
			if (pParent == nullptr && isGrouped())
			{
				unassociateFromGroupNode();
			}
		}


		/**
		* Indicate that the transform of this spatial has changed and that
		* a refresh is required.
		*/
		// NOTE: Spatial has an identical implementation of this method,
		// thus it was commented out.
		//    @Override
		//    protected void setTransformRefresh() {
		//        refreshFlags |= RF_TRANSFORM;
		//        setBoundRefresh();
		//    }
		/**
		* Recomputes the matrix returned by {@link Geometry#getWorldMatrix() }.
		* This will require a localized transform update for this geometry.
		*/
		void Geometry::computeWorldMatrix()
		{
			// Force a local update of the geometry's transform
			checkDoTransformUpdate();
#if 0
			// Compute the cached world matrix
			cachedWorldMat.loadIdentity();
			cachedWorldMat.setRotationQuaternion(worldTransform.getRotation());
			cachedWorldMat.setTranslation(worldTransform.getTranslation());

			TempVars vars = TempVars.get();
			Matrix4f scaleMat = vars.tempMat4;
			scaleMat.loadIdentity();
			scaleMat.scale(worldTransform.getScale());
			cachedWorldMat.multLocal(scaleMat);
			vars.release();
#endif
		}
	}
}
