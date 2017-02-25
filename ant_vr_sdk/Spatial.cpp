#include "Spatial.h"
#include "Node.h"

namespace jet
{
	namespace util
	{
		
		Spatial::~Spatial()
		{
		}

		Spatial::Spatial(const std::string& name) : m_strName(name), m_pParent(nullptr), m_iRefreshFlags(0), m_bRequiresUpdates(false)
		{
		}

		void Spatial::forceRefresh(bool transforms, bool bounds, bool lights)
		{
			if (transforms) {
				setTransformRefresh();
			}
			if (bounds) {
				setBoundRefresh();
			}
			if (lights) {
				setLightListRefresh();
			}
		}

		bool Spatial::checkCulling(Camera* pCam)
		{
#if 0
			if (m_iRefreshFlags != 0) {

				throw new IllegalStateException("Scene graph is not properly updated for rendering.\n"
					+ "State was changed after rootNode.updateGeometricState() call. \n"
					+ "Make sure you do not modify the scene from another thread!\n"
					+ "Problem spatial name: " + getName());
			}
#endif
			assert(m_iRefreshFlags == 0);
			return true;

#if 0
			CullHint cm = getCullHint();
			assert(cm != CullHint::INHERIT);

			if (cm == CullHint::ALWAYS)
			{
				setLastFrustumIntersection(FrustumIntersect::OUTSIDE);
				return false;
			}
			else if (cm == CullHint::NEVER)
			{
				setLastFrustumIntersection(FrustumIntersect::INTERSECTS);
				return true;
			}

			// check to see if we can cull this node
			m_FrustumeIntersects = (m_pParent ? m_pParent->m_FrustumeIntersects
				: FrustumIntersect::INTERSECTS);

			if (m_FrustumeIntersects == FrustumIntersect::INTERSECTS)
			{
				if (getQueueBucket() == Bucket::GUI)
				{
					return cam.containsGui(getWorldBound());
				}
				else
				{
					m_FrustumeIntersects = cam.contains(getWorldBound());
				}
			}

			return m_FrustumeIntersects != FrustumIntersect::OUTSIDE;
#endif
		}

		void Spatial::rotateUpTo(const glm::vec3& newUp) 
		{

			//				Vector3f compVecA = vars.vect1;
			//				Quaternion q = vars.quat1;
			glm::quat q;

			// First figure out the current up vector.
			//				Vector3f upY = compVecA.set(Vector3f.UNIT_Y);
			glm::vec3 upY(0.0f, 1.0f, 0.0f);
			glm::quat rot = m_LocalTransform.getRotate();
			//				rot.multLocal(upY);
			upY = rot * upY;

			// get angle between vectors
			//				float angle = upY.angleBetween(newUp);
			float angle = glm::acos(glm::dot(upY, newUp));

			// figure out rotation axis by taking cross product
			//				Vector3f rotAxis = upY.crossLocal(newUp).normalizeLocal();
			glm::vec3 rotAxis = glm::cross(upY, newUp);
			glm::normalize(rotAxis);

			// Build a rotation quat and apply current local rotation.
			//				q.fromAngleNormalAxis(angle, rotAxis);
			//				q.mult(rot, rot);
			q = glm::angleAxis(angle, rotAxis);
			rot = q * rot;
			m_LocalTransform.setRotate(rot);

			//				vars.release();

			if (m_LocalTransform.isDirty())
			{
				setTransformRefresh();
			}
		}

		void Spatial::updateGeometricState()
		{
			// assume that this Spatial is a leaf, a proper implementation
			// for this method should be provided by Node.

			// NOTE: Update world transforms first because
			// bound transform depends on them.
			if ((m_iRefreshFlags & RF_LIGHTLIST) != 0)
			{
				updateWorldLightList();
			}
			if ((m_iRefreshFlags & RF_TRANSFORM) != 0)
			{
				updateWorldTransforms();
			}
			if ((m_iRefreshFlags & RF_BOUND) != 0)
			{
				updateWorldBound();
			}
			if ((m_iRefreshFlags & RF_MATPARAM_OVERRIDE) != 0)
			{
				updateMatParamOverrides();
			}

			//				assert refreshFlags == 0;
			assert(m_iRefreshFlags == 0);
		}

		bool Spatial::hasAncestor(Node* pAncestor)
		{
			if (!m_pParent)
			{
				return false;
			}
			else if (m_pParent->equals(pAncestor))
			{
				return true;
			}
			else
			{
				return m_pParent->hasAncestor(pAncestor);
			}
		}

		CullHint Spatial::getCullHint() const
		{
			if (m_CullHint != CullHint::INHERIT)
			{
				return m_CullHint;
			}
			else if (m_pParent)
			{
				return m_pParent->getCullHint();
			}
			else
			{
				return CullHint::DYNAMIC;
			}
		}

		BatchHint Spatial::getBatchHint() const
		{
			if (m_BatchHint != BatchHint::INHERIT)
			{
				return m_BatchHint;
			}
			else if (m_pParent)
			{
				return m_pParent->getBatchHint();
			}
			else
			{
				return BatchHint::ALWAYS;
			}
		}

