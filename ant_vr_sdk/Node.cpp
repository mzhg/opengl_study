#include "Node.h"

namespace jet
{
	namespace util
	{
		
		/**
		* Constructor instantiates a new <code>Node</code> with a default empty
		* list for containing children.
		*
		* @param name the name of the scene element. This is required for
		* identification and comparison purposes.
		*/
		Node::Node(const std::string& name) : Spatial(name), m_pSpatialManager(nullptr)
		{
			// For backwards compatibility, only clear the "requires
			// update" flag if we are not a subclass of Node.
			// This prevents subclass from silently failing to receive
			// updates when they upgrade.

			const char* pName = typeid(*this).name();
			setRequiresUpdates(strcmp(pName, "Node") != 0);
		}

		void Node::updateLogicalState(float tpf)
		{
			Spatial::updateLogicalState(tpf);

			// Only perform updates on children if we are the
			// root and then only peform updates on children we
			// know to require updates.
			// So if this isn't the root, abort.
			if (m_pParent != nullptr) {
				return;
			}

			for (Spatial* s : getUpdateList()) {
				s->updateLogicalState(tpf);
			}
		}

		void Node::updateGeometricState()
		{
			if (m_iRefreshFlags == 0)
			{
				// This branch has no geometric state that requires updates.
				return;
			}
			if ((m_iRefreshFlags & RF_LIGHTLIST) != 0)
			{
				updateWorldLightList();
			}
			if ((m_iRefreshFlags & RF_TRANSFORM) != 0)
			{
				// combine with parent transforms- same for all spatial
				// subclasses.
				updateWorldTransforms();
			}
			if ((m_iRefreshFlags & RF_MATPARAM_OVERRIDE) != 0)
			{
				updateMatParamOverrides();
			}

			m_iRefreshFlags &= ~RF_CHILD_LIGHTLIST;
			if (!m_pChildren.empty())
			{
				// the important part- make sure child geometric state is refreshed
				// first before updating own world bound. This saves
				// a round-trip later on.
				// NOTE 9/19/09
				// Although it does save a round trip,
				for (Spatial* pChild : m_pChildren) {
					pChild->updateGeometricState();
				}
			}

			if ((m_iRefreshFlags & RF_BOUND) != 0){
				updateWorldBound();
			}

			assert(m_iRefreshFlags == 0);
		}

		/**
		* <code>getTriangleCount</code> returns the number of triangles contained
		* in all sub-branches of this node that contain geometry.
		*
		* @return the triangle count of this branch.
		*/

		int Node::getTriangleCount()
		{
			int count = 0;
			for (int i = 0; i < m_pChildren.size(); i++)
			{
				count += m_pChildren[i]->getTriangleCount();
			}

			return count;
		}

		/**
		* <code>getVertexCount</code> returns the number of vertices contained
		* in all sub-branches of this node that contain geometry.
		*
		* @return the vertex count of this branch.
		*/
		int Node::getVertexCount()
		{
			int count = 0;
			for (int i = 0; i < m_pChildren.size(); i++)
			{
				count += m_pChildren[i]->getVertexCount();
			}

			return count;
		}

		
		/**
		*
		* <code>attachChildAt</code> attaches a child to this node at an index. This node
		* becomes the child's parent. The current number of children maintained is
		* returned.
		* <br>
		* If the child already had a parent it is detached from that former parent.
		*
		* @param child
		*            the child to attach to this node.
		* @return the number of children maintained by this node.
		* @throws NullPointerException if child is null.
		*/
		size_t Node::attachChildAt(Spatial* pChild, int index)
		{
			//				if (child == null)
			//					throw new NullPointerException();
			assert(pChild);

			if (pChild->getParent() != this && pChild != this)
			{
				if (pChild->getParent() != nullptr)
				{
					pChild->getParent()->detachChild(pChild);
				}
				pChild->setParent(this);
				//					m_pChildren.add(index, pChild);
				m_pChildren.insert(m_pChildren.begin() + index, pChild);
				// XXX: Not entirely correct? Forces bound update up the
				// tree stemming from the attached child. Also forces
				// transform update down the tree-
				pChild->setTransformRefresh();
				pChild->setLightListRefresh();
				pChild->setMatParamOverrideRefresh();
#if 0
				if (logger.isLoggable(Level.FINE)) {
					logger.log(Level.FINE, "Child ({0}) attached to this node ({1})",
						new Object[]{child.getName(), getName()});
				}
#endif
				invalidateUpdateList();
			}
			return m_pChildren.size();
		}

