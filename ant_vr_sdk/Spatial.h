#pragma once
#include "BoundingVolume.h"
#include "LightList.h"
#include "Camera.h"
#include "RenderQueue.h"
#include <map>
#include <algorithm>
#include "Util.h"
#include "Material.h"

namespace jet
{
	namespace util
	{

		/**
		* An interface for scene-graph controls.
		* <p>
		* <code>Control</code>s are used to specify certain update and render logic
		* for a {@link Spatial}.
		*
		*/
		class Control
		{
		public:
			/**
			* Creates a clone of the Control, the given Spatial is the cloned
			* version of the spatial to which this control is attached to.
			* @param spatial
			* @return A clone of this control for the spatial
			*/
			virtual std::shared_ptr<Control> cloneForSpatial(class Spatial* pSpatial) = 0;

			/**
			* @param spatial the spatial to be controlled. This should not be called
			* from user code.
			*/
			virtual void setSpatial(class Spatial* pSpatial) = 0;

			/**
			* Updates the control. This should not be called from user code.
			* @param tpf Time per frame.
			*/
			virtual void update(float tpf) = 0;

			/**
			* Should be called prior to queuing the spatial by the RenderManager. This
			* should not be called from user code.
			*
			* @param rm
			* @param vp
			*/
			virtual void render(class RenderManager* pRM, class ViewPort& vp) = 0;
		};

		typedef std::shared_ptr<Control> ControlPtr;

		/**
		* Specifies how frustum culling should be handled by
		* this spatial.
		*/
		enum class CullHint 
		{

			/**
			* Do whatever our parent does. If no parent, default to {@link #Dynamic}.
			*/
			INHERIT,
			/**
			* Do not draw if we are not at least partially within the view frustum
			* of the camera. This is determined via the defined
			* Camera planes whether or not this Spatial should be culled.
			*/
			DYNAMIC,
			/**
			* Always cull this from the view, throwing away this object
			* and any children from rendering commands.
			*/
			ALWAYS,
			/**
			* Never cull this from view, always draw it.
			* Note that we will still get culled if our parent is culled.
			*/
			NEVER
		};

		/**
		* Specifies if this spatial should be batched
		*/
		enum class BatchHint 
		{
			/**
			* Do whatever our parent does. If no parent, default to {@link #Always}.
			*/
			INHERIT,
			/**
			* This spatial will always be batched when attached to a BatchNode.
			*/
			ALWAYS,
			/**
			* This spatial will never be batched when attached to a BatchNode.
			*/
			NEVER
		};

		class Spatial
		{
		public:
			friend class Node;

			/**
			* (Internal use only) Forces a refresh of the given types of data.
			*
			* @param transforms Refresh world transform based on parents'
			* @param bounds Refresh bounding volume data based on child nodes
			* @param lights Refresh light list based on parents'
			*/
			void forceRefresh(bool transforms, bool bounds, bool lights);

			/**
			* <code>checkCulling</code> checks the spatial with the camera to see if it
			* should be culled.
			* <p>
			* This method is called by the renderer. Usually it should not be called
			* directly.
			*
			* @param pCam The camera to check against.
			* @return true if inside or intersecting camera frustum
			* (should be rendered), false if outside.
			*/
			virtual bool checkCulling(Camera* pCam);

			/**
			* Sets the name of this spatial.
			*
			* @param name
			*            The spatial's new name.
			*/
			void setName(const std::string& name) { m_strName = name; }
			void setName(const char* pName)       { m_strName = pName; }

			/**
			* Returns the name of this spatial.
			*
			* @return This spatial's name.
			*/
			const std::string& getName() const{ return m_strName; }

			/**
			* Returns the local {@link LightList}, which are the lights
			* that were directly attached to this <code>Spatial</code> through the
			* {@link #addLight(com.jme3.light.Light) } and
			* {@link #removeLight(com.jme3.light.Light) } methods.
			*
			* @return The local light list
			*/
			LightListPtr getLocalLightList() { return m_pLocalLights;}

			/**
			* Returns the world {@link LightList}, containing the lights
			* combined from all this <code>Spatial's</code> parents up to and including
			* this <code>Spatial</code>'s lights.
			*
			* @return The combined world light list
			*/
			LightListPtr getWorldLightList() {return m_pWorldLights;}

#if 0
			/**
			* Get the local material parameter overrides.
			*
			* @return The list of local material parameter overrides.
			*/
			public SafeArrayList<MatParamOverride> getLocalMatParamOverrides() {
				return localOverrides;
			}

