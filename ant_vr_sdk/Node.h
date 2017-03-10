
#pragma once

#include "Spatial.h"

namespace jet
{
	namespace util
	{
	
		/**
		* <code>Node</code> defines an internal node of a scene graph. The internal
		* node maintains a collection of children and handles merging said children
		* into a single bound to allow for very fast culling of multiple nodes. Node
		* allows for any number of children to be attached.
		*/
		class Node : public Spatial
		{
		public:
			friend class SceneManager;
			/**
			* Constructor instantiates a new <code>Node</code> with a default empty
			* list for containing children.
			*
			* @param name the name of the scene element. This is required for
			* identification and comparison purposes.
			*/
			Node(const std::string& name) : Spatial(name)
			{
				// For backwards compatibility, only clear the "requires
				// update" flag if we are not a subclass of Node.
				// This prevents subclass from silently failing to receive
				// updates when they upgrade.
				
				const char* pName = typeid(*this).name();
				setRequiresUpdates(strcmp(pName, "Node") != 0);
			}

			/**
			*
			* <code>getQuantity</code> returns the number of children this node
			* maintains.
			*
			* @return the number of children this node maintains.
			*/
			int getQuantity() const { return m_pChildren.size(); }

			void updateLogicalState(float tpf) override
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

			void updateGeometricState() override
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
			