		/**
		* <code>detachChild</code> removes a given child from the node's list.
		* This child will no longer be maintained.
		*
		* @param child
		*            the child to remove.
		* @return the index the child was at. -1 if the child was not in the list.
		*/
		int Node::detachChild(Spatial* pChild) 
		{
			//				if (pChild == null)
			//					throw new NullPointerException();
			assert(pChild);

			if (pChild->getParent() == this)
			{
				int index = Numeric::indexOf(m_pChildren, pChild);
				if (index != -1)
				{
					detachChildAt(index);
				}
				return index;
			}

			return -1;
		}

		/**
		* <code>detachChild</code> removes a given child from the node's list.
		* This child will no longe be maintained. Only the first child with a
		* matching name is removed.
		*
		* @param childName
		*            the child to remove.
		* @return the index the child was at. -1 if the child was not in the list.
		*/
		int Node::detachChildNamed(std::string& childName)
		{
			//				if (childName == null)
			//					throw new NullPointerException();


			for (int x = 0, max = m_pChildren.size(); x < max; x++) {
				Spatial* pChild = m_pChildren[x];
				if (childName == pChild->getName())
				{
					detachChildAt(x);
					return x;
				}
			}
			return -1;
		}

		/**
		*
		* <code>detachChildAt</code> removes a child at a given index. That child
		* is returned for saving purposes.
		*
		* @param index
		*            the index of the child to be removed.
		* @return the child at the supplied index.
		*/
		Spatial* Node::detachChildAt(int index)
		{
			Spatial* pChild = m_pChildren[index];
			m_pChildren.erase(m_pChildren.begin() + index);
			if (pChild != nullptr)
			{
				pChild->setParent(nullptr);
				//					logger.log(Level.FINE, "{0}: Child removed.", this.toString());

				// since a child with a bound was detached;
				// our own bound will probably change.
				setBoundRefresh();

				// our world transform no longer influences the child.
				// XXX: Not neccessary? Since child will have transform updated
				// when attached anyway.
				pChild->setTransformRefresh();
				// lights are also inherited from parent
				pChild->setLightListRefresh();
				pChild->setMatParamOverrideRefresh();

				invalidateUpdateList();
			}
			return pChild;
		}

		/**
		*
		* <code>detachAllChildren</code> removes all children attached to this
		* node.
		*/
		void Node::detachAllChildren() 
		{
			// Note: this could be a bit more efficient if it delegated
			// to a private method that avoided setBoundRefresh(), etc.
			// for every child and instead did one in here at the end.
			for (int i = m_pChildren.size() - 1; i >= 0; i--) {
				detachChildAt(i);
			}
			//				logger.log(Level.FINE, "{0}: All children removed.", this.toString());
		}

		/**
		* <code>getChildIndex</code> returns the index of the given spatial
		* in this node's list of children.
		* @param sp
		*          The spatial to look up
		* @return
		*          The index of the spatial in the node's children, or -1
		*          if the spatial is not attached to this node
		*/
		int Node::getChildIndex(Spatial* sp) { return Numeric::indexOf(m_pChildren, sp); }

		/**
		* More efficient than e.g detaching and attaching as no updates are needed.
		*
		* @param index1 The index of the first child to swap
		* @param index2 The index of the second child to swap
		*/
		void Node::swapChildren(int index1, int index2)
		{
#if 0
			Spatial c2 = children.get(index2);
			Spatial c1 = children.remove(index1);
			children.add(index1, c2);
			children.remove(index2);
			children.add(index2, c1);
#endif
		}

		/*
		*
		* <code>getChild</code> returns a child at a given index.
		*
		* @param i
		*            the index to retrieve the child from.
		* @return the child at a specified index.
		*/
//		Spatial* Node::getChild(int i) { return m_pChildren[i]; }

		/**
		* <code>getChild</code> returns the first child found with exactly the
		* given name (case sensitive.) This method does a depth first recursive
		* search of all descendants of this node, it will return the first spatial
		* found with a matching name.
		*
		* @param name
		*            the name of the child to retrieve. If null, we'll return null.
		* @return the child if found, or null.
		*/
		Spatial* Node::getChild(const std::string& name)
		{
			for (Spatial* pChild : m_pChildren)
			{
				if (name == pChild->getName())
				{
					return pChild;
				}

				Node* pNode = static_cast<Node*>(pChild);
				if (pNode) {
					Spatial* out = pNode->getChild(name);
					if (out != nullptr)
					{
						return out;
					}
				}
			}
			return nullptr;
		}
		/**
		* determines if the provided Spatial is contained in the children list of
		* this node.
		*
		* @param spat
		*            the child object to look for.
		* @return true if the object is contained, false otherwise.
		*/
		bool Node::hasChild(Spatial* spat)
		{
			//				if (m_pChildren.contains(spat))
			if (Numeric::indexOf(m_pChildren, spat) >= 0)
				return true;

			for (Spatial* pChild : m_pChildren) 
			{
				Node* pNode = static_cast<Node*>(pChild);
				if (pNode && pNode->hasChild(spat))
				{
					return true;
				}
			}

			return false;
		}