		Bucket Spatial::getQueueBucket() const
		{
			if (m_QueueBucket != Bucket::INHERIT)
			{
				return m_QueueBucket;
			}
			else if (m_pParent)
			{
				return m_pParent->getQueueBucket();
			}
			else
			{
				return Bucket::OPAQUEA;
			}
		}

		ShadowMode Spatial::getShadowMode() const
		{
			if (m_ShadowMode != ShadowMode::INHERIT)
			{
				return m_ShadowMode;
			}
			else if (m_pParent)
			{
				return m_pParent->getShadowMode();
			}
			else
			{
				return ShadowMode::OFF;
			}
		}

		void Spatial::setLightListRefresh()
		{
			m_iRefreshFlags |= RF_LIGHTLIST;
			// Make sure next updateGeometricState() visits this branch
			// to update lights.
			Spatial* p = m_pParent;
			while (p) {
				if ((p->m_iRefreshFlags & RF_CHILD_LIGHTLIST) != 0)
				{
					// The parent already has this flag,
					// so must all ancestors.
					return;
				}
				p->m_iRefreshFlags |= RF_CHILD_LIGHTLIST;
				p = p->m_pParent;
			}
		}

		void Spatial::setMatParamOverrideRefresh()
		{
			m_iRefreshFlags |= RF_MATPARAM_OVERRIDE;
			Spatial* p = m_pParent;
			while (p != nullptr)
			{
				if ((p->m_iRefreshFlags & RF_MATPARAM_OVERRIDE) != 0)
				{
					return;
				}

				p->m_iRefreshFlags |= RF_MATPARAM_OVERRIDE;
				p = p->m_pParent;
			}
		}

		/**
		* Indicate that the bounding of this spatial has changed and that
		* a refresh is required.
		*/
		void Spatial::setBoundRefresh()
		{
			m_iRefreshFlags |= RF_BOUND;

			Spatial* p = m_pParent;
			while (p != nullptr)
			{
				if ((p->m_iRefreshFlags & RF_BOUND) != 0)
				{
					return;
				}

				p->m_iRefreshFlags |= RF_BOUND;
				p = p->m_pParent;
			}
		}

		void Spatial::updateWorldTransforms()
		{

			if (m_pParent == nullptr) 
			{
				m_WorldTransform = m_LocalTransform;
				m_iRefreshFlags &= ~RF_TRANSFORM;
			}
			else
			{
				// check if transform for parent is updated
				assert((m_pParent->m_iRefreshFlags & RF_TRANSFORM) == 0);
				m_WorldTransform = m_LocalTransform;
				m_WorldTransform.combineWithParent(m_pParent->m_WorldTransform);
				m_iRefreshFlags &= ~RF_TRANSFORM;
			}
		}

		/**
		* Computes the world transform of this Spatial in the most
		* efficient manner possible.
		*/
		void Spatial::checkDoTransformUpdate()
		{
			if ((m_iRefreshFlags & RF_TRANSFORM) == 0)
			{
				return;
			}

			if (m_pParent == nullptr)
			{
				m_WorldTransform = m_LocalTransform;
				m_iRefreshFlags &= ~RF_TRANSFORM;
			}
			else
			{
				//					TempVars vars = TempVars.get();

				//					Spatial[] stack = vars.spatialStack;
				std::vector<Spatial*> stack;
				Spatial* rootNode = this;
				int i = 0;
				while (true)
				{
					Spatial* hisParent = rootNode->m_pParent;
					if (hisParent == nullptr)
					{
						rootNode->m_WorldTransform = rootNode->m_LocalTransform;
						rootNode->m_iRefreshFlags &= ~RF_TRANSFORM;
						i--;
						break;
					}

					stack.push_back(rootNode);

					if ((hisParent->m_iRefreshFlags & RF_TRANSFORM) == 0)
					{
						break;
					}

					rootNode = hisParent;
					i++;
				}

				//					vars.release();

				for (int j = i; j >= 0; j--)
				{
					rootNode = stack[j];
					//rootNode.worldTransform.set(rootNode.localTransform);
					//rootNode.worldTransform.combineWithParent(rootNode.parent.worldTransform);
					//rootNode.refreshFlags &= ~RF_TRANSFORM;
					rootNode->updateWorldTransforms();
				}
			}
		}

		/**
		* Computes this Spatial's world bounding volume in the most efficient
		* manner possible.
		*/
		void Spatial::checkDoBoundUpdate()
		{
			if ((m_iRefreshFlags & RF_BOUND) == 0)
			{
				return;
			}

			checkDoTransformUpdate();

			// Go to children recursively and update their bound
			if (dynamic_cast<Node*>(this))
			{
				Node* node = (Node*) this;
				int len = node->getQuantity();
				for (int i = 0; i < len; i++) {
					Spatial* child = node->getChild(i);
					child->checkDoBoundUpdate();
				}
			}

			// All children's bounds have been updated. Update my own now.
			updateWorldBound();
		}

		bool Spatial::removeFromParent()
		{
			if (m_pParent) {
				m_pParent->detachChild(this);
				return true;
			}
			return false;
		}
	}
}