			/**
			* Get the world material parameter overrides.
			*
			* Note that this list is only updated on a call to
			* {@link #updateGeometricState()}. After update, the world overrides list
			* will contain the {@link #getParent() parent's} world overrides combined
			* with this spatial's {@link #getLocalMatParamOverrides() local overrides}.
			*
			* @return The list of world material parameter overrides.
			*/
			public SafeArrayList<MatParamOverride> getWorldMatParamOverrides() {
				return worldOverrides;
			}

#endif
			/**
			* <code>getWorldRotation</code> retrieves the absolute rotation of the
			* Spatial.
			*
			* @return the Spatial's world rotation quaternion.
			*/
			const glm::quat& getWorldRotation() 
			{
				checkDoTransformUpdate();
				return m_WorldTransform.getRotate();
			}

			/**
			* <code>getWorldTranslation</code> retrieves the absolute translation of
			* the spatial.
			*
			* @return the Spatial's world translation vector.
			*/
			const glm::vec3& getWorldTranslation() 
			{
				checkDoTransformUpdate();
				return m_WorldTransform.getTranslate();
			}

			/**
			* <code>getWorldScale</code> retrieves the absolute scale factor of the
			* spatial.
			*
			* @return the Spatial's world scale factor.
			*/
			const glm::vec3& getWorldScale() 
			{
				checkDoTransformUpdate();
				return m_WorldTransform.getScale();
			}

			/**
			* <code>getWorldTransform</code> retrieves the world transformation
			* of the spatial.
			*
			* @return the world transform.
			*/
			const Transform& getWorldTransform() 
			{
				checkDoTransformUpdate();
				return m_WorldTransform;
			}

			/**
			* <code>rotateUpTo</code> is a utility function that alters the
			* local rotation to point the Y axis in the direction given by newUp.
			*
			* @param newUp
			*            the up vector to use - assumed to be a unit vector.
			*/
			void rotateUpTo(const glm::vec3& newUp);

			/**
			* <code>lookAt</code> is a convenience method for auto-setting the local
			* rotation based on a position in world space and an up vector. It computes the rotation
			* to transform the z-axis to point onto 'position' and the y-axis to 'up'.
			* Unlike {@link Quaternion#lookAt(com.jme3.math.Vector3f, com.jme3.math.Vector3f) }
			* this method takes a world position to look at and not a relative direction.
			*
			* Note : 28/01/2013 this method has been fixed as it was not taking into account the parent rotation.
			* This was resulting in improper rotation when the spatial had rotated parent nodes.
			* This method is intended to work in world space, so no matter what parent graph the
			* spatial has, it will look at the given position in world space.
			*
			* @param position
			*            where to look at in terms of world coordinates
			* @param upVector
			*            a vector indicating the (local) up direction. (typically {0,
			*            1, 0} in jME.)
			*/
			void lookAt(const glm::vec3& f3Position, const glm::vec3& f3Up = glm::vec3(0, 1, 0));
#if 0
			/**
			* Adds a local material parameter override.
			*
			* @param override The override to add.
			* @see MatParamOverride
			*/
			void addMatParamOverride(MatParamOverride override) {
				if (override == null) {
					throw new IllegalArgumentException("override cannot be null");
				}
				localOverrides.add(override);
				setMatParamOverrideRefresh();
			}

			/**
			* Remove a local material parameter override if it exists.
			*
			* @param override The override to remove.
			* @see MatParamOverride
			*/
			void removeMatParamOverride(MatParamOverride override) {
				if (localOverrides.remove(override)) {
					setMatParamOverrideRefresh();
				}
			}

			/**
			* Remove all local material parameter overrides.
			*
			* @see #addMatParamOverride(com.jme3.material.MatParamOverride)
			*/
			void clearMatParamOverrides() {
				if (!localOverrides.isEmpty()) {
					setMatParamOverrideRefresh();
				}
				localOverrides.clear();
			}

			/**
			* Called when the Spatial is about to be rendered, to notify
			* controls attached to this Spatial using the Control.render() method.
			*
			* @param rm The RenderManager rendering the Spatial.
			* @param vp The ViewPort to which the Spatial is being rendered to.
			*
			* @see Spatial#addControl(com.jme3.scene.control.Control)
			* @see Spatial#getControl(java.lang.Class)
			*/
			void runControlRender(RenderManager rm, ViewPort vp) {
				if (controls.isEmpty()) {
					return;
				}

				for (Control c : controls.getArray()) {
					c.render(rm, vp);
				}
			}