			int getTriangleCount() override{
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
			 int getVertexCount() override{
				int count = 0;
				for (int i = 0; i < m_pChildren.size(); i++) 
				{
					count += m_pChildren[i]->getVertexCount();
				}

				return count;
			}

			/**
			* <code>attachChild</code> attaches a child to this node. This node
			* becomes the child's parent. The current number of children maintained is
			* returned.
			* <br>
			* If the child already had a parent it is detached from that former parent.
			*
			* @param child
			*            the child to attach to this node.
			* @return the number of children maintained by this node.
			* @throws IllegalArgumentException if child is null.
			*/
			virtual size_t attachChild(Spatial* pChild) {
				return attachChildAt(pChild, m_pChildren.size());
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
			size_t attachChildAt(Spatial* pChild, int index)
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
			int detachChild(Spatial* pChild) {
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
			int detachChildNamed(std::string& childName) 
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
			Spatial* detachChildAt(int index) 
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
			void detachAllChildren() {
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
			int getChildIndex(Spatial* sp) { return Numeric::indexOf(m_pChildren, sp); }

			/**
			* More efficient than e.g detaching and attaching as no updates are needed.
			*
			* @param index1 The index of the first child to swap
			* @param index2 The index of the second child to swap
			*/
			void swapChildren(int index1, int index2) 
			{
#if 0
				Spatial c2 = children.get(index2);
				Spatial c1 = children.remove(index1);
				children.add(index1, c2);
				children.remove(index2);
				children.add(index2, c1);
#endif
			}

			/**
			*
			* <code>getChild</code> returns a child at a given index.
			*
			* @param i
			*            the index to retrieve the child from.
			* @return the child at a specified index.
			*/
			Spatial* getChild(int i) { return m_pChildren[i];}

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
			Spatial* getChild(const std::string& name) 
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
			bool hasChild(Spatial* spat) 
			{
//				if (m_pChildren.contains(spat))
				if (Numeric::indexOf(m_pChildren, spat) >= 0)
					return true;

				for (Spatial* pChild : m_pChildren) {
					Node* pNode = static_cast<Node*>(pChild);
					if (pNode && pNode->hasChild(spat)) 
					{
						return true;
					}
				}

				return false;
			}

			/**
			* Returns all children to this node. Note that modifying that given
			* list is not allowed.
			*
			* @return a list containing all children to this node
			*/
			std::vector<Spatial*>& getChildren() { return m_pChildren; }


			void setMaterial(MaterialPtr mat) override
			{
				for (int i = 0; i < m_pChildren.size(); i++){
					m_pChildren[i]->setMaterial(mat);
				}
			}

			void setLodLevel(int lod) override{
				Spatial::setLodLevel(lod);
				for (Spatial* pChild : m_pChildren) 
				{
					pChild->setLodLevel(lod);
				}
			}

#if 0
			int collideWith(Collidable other, CollisionResults results){
				int total = 0;
				// optimization: try collideWith BoundingVolume to avoid possibly redundant tests on children
				// number 4 in condition is somewhat arbitrary. When there is only one child, the boundingVolume test is redundant at all.
				// The idea is when there are few children, it can be too expensive to test boundingVolume first.
				/*
				I'm removing this change until some issues can be addressed and I really
				think it needs to be implemented a better way anyway.
				First, it causes issues for anyone doing collideWith() with BoundingVolumes
				and expecting it to trickle down to the children.  For example, children
				with BoundingSphere bounding volumes and collideWith(BoundingSphere).  Doing
				a collision check at the parent level then has to do a BoundingSphere to BoundingBox
				collision which isn't resolved.  (Having to come up with a collision point in that
				case is tricky and the first sign that this is the wrong approach.)
				Second, the rippling changes this caused to 'optimize' collideWith() for this
				special use-case are another sign that this approach was a bit dodgy.  The whole
				idea of calculating a full collision just to see if the two shapes collide at all
				is very wasteful.
				A proper implementation should support a simpler boolean check that doesn't do
				all of that calculation.  For example, if 'other' is also a BoundingVolume (ie: 99.9%
				of all non-Ray cases) then a direct BV to BV intersects() test can be done.  So much
				faster.  And if 'other' _is_ a Ray then the BV.intersects(Ray) call can be done.
				I don't have time to do it right now but I'll at least un-break a bunch of peoples'
				code until it can be 'optimized' properly.  Hopefully it's not too late to back out
				the other dodgy ripples this caused.  -pspeed (hindsight-expert ;))
				Note: the code itself is relatively simple to implement but I don't have time to
				a) test it, and b) see if '> 4' is still a decent check for it.  Could be it's fast
				enough to do all the time for > 1.
				if (children.size() > 4)
				{
				BoundingVolume bv = this.getWorldBound();
				if (bv==null) return 0;

				// collideWith without CollisionResults parameter used to avoid allocation when possible
				if (bv.collideWith(other) == 0) return 0;
				}
				*/
				for (Spatial* pChild : m_pChildren){
					total += pChild->collideWith(other, results);
				}
				return total;
			}
#endif
			void setModelBound(BoundingVolumePtr modelBound) override
			{
				for (Spatial* pChild : m_pChildren)
				{
					pChild->setModelBound(modelBound.get() != nullptr ? BoundingVolumePtr(modelBound->clone()) : nullptr);
				}
			}

			virtual void updateModelBound() override
			{
				for (Spatial* pChild : m_pChildren)
				{
					pChild->updateModelBound();
				}
			}

#if 0
			void depthFirstTraversal(SceneGraphVisitor visitor, DFSMode mode) {
				if (mode == DFSMode.POST_ORDER) {
					for (Spatial child : children.getArray()) {
						child.depthFirstTraversal(visitor);
					}
					visitor.visit(this);
				}
				else { //pre order
					visitor.visit(this);
					for (Spatial child : children.getArray()) {
						child.depthFirstTraversal(visitor);
					}
				}
			}
#endif
		protected:

#if 0
			void breadthFirstTraversal(SceneGraphVisitor visitor, Queue<Spatial> queue) {
				queue.addAll(children);
			}
#endif

			
			void setTransformRefresh() override
			{
				Spatial::setTransformRefresh();

				for (Spatial* pChild : m_pChildren)  // TODO
				{
					if ((pChild->m_iRefreshFlags & RF_TRANSFORM) != 0)
						continue;

					pChild->setTransformRefresh();
				}
			}

			
			void setLightListRefresh() override
			{
				Spatial::setLightListRefresh();
				for (Spatial* pChild : m_pChildren){
					if ((pChild->m_iRefreshFlags & RF_LIGHTLIST) != 0)
						continue;

					pChild->setLightListRefresh();
				}
			}

		
			void setMatParamOverrideRefresh()
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

			void updateWorldBound()
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

			void setParent(Node* parent) override
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

		private:

			void addUpdateChildren(std::vector<Spatial*> results)
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
			void invalidateUpdateList() 
			{
				m_bUpdateListValid = false;
				if (m_pParent) 
				{
					m_pParent->invalidateUpdateList();
				}
			}

			std::vector<Spatial*>& getUpdateList()
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
			
		protected:
			// This node's children.
			std::vector<Spatial*> m_pChildren;
			
			// If this node is a root, this list will contain the current
			// set of children (and children of children) that require
			// updateLogicalState() to be called as indicated by their
			// requiresUpdate() method.
			std::vector<Spatial*> m_pUpdateList;

		private:
			/**
			* False if the update list requires rebuilding.  This is Node.class
			* specific and therefore not included as part of the Spatial update flags.
			* A flag is used instead of nulling the updateList to avoid reallocating
			* a whole list every time the scene graph changes.
			*/
			bool m_bUpdateListValid = false;

			class SceneManager* m_pSceneManager;
		};
	}
}

