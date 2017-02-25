#pragma once
#include "Spatial.h"
#include "Shape3D.h"
#include "GeometryGroupNode.h"

namespace jet
{
	namespace util
	{
		/**
		* <code>Geometry</code> defines a leaf node of the scene graph. The leaf node
		* contains the geometric data for rendering objects. It manages all rendering
		* information such as a {@link Material} object to define how the surface
		* should be shaded and the {@link Mesh} data to contain the actual geometry.
		*/
		class Geometry : public Spatial
		{
		public:
			/**
			* Create a geometry node with mesh data.
			* The material of the geometry is null, it cannot
			* be rendered until it is set.
			*
			* @param name The name of this geometry
			* @param mesh The mesh data for this geometry
			*/
			Geometry(const std::string& name, ShapePtr pMesh) : Spatial(name)
			{
				// For backwards compatibility, only clear the "requires
				// update" flag if we are not a subclass of Geometry.
				// This prevents subclass from silently failing to receive
				// updates when they upgrade.
				setRequiresUpdates(strcmp(typeid(this).name(), "Geometry") != 0);
				m_pMesh = pMesh;
			}

			bool checkCulling(Camera* pCam) override
			{
				if (isGrouped()) 
				{
					setLastFrustumIntersection(FrustumIntersect::OUTSIDE);
					return false;
				}
				return Spatial::checkCulling(pCam);
			}

			/**
			* @return If ignoreTransform mode is set.
			*
			* @see Geometry#setIgnoreTransform(boolean)
			*/
			bool isIgnoreTransform() const { 	return m_bIgnoreTransform;}

			/**
			* @param ignoreTransform If true, the geometry's transform will not be applied.
			*/
			void setIgnoreTransform(bool ignoreTransform) {	m_bIgnoreTransform = ignoreTransform;}

			/**
			* Sets the LOD level to use when rendering the mesh of this geometry.
			* Level 0 indicates that the default index buffer should be used,
			* levels [1, LodLevels + 1] represent the levels set on the mesh
			* with {@link Mesh#setLodLevels(com.jme3.scene.VertexBuffer[]) }.
			*
			* @param lod The lod level to set
			*/
			void setLodLevel(int lod) 
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

				if (isGrouped()) {
					m_pGroupNode->onMeshChange(this);
				}
			}

			/**
			* Returns the LOD level set with {@link #setLodLevel(int) }.
			*
			* @return the LOD level set
			*/
			int getLodLevel() const { return m_iLodLevel; }

			/**
			* Returns this geometry's mesh vertex count.
			*
			* @return this geometry's mesh vertex count.
			*
			* @see Mesh#getVertexCount()
			*/
			int getVertexCount() { return m_pMesh->getVertexCount();}

			/**
			* Returns this geometry's mesh triangle count.
			*
			* @return this geometry's mesh triangle count.
			*
			* @see Mesh#getTriangleCount()
			*/
			int getTriangleCount() { return m_pMesh->getTriangleCount();}

			/**
			* Sets the mesh to use for this geometry when rendering.
			*
			* @param mesh the mesh to use for this geometry
			*
			*/
			void setMesh(ShapePtr pMesh)
			{
#if 0
				if (mesh == null) {
					throw new IllegalArgumentException();
				}
#endif
				assert(pMesh.get());
				
				m_pMesh = pMesh;
				setBoundRefresh();

				if (isGrouped()) {
					m_pGroupNode->onMeshChange(this);
				}
			}

			/**
			* Returns the mesh to use for this geometry
			*
			* @return the mesh to use for this geometry
			*
			* @see #setMesh(com.jme3.scene.Mesh)
			*/
			ShapePtr getMesh() { return m_pMesh; }

			/**
			* Sets the material to use for this geometry.
			*
			* @param material the material to use for this geometry
			*/
			void setMaterial(MaterialPtr pMaterial) override
			{
				m_pMaterial = pMaterial;

				if (isGrouped()) {
					m_pGroupNode->onMaterialChange(this);
				}
			}

			/**
			* Returns the material that is used for this geometry.
			*
			* @return the material that is used for this geometry
			*
			* @see #setMaterial(com.jme3.material.Material)
			*/
			MaterialPtr getMaterial() const {return m_pMaterial;}