			/**
			* Add a control to the list of controls.
			* @param control The control to add.
			*
			* @see Spatial#removeControl(java.lang.Class)
			*/
			public void addControl(Control control) {
				boolean before = requiresUpdates();
				controls.add(control);
				control.setSpatial(this);
				boolean after = requiresUpdates();
				// If the requirement to be updated has changed
				// then we need to let the parent node know so it
				// can rebuild its update list.
				if (parent != null && before != after) {
					parent.invalidateUpdateList();
				}
			}

			/**
			* Removes the first control that is an instance of the given class.
			*
			* @see Spatial#addControl(com.jme3.scene.control.Control)
			*/
			public void removeControl(Class< ? extends Control> controlType) {
				boolean before = requiresUpdates();
				for (int i = 0; i < controls.size(); i++) {
					if (controlType.isAssignableFrom(controls.get(i).getClass())) {
						Control control = controls.remove(i);
						control.setSpatial(null);
						break; // added to match the javadoc  -pspeed
					}
				}
				boolean after = requiresUpdates();
				// If the requirement to be updated has changed
				// then we need to let the parent node know so it
				// can rebuild its update list.
				if (parent != null && before != after) {
					parent.invalidateUpdateList();
				}
			}

			/**
			* Removes the given control from this spatial's controls.
			*
			* @param control The control to remove
			* @return True if the control was successfully removed. False if the
			* control is not assigned to this spatial.
			*
			* @see Spatial#addControl(com.jme3.scene.control.Control)
			*/
			public boolean removeControl(Control control) {
				boolean before = requiresUpdates();
				boolean result = controls.remove(control);
				if (result) {
					control.setSpatial(null);
				}

				boolean after = requiresUpdates();
				// If the requirement to be updated has changed
				// then we need to let the parent node know so it
				// can rebuild its update list.
				if (parent != null && before != after) {
					parent.invalidateUpdateList();
				}
				return result;
			}

			/**
			* Returns the first control that is an instance of the given class,
			* or null if no such control exists.
			*
			* @param controlType The superclass of the control to look for.
			* @return The first instance in the list of the controlType class, or null.
			*
			* @see Spatial#addControl(com.jme3.scene.control.Control)
			*/
			public <T extends Control> T getControl(Class<T> controlType) {
				for (Control c : controls.getArray()) {
					if (controlType.isAssignableFrom(c.getClass())) {
						return (T)c;
					}
				}
				return null;
			}
#endif
			/**
			* Returns the control at the given index in the list.
			*
			* @param index The index of the control in the list to find.
			* @return The control at the given index.
			*
			* @throws IndexOutOfBoundsException
			*      If the index is outside the range [0, getNumControls()-1]
			*
			* @see Spatial#addControl(com.jme3.scene.control.Control)
			*/
			ControlPtr getControl(int index) 
			{
				return m_pControls.at(index);
			}

			/**
			* @return The number of controls attached to this Spatial.
			* @see Spatial#addControl(com.jme3.scene.control.Control)
			* @see Spatial#removeControl(java.lang.Class)
			*/
			uint32_t getNumControls() 
			{
				return (uint32_t)m_pControls.size();
			}

			/**
			* <code>updateLogicalState</code> calls the <code>update()</code> method
			* for all controls attached to this Spatial.
			*
			* @param tpf Time per frame.
			*
			* @see Spatial#addControl(com.jme3.scene.control.Control)
			*/
			virtual void updateLogicalState(float tpf) 
			{
				runControlUpdate(tpf);
			}

			/**
			* <code>updateGeometricState</code> updates the lightlist,
			* computes the world transforms, and computes the world bounds
			* for this Spatial.
			* Calling this when the Spatial is attached to a node
			* will cause undefined results. User code should only call this
			* method on Spatials having no parent.
			*
			* @see Spatial#getWorldLightList()
			* @see Spatial#getWorldTransform()
			* @see Spatial#getWorldBound()
			*/
			virtual void updateGeometricState();

			/**
			* Convert a vector (in) from this spatial's local coordinate space to world
			* coordinate space.
			*
			* @param in
			*            vector to read from
			* @param store
			*            where to write the result (null to create a new vector, may be
			*            same as in)
			* @return the result (store)
			*/
			glm::vec3 localToWorld(const glm::vec3& in) 
			{
				checkDoTransformUpdate();
//				return worldTransform.transformVector(in, store);
				return Transform::transform(m_WorldTransform , in);
			}