		void Node::setSpatialManager(class SpatialManager* pManager)
		{
			m_pSpatialManager = pManager;
		}
		
//		std::vector<Spatial*>& Node::getChildren() { return m_pChildren; }

		void Node::setMaterial(MaterialPtr mat) 
		{
			for (int i = 0; i < m_pChildren.size(); i++){
				m_pChildren[i]->setMaterial(mat);
			}
		}

		void Node::setLodLevel(int lod) 
		{
			Spatial::setLodLevel(lod);
			for (Spatial* pChild : m_pChildren)
			{
				pChild->setLodLevel(lod);
			}
		}

		void Node::setModelBound(BoundingVolumePtr modelBound)
		{
			for (Spatial* pChild : m_pChildren)
			{
				pChild->setModelBound(modelBound.get() != nullptr ? BoundingVolumePtr(modelBound->clone()) : nullptr);
			}
		}

		void Node::updateModelBound()
		{
			for (Spatial* pChild : m_pChildren)
			{
				pChild->updateModelBound();
			}
		}

		void Node::setTransformRefresh()
		{
			Spatial::setTransformRefresh();

			for (Spatial* pChild : m_pChildren)  // TODO
			{
				if ((pChild->m_iRefreshFlags & RF_TRANSFORM) != 0)
					continue;

				pChild->setTransformRefresh();
			}
		}


		void Node::setLightListRefresh()
		{
			Spatial::setLightListRefresh();
			for (Spatial* pChild : m_pChildren){
				if ((pChild->m_iRefreshFlags & RF_LIGHTLIST) != 0)
					continue;

				pChild->setLightListRefresh();
			}
		}


		void Node::setMatParamOverrideRefresh()
		{
			Spatial::setMatParamOverrideRefresh();
			for (Spatial* pChild : m_pChildren)
			{
				if ((pChild->m_iRefreshFlags & RF_MATPARAM_OVERRIDE) != 0)
				{
					continue;
				}

				pChild->setMatParamOverrideRefresh();
			}
		}

		void Node::updateWorldBound()
		{
			Spatial::updateWorldBound();
#if 0
			// for a node, the world bound is a combination of all it's children
			// bounds
			BoundingVolume* pResultBound = nullptr;
			for (Spatial* pChild : m_pChildren)
			{
				// child bound is assumed to be updated
				assert(pChild->m_iRefreshFlags & RF_BOUND) == 0;
				if (pResultBound)
				{
					// merge current world bound with child world bound
					pResultBound->mergeLocal(pChild->getWorldBound());
				}
				else
				{
					// set world bound to first non-null child world bound
					if (pChild->getWorldBound())
					{
						resultBound = child.getWorldBound().clone(this.worldBound);
					}
				}
			}
			this.worldBound = resultBound;
#endif
		}

		void Node::setParent(Node* parent)
		{
			if (this->m_pParent == nullptr && parent != nullptr)
			{
				// We were a root before and now we aren't... make sure if
				// we had an updateList then we clear it completely to
				// avoid holding the dead array.
				//					updateList = null;
				m_pUpdateList.clear();
				m_bUpdateListValid = false;
			}
			Spatial::setParent(parent);
		}

//	private:

		void Node::addUpdateChildren(std::vector<Spatial*> results)
		{
			for (Spatial* pChild : m_pChildren)
			{
				if (pChild->requiresUpdates())
				{
					results.push_back(pChild);
				}
				/*
				if (child instanceof Node) {
				((Node)child).addUpdateChildren(results);
				}
				*/

				Node* pNode = dynamic_cast<Node*>(pChild);
				if (pNode)
				{
					pNode->addUpdateChildren(results);
				}
			}
		}

		/**
		*  Called to invalidate the root node's update list.  This is
		*  called whenever a spatial is attached/detached as well as
		*  when a control is added/removed from a Spatial in a way
		*  that would change state.
		*/
		void Node::invalidateUpdateList()
		{
			m_bUpdateListValid = false;
			if (m_pParent)
			{
				m_pParent->invalidateUpdateList();
			}
		}

		std::vector<Spatial*>& Node::getUpdateList()
		{
			if (m_bUpdateListValid)
			{
				return m_pUpdateList;
			}

			m_pUpdateList.clear();

			// Build the list
			addUpdateChildren(m_pUpdateList);
			m_bUpdateListValid = true;
			return m_pUpdateList;
		}
	}
}