			/**
			* @return The bounding volume of the mesh, in model space.
			*/
			const BoundingVolumePtr getModelBound() { return m_pWorldBound; }

			/**
			* Updates the bounding volume of the mesh. Should be called when the
			* mesh has been modified.
			*/
			void updateModelBound() {
//				m_pMesh->updateBound();  TODO
				setBoundRefresh();
			}

			/**
			* <code>updateWorldBound</code> updates the bounding volume that contains
			* this geometry. The location of the geometry is based on the location of
			* all this node's parents.
			*
			* @see Spatial#updateWorldBound()
			*/
			void updateWorldBound() override
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

			void updateWorldTransforms() override
			{
				Spatial::updateWorldTransforms();
				computeWorldMatrix();

				if (isGrouped()) {
					m_pGroupNode->onTransformChange(this);
				}

				// geometry requires lights to be sorted
				m_pWorldLights->sort(true);
			}

			void updateWorldLightList() override
			{
				Spatial::updateWorldLightList();
				// geometry requires lights to be sorted
				m_pWorldLights->sort(true);
			}

			/**
			* Associate this <code>Geometry</code> with a {@link GeometryGroupNode}.
			*
			* Should only be called by the parent {@link GeometryGroupNode}.
			*
			* @param node Which {@link GeometryGroupNode} to associate with.
			* @param startIndex The starting index of this geometry in the group.
			*/
			void associateWithGroupNode(GeometryGroupNode* pNode, int iStartIndex) 
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
		    void unassociateFromGroupNode() 
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

			void setParent(Node* pParent) override 
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
			void computeWorldMatrix() 
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

			/**
			* A {@link Matrix4f matrix} that transforms the {@link Geometry#getMesh() mesh}
			* from model space to world space. This matrix is computed based on the
			* {@link Geometry#getWorldTransform() world transform} of this geometry.
			* In order to receive updated values, you must call {@link Geometry#computeWorldMatrix() }
			* before using this method.
			*
			* @return Matrix to transform from local space to world space
			*/
			const glm::mat4& getWorldMatrix() { return m_WorldTransform.getCombinedMatrix(); }

			/**
			* Sets the model bound to use for this geometry.
			* This alters the bound used on the mesh as well via
			* {@link Mesh#setBound(com.jme3.bounding.BoundingVolume) } and
			* forces the world bounding volume to be recomputed.
			*
			* @param modelBound The model bound to set
			*/
			void setModelBound(BoundingVolumePtr modelBound) 
			{
#if 0
				this.worldBound = null;
				mesh.setBound(modelBound);
#endif
				m_pWorldBound = modelBound;
				setBoundRefresh();

				// NOTE: Calling updateModelBound() would cause the mesh
				// to recompute the bound based on the geometry thus making
				// this call useless!
				//updateModelBound();
			}
#if 0
			public int collideWith(Collidable other, CollisionResults results) {
				// Force bound to update
				checkDoBoundUpdate();
				// Update transform, and compute cached world matrix
				computeWorldMatrix();

				assert(refreshFlags & (RF_BOUND | RF_TRANSFORM)) == 0;

				if (mesh != null) {
					// NOTE: BIHTree in mesh already checks collision with the
					// mesh's bound
					int prevSize = results.size();
					int added = mesh.collideWith(other, cachedWorldMat, worldBound, results);
					int newSize = results.size();
					for (int i = prevSize; i < newSize; i++) {
						results.getCollisionDirect(i).setGeometry(this);
					}
					return added;
				}
				return 0;
			}

			@Override
				public void depthFirstTraversal(SceneGraphVisitor visitor, DFSMode mode) {
				visitor.visit(this);
			}

			@Override
				protected void breadthFirstTraversal(SceneGraphVisitor visitor, Queue<Spatial> queue) {
			}
#endif
			/**
			* Determine whether this <code>Geometry</code> is managed by a
			* {@link GeometryGroupNode} or not.
			*
			* @return True if managed by a {@link GeometryGroupNode}.
			*/
			bool isGrouped() const { return m_pGroupNode != nullptr;}

			~Geometry();

		protected:
			ShapePtr m_pMesh;
			int      m_iLodLevel;
			MaterialPtr  m_pMaterial;

			bool     m_bIgnoreTransform;
			GeometryGroupNode* m_pGroupNode;
			int      m_iStartIndex;


		};
	}
}