			/**
			* Convert a vector (in) from world coordinate space to this spatial's local
			* coordinate space.
			*
			* @param in
			*            vector to read from
			* @param store
			*            where to write the result
			* @return the result (store)
			*/
			glm::vec3 worldToLocal(const glm::vec3& in) 
			{
				checkDoTransformUpdate();
//				return worldTransform.transformInverseVector(in, store);
				return Transform::transformInverse(m_WorldTransform, in);
			}

			/**
			* <code>getParent</code> retrieves this node's parent. If the parent is
			* null this is the root node.
			*
			* @return the parent of this node.
			*/
			Node* getParent() {	return m_pParent;}

			/**
			* <code>removeFromParent</code> removes this Spatial from it's parent.
			*
			* @return true if it has a parent and performed the remove.
			*/
			virtual bool removeFromParent();

			/**
			* determines if the provided Node is the parent, or parent's parent, etc. of this Spatial.
			*
			* @param ancestor
			*            the ancestor object to look for.
			* @return true if the ancestor is found, false otherwise.
			*/
			bool hasAncestor(Node* pAncestor);

			virtual bool equals(const Spatial* pOther)
			{
				return this == pOther;
			}

			/**
			* <code>getLocalRotation</code> retrieves the local rotation of this
			* node.
			*
			* @return the local rotation of this node.
			*/
			const glm::quat& getLocalRotation() const {	return m_LocalTransform.getRotate();}

			/**
			* <code>setLocalRotation</code> sets the local rotation of this node
			* by using a {@link Matrix3f}.
			*
			* @param rotation
			*            the new local rotation.
			*/
			void setLocalRotation(const glm::mat3& rotation) 
			{
//				localTransform.getRotation().fromRotationMatrix(rotation);
				m_LocalTransform.setRotateFromMatrix(rotation);
				setTransformRefresh();
			}

			/**
			* <code>setLocalRotation</code> sets the local rotation of this node.
			*
			* @param quaternion
			*            the new local rotation.
			*/
			void setLocalRotation(const glm::quat& quaternion) 
			{
				m_LocalTransform.setRotate(quaternion);
				setTransformRefresh();
			}

			/**
			* <code>getLocalScale</code> retrieves the local scale of this node.
			*
			* @return the local scale of this node.
			*/
			const glm::vec3& getLocalScale() { return m_LocalTransform.getScale(); }

			/**
			* <code>setLocalScale</code> sets the local scale of this node.
			*
			* @param localScale
			*            the new local scale, applied to x, y and z
			*/
			void setLocalScale(float fLocalScale) {
				m_LocalTransform.setScale(glm::vec3(fLocalScale));
				setTransformRefresh();
			}

			/**
			* <code>setLocalScale</code> sets the local scale of this node.
			*/
			void setLocalScale(float x, float y, float z) {
				m_LocalTransform.setScale(x, y, z);
				setTransformRefresh();
			}

			/**
			* <code>setLocalScale</code> sets the local scale of this node.
			*
			* @param localScale
			*            the new local scale.
			*/
			void setLocalScale(const glm::vec4  f3LocalScale) {
				m_LocalTransform.setScale(f3LocalScale);
				setTransformRefresh();
			}

			/**
			* <code>getLocalTranslation</code> retrieves the local translation of
			* this node.
			*
			* @return the local translation of this node.
			*/
			const glm::vec3& getLocalTranslation() { return m_LocalTransform.getTranslate();}

			/**
			* <code>setLocalTranslation</code> sets the local translation of this
			* spatial.
			*
			* @param localTranslation
			*            the local translation of this spatial.
			*/
			void setLocalTranslation(const glm::vec3& f3LocalTranslation) 
			{
				m_LocalTransform.setTranslate(f3LocalTranslation);
				setTransformRefresh();
			}

			/**
			* <code>setLocalTranslation</code> sets the local translation of this
			* spatial.
			*/
			void setLocalTranslation(float x, float y, float z) 
			{
				m_LocalTransform.setTranslate(x, y, z);
				setTransformRefresh();
			}

			/**
			* <code>setLocalTransform</code> sets the local transform of this
			* spatial.
			*/
			void setLocalTransform(Transform t) 
			{
				m_LocalTransform = t;
				setTransformRefresh();
			}

			/**
			* <code>getLocalTransform</code> retrieves the local transform of
			* this spatial.
			*
			* @return the local transform of this spatial.
			*/
			const Transform& getLocalTransform() { return m_LocalTransform; }

			/**
			* Applies the given material to the Spatial, this will propagate the
			* material down to the geometries in the scene graph.
			*
			* @param material The material to set.
			*/
			virtual void setMaterial(MaterialPtr material) {}
#if 0
			/**
			* <code>addLight</code> adds the given light to the Spatial; causing
			all
			* child Spatials to be affected by it.
			*
			* @param light The light to add.
			*/
			public void addLight(Light light) {
				localLights.add(light);
				setLightListRefresh();
			}

			/**
			* <code>removeLight</code> removes the given light from the Spatial.
			*
			* @param light The light to remove.
			* @see Spatial#addLight(com.jme3.light.Light)
			*/
			public void removeLight(Light light) {
				localLights.remove(light);
				setLightListRefresh();
			}
#endif
			/**
			* Translates the spatial by the given translation vector.
			*
			* @return The spatial on which this method is called, e.g <code>this</code>.
			*/
			Spatial* move(float x, float y, float z) 
			{
				m_LocalTransform.move(x,y,z);
				setTransformRefresh();

				return this;
			}

			/**
			* Translates the spatial by the given translation vector.
			*
			* @return The spatial on which this method is called, e.g <code>this</code>.
			*/
			Spatial* move(const glm::vec3& f3Offset) 
			{
				m_LocalTransform.move(f3Offset);
				setTransformRefresh();

				return this;
			}

			/**
			* Scales the spatial by the given value
			*
			* @return The spatial on which this method is called, e.g <code>this</code>.
			*/
			Spatial* scale(float s) {  return scale(s, s, s);}

			/**
			* Scales the spatial by the given scale vector.
			*
			* @return The spatial on which this method is called, e.g <code>this</code>.
			*/
			Spatial* scale(float x, float y, float z) 
			{
				m_LocalTransform.scale(x, y, z);
				setTransformRefresh();

				return this;
			}

			/**
			* Rotates the spatial by the given rotation.
			*
			* @return The spatial on which this method is called, e.g <code>this</code>.
			*/
			Spatial* rotate(const glm::quat& rot) 
			{
				m_LocalTransform.rotate(rot);
				setTransformRefresh();

				return this;
			}

			/**
			* Rotates the spatial by the xAngle, yAngle and zAngle angles (in radians),
			* (aka pitch, yaw, roll) in the local coordinate space.
			*
			* @return The spatial on which this method is called, e.g <code>this</code>.
			*/
			Spatial* rotate(float xAngle, float yAngle, float zAngle) 
			{
//				TempVars vars = TempVars.get();
//				Quaternion q = vars.quat1;
//				q.fromAngles(xAngle, yAngle, zAngle);

				glm::quat q(glm::vec3(xAngle, yAngle, zAngle));
				rotate(q);
//				vars.release();

				return this;
			}

			/**
			* Centers the spatial in the origin of the world bound.
			* @return The spatial on which this method is called, e.g <code>this</code>.
			*/
			Spatial* center() 
			{
				glm::vec3 worldTrans = getWorldTranslation();
				glm::vec3 worldCenter;

#if 1
				worldCenter = glm::vec3(0);
#else
				worldCenter = getWorldBound()->getCenter();
#endif

				glm::vec3 absTrans = worldTrans - worldCenter;
				setLocalTranslation(absTrans);

				return this;
			}

			/**
			* @see #setCullHint(CullHint)
			* @return the cull mode of this spatial, or if set to CullHint.Inherit, the
			* cull mode of its parent.
			*/
			CullHint getCullHint() const;

			BatchHint getBatchHint() const;

			/**
			* Returns this spatial's renderqueue bucket. If the mode is set to inherit,
			* then the spatial gets its renderqueue bucket from its parent.
			*
			* @return The spatial's current renderqueue mode.
			*/
			Bucket getQueueBucket() const;

			/**
			* @return The shadow mode of this spatial, if the local shadow
			* mode is set to inherit, then the parent's shadow mode is returned.
			*
			* @see Spatial#setShadowMode(com.jme3.renderer.queue.RenderQueue.ShadowMode)
			* @see ShadowMode
			*/
			ShadowMode getShadowMode() const;

			/**
			* Sets the level of detail to use when rendering this Spatial,
			* this call propagates to all geometries under this Spatial.
			*
			* @param lod The lod level to set.
			*/
			virtual void setLodLevel(int lod) {}

			/**
			* <code>updateModelBound</code> recalculates the bounding object for this
			* Spatial.
			*/
			virtual void updateModelBound() = 0;

			/**
			* <code>setModelBound</code> sets the bounding object for this Spatial.
			*
			* @param modelBound
			*            the bounding object for this spatial.
			*/
			virtual void setModelBound(BoundingVolumePtr modelBound) = 0;

			/**
			* @return The sum of all vertices under this Spatial.
			*/
			virtual int getVertexCount() = 0;

			/**
			* @return The sum of all triangles under this Spatial.
			*/
			virtual int getTriangleCount() { return 0; };

			/**
			* <code>getWorldBound</code> retrieves the world bound at this node
			* level.
			*
			* @return the world bound at this level.
			*/
			BoundingVolumePtr getWorldBound() {
				checkDoBoundUpdate();
				return m_pWorldBound;
			}

			/**
			* <code>setCullHint</code> alters how view frustum culling will treat this
			* spatial.
			*
			* @param hint one of: <code>CullHint.Dynamic</code>,
			* <code>CullHint.Always</code>, <code>CullHint.Inherit</code>, or
			* <code>CullHint.Never</code>
			* <p>
			* The effect of the default value (CullHint.Inherit) may change if the
			* spatial gets re-parented.
			*/
			void setCullHint(CullHint hint) {	m_CullHint = hint;}

			/**
			* <code>setBatchHint</code> alters how batching will treat this spatial.
			*
			* @param hint one of: <code>BatchHint.Never</code>,
			* <code>BatchHint.Always</code>, or <code>BatchHint.Inherit</code>
			* <p>
			* The effect of the default value (BatchHint.Inherit) may change if the
			* spatial gets re-parented.
			*/
			void setBatchHint(BatchHint hint) {	m_BatchHint = hint;}

			/**
			* @return the cullmode set on this Spatial
			*/
			CullHint getLocalCullHint() const { return m_CullHint; }

			/**
			* @return the batchHint set on this Spatial
			*/
			BatchHint getLocalBatchHint() const { return m_BatchHint;}

			/**
			* <code>setQueueBucket</code> determines at what phase of the
			* rendering process this Spatial will rendered. See the
			* {@link Bucket} enum for an explanation of the various
			* render queue buckets.
			*
			* @param queueBucket
			*            The bucket to use for this Spatial.
			*/
			void setQueueBucket(Bucket queueBucket) { m_QueueBucket = queueBucket;}

			/**
			* Sets the shadow mode of the spatial
			* The shadow mode determines how the spatial should be shadowed,
			* when a shadowing technique is used. See the
			* documentation for the class {@link ShadowMode} for more information.
			*
			* @see ShadowMode
			*
			* @param shadowMode The local shadow mode to set.
			*/
			void setShadowMode(ShadowMode shadowMode) { m_ShadowMode = shadowMode;}

			/**
			* @return The locally set queue bucket mode
			*
			* @see Spatial#setQueueBucket(com.jme3.renderer.queue.RenderQueue.Bucket)
			*/
			Bucket getLocalQueueBucket() const {	return m_QueueBucket;}

			/**
			* @return The locally set shadow mode
			*
			* @see Spatial#setShadowMode(com.jme3.renderer.queue.RenderQueue.ShadowMode)
			*/
			ShadowMode getLocalShadowMode() const { return m_ShadowMode; }

			/**
			* Returns this spatial's last frustum intersection result. This int is set
			* when a check is made to determine if the bounds of the object fall inside
			* a camera's frustum. If a parent is found to fall outside the frustum, the
			* value for this spatial will not be updated.
			*
			* @return The spatial's last frustum intersection result.
			*/
			FrustumIntersect getLastFrustumIntersection() const { return m_FrustumeIntersects;}

			/**
			* Overrides the last intersection result. This is useful for operations
			* that want to start rendering at the middle of a scene tree and don't want
			* the parent of that node to influence culling.
			*
			* @param intersects
			*            the new value
			*/
			void setLastFrustumIntersection(FrustumIntersect intersects) {	m_FrustumeIntersects = intersects;}

#if 0
			/**
			* Returns the Spatial's name followed by the class of the spatial <br>
			* Example: "MyNode (com.jme3.scene.Spatial)
			*
			* @return Spatial's name followed by the class of the Spatial
			*/
			
			public String toString() {
				return name + " (" + this.getClass().getSimpleName() + ')';
			}
#endif
			/**
			* Creates a transform matrix that will convert from this spatials'
			* local coordinate space to the world coordinate space
			* based on the world transform.
			*
			* @param store Matrix where to store the result, if null, a new one
			* will be created and returned.
			*
			* @return store if not null, otherwise, a new matrix containing the result.
			*
			* @see Spatial#getWorldTransform()
			*/
			const glm::mat4& getLocalToWorldMatrix() {
#if 0
				if (store == null) {
					store = new Matrix4f();
				}
				else {
					store.loadIdentity();
				}
				// multiply with scale first, then rotate, finally translate (cf.
				// Eberly)
				store.scale(getWorldScale());
				store.multLocal(getWorldRotation());
				store.setTranslation(getWorldTranslation());
#endif
				return m_WorldTransform.getCombinedMatrix();
			}

#if 0
			/**
			* Visit each scene graph element ordered by DFS with the default post order mode.
			* @param visitor
			* @see #depthFirstTraversal(com.jme3.scene.SceneGraphVisitor, com.jme3.scene.Spatial.DFSMode)
			*/
			void depthFirstTraversal(SceneGraphVisitor visitor) {
				depthFirstTraversal(visitor, DFSMode.POST_ORDER);
			}

			/**
			* Specifies the mode of the depth first search.
			*/
			enum class DFSMode {
				/**
				* Pre order: the current spatial is visited first, then its children.
				*/
				PRE_ORDER,
				/**
				* Post order: the children are visited first, then the parent.
				*/
				POST_ORDER;
			}

			/**
			* Visit each scene graph element ordered by DFS.
			* There are two modes: pre order and post order.
			* @param visitor
			* @param mode the traversal mode: pre order or post order
			*/
			virtual void depthFirstTraversal(SceneGraphVisitor visitor, DFSMode mode) = 0;

			/**
			* Visit each scene graph element ordered by BFS
			* @param visitor
			*/
			void breadthFirstTraversal(SceneGraphVisitor visitor) {
				Queue<Spatial> queue = new LinkedList<Spatial>();
				queue.add(this);

				while (!queue.isEmpty()) {
					Spatial s = queue.poll();
					visitor.visit(s);
					s.breadthFirstTraversal(visitor, queue);
				}
			}

			virtual void breadthFirstTraversal(SceneGraphVisitor visitor, Queue<Spatial> queue) = 0;
#endif

			virtual ~Spatial();

		protected:
//			Spatial() : Spatial(""){}

			Spatial(const std::string& name);

			/**
			* Returns true if this spatial requires updateLogicalState() to
			* be called, either because setRequiresUpdate(true) has been called
			* or because the spatial has controls.  This is package private to
			* avoid exposing it to the public API since it is only used by Node.
			*/
			bool requiresUpdates() { return m_bRequiresUpdates | !m_pControls.empty();}
			/**
			* Subclasses can call this with true to denote that they require
			* updateLogicalState() to be called even if they contain no controls.
			* Setting this to false reverts to the default behavior of only
			* updating if the spatial has controls.  This is not meant to
			* indicate dynamic state in any way and must be called while
			* unattached or an IllegalStateException is thrown.  It is designed
			* to be called during object construction and then never changed, ie:
			* it's meant to be subclass specific state and not runtime state.
			* Subclasses of Node or Geometry that do not set this will get the
			* old default behavior as if this was set to true.  Subclasses should
			* call setRequiresUpdate(false) in their constructors to receive
			* optimal behavior if they don't require updateLogicalState() to be
			* called even if there are no controls.
			*/
			void setRequiresUpdates(bool f) 
			{
				// Note to explorers, the reason this was done as a protected setter
				// instead of passed on construction is because it frees all subclasses
				// from having to make sure to always pass the value up in case they
				// are subclassed.
				// The reason that requiresUpdates() isn't just a protected method to
				// override (which would be more correct) is because the flag provides
				// some flexibility in how we break subclasses.  A protected method
				// would require that all subclasses that required updates need implement
				// this method or they would silently stop processing updates.  A flag
				// lets us set a default when a subclass is detected that is different
				// than the internal "more efficient" default.
				// Spatial's default is 'true' for this flag requiring subclasses to
				// override it for more optimal behavior.  Node and Geometry will override
				// it to false if the class is Node.class or Geometry.class.
				// This means that all subclasses will default to the old behavior
				// unless they opt in.
//				if (parent != null) {
//					throw new IllegalStateException("setRequiresUpdates() cannot be called once attached.");
//				}

				assert(m_pParent == nullptr);
				m_bRequiresUpdates = f;
			}

			/**
			* Indicate that the transform of this spatial has changed and that
			* a refresh is required.
			*/
			virtual void setTransformRefresh() 
			{
				m_iRefreshFlags |= RF_TRANSFORM;
				setBoundRefresh();
			}

			virtual void setLightListRefresh();

			virtual void setMatParamOverrideRefresh();

			/**
			* Indicate that the bounding of this spatial has changed and that
			* a refresh is required.
			*/
			void setBoundRefresh();

			/**
			* Should be overridden by Node and Geometry.
			*/
			virtual void updateWorldBound() 
			{
				// the world bound of a leaf is the same as it's model bound
				// for a node, the world bound is a combination of all it's children
				// bounds
				// -> handled by subclass
				m_iRefreshFlags &= ~RF_BOUND;
			}

			virtual void updateWorldLightList() 
			{
#if 0
				if (m_pParent) 
				{
					m_pWorldLights.update(m_pLocalLights, nullptr);
					m_iRefreshFlags &= ~RF_LIGHTLIST;
				}
				else 
				{
					assert((m_pParent->m_iRefreshFlags & RF_LIGHTLIST) == 0);
					m_pWorldLights.update(m_pLocalLights, m_pParent->m_pWorldLights);
					m_iRefreshFlags &= ~RF_LIGHTLIST;
				}
#endif
			}

			void updateMatParamOverrides() 
			{
				m_iRefreshFlags &= ~RF_MATPARAM_OVERRIDE;

#if 0
				worldOverrides.clear();
				if (parent == null) {
					worldOverrides.addAll(localOverrides);
				}
				else {
					assert(parent.refreshFlags & RF_MATPARAM_OVERRIDE) == 0;
					worldOverrides.addAll(parent.worldOverrides);
					worldOverrides.addAll(localOverrides);
				}
#endif
			}

			/**
			* Should only be called from updateGeometricState().
			* In most cases should not be subclassed.
			*/
			virtual void updateWorldTransforms();

			/**
			* Computes the world transform of this Spatial in the most
			* efficient manner possible.
			*/
			void checkDoTransformUpdate();

			/**
			* Computes this Spatial's world bounding volume in the most efficient
			* manner possible.
			*/
			void checkDoBoundUpdate();

			/**
			* Called by {@link Node#attachChild(Spatial)} and
			* {@link Node#detachChild(Spatial)} - don't call directly.
			* <code>setParent</code> sets the parent of this node.
			*
			* @param parent
			*            the parent of this node.
			*/
			virtual void setParent(Node* pParent) { m_pParent = pParent; }

		private:
			void runControlUpdate(float tpf) {
				if (m_pControls.empty()) {
					return;
				}
#if 0
				for (Control c : controls.getArray()) {
					c.update(tpf);
				}
#endif

				std::vector<ControlPtr> arr(m_pControls);
				for (auto it = arr.begin(); it != arr.end(); it++)
				{
					it->get()->update(tpf);
				}
			}

		public:
			float m_fQueueDistance;
		protected:
			/**
			* Refresh flag types
			*/
			static const int RF_TRANSFORM = 0x01;  // need light resort + combine transforms
			static const int RF_BOUND = 0x02;
			static const int RF_LIGHTLIST = 0x04; // changes in light lists
			static const int RF_CHILD_LIGHTLIST = 0x08; //some child need geometry update
			static const int RF_MATPARAM_OVERRIDE = 0x10;

			std::string m_strName;

			CullHint m_CullHint;
			BatchHint m_BatchHint;

			BoundingVolumePtr m_pWorldBound;

			LightListPtr m_pLocalLights;
			LightListPtr m_pWorldLights;

			FrustumIntersect m_FrustumeIntersects;
			Bucket           m_QueueBucket;
			ShadowMode       m_ShadowMode;
			
			Transform        m_LocalTransform;
			Transform        m_WorldTransform;

			std::vector<ControlPtr>      m_pControls;
			std::map<std::string, void*> m_pUserData;
			// Spatial's parent, or null if it has none.
			class Node*            m_pParent;

			/**
			* Refresh flags. Indicate what data of the spatial need to be
			* updated to reflect the correct state.
			*/
			int m_iRefreshFlags;

			/**
			* Set to true if a subclass requires updateLogicalState() even
			* if it doesn't have any controls.  Defaults to true thus implementing
			* the legacy behavior for any subclasses not specifically turning it
			* off.
			* This flag should be set during construction and never changed
			* as it's supposed to be class-specific and not runtime state.
			*/
			bool m_bRequiresUpdates;

		private:

			enum class AttachState
			{
				NONE, 
				ATTACH,
				HOST,
				DEATTACH
			};

			// When a Spatial created, the value initlized with NONE.
			// When it attacehd to a Node, the value became ATTACH.
			// When it parsed by SceneManager, the value become HOST.
			// When it dettacehd from a Node, the value became DEATTACH.
			// When it parsed by SceneManager with DEATTACH state, the value became NONE
			AttachState m_AttachState;
		};
	}
}

