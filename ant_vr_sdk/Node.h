
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
			Node(const std::string& name);

			/**
			*
			* <code>getQuantity</code> returns the number of children this node
			* maintains.
			*
			* @return the number of children this node maintains.
			*/
			int getQuantity() const { return m_pChildren.size(); }

			void updateLogicalState(float tpf) override;

			void updateGeometricState() override;

			/**
			* <code>getTriangleCount</code> returns the number of triangles contained
			* in all sub-branches of this node that contain geometry.
			*
			* @return the triangle count of this branch.
			*/
			
			int getTriangleCount() override;

			/**
			* <code>getVertexCount</code> returns the number of vertices contained
			* in all sub-branches of this node that contain geometry.
			*
			* @return the vertex count of this branch.
			*/
			int getVertexCount() override;

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
			virtual size_t attachChild(Spatial* pChild) 
			{
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
			size_t attachChildAt(Spatial* pChild, int index);

			/**
			* <code>detachChild</code> removes a given child from the node's list.
			* This child will no longer be maintained.
			*
			* @param child
			*            the child to remove.
			* @return the index the child was at. -1 if the child was not in the list.
			*/
			int detachChild(Spatial* pChild);

			/**
			* <code>detachChild</code> removes a given child from the node's list.
			* This child will no longe be maintained. Only the first child with a
			* matching name is removed.
			*
			* @param childName
			*            the child to remove.
			* @return the index the child was at. -1 if the child was not in the list.
			*/
			int detachChildNamed(std::string& childName);

			/**
			*
			* <code>detachChildAt</code> removes a child at a given index. That child
			* is returned for saving purposes.
			*
			* @param index
			*            the index of the child to be removed.
			* @return the child at the supplied index.
			*/
			Spatial* detachChildAt(int index);

			/**
			*
			* <code>detachAllChildren</code> removes all children attached to this
			* node.
			*/
			void detachAllChildren();

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
			void swapChildren(int index1, int index2);

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
			Spatial* getChild(const std::string& name);
			/**
			* determines if the provided Spatial is contained in the children list of
			* this node.
			*
			* @param spat
			*            the child object to look for.
			* @return true if the object is contained, false otherwise.
			*/
			bool hasChild(Spatial* spat);

			/**
			* Returns all children to this node. Note that modifying that given
			* list is not allowed.
			*
			* @return a list containing all children to this node
			*/
			std::vector<Spatial*>& getChildren() { return m_pChildren; }

			void setMaterial(MaterialPtr mat) override;

			void setLodLevel(int lod) override;

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
			void setModelBound(BoundingVolumePtr modelBound) override;
			virtual void updateModelBound() override;
			virtual bool isBatchNode() const { return false; }

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

			void setSpatialManager(class SpatialManager* pManager);
			void setTransformRefresh() override;
			void setLightListRefresh() override;
			void setMatParamOverrideRefresh() override;

			void updateWorldBound() override;

			void setParent(Node* parent) override;

		private:

			void addUpdateChildren(std::vector<Spatial*> results);

			/**
			*  Called to invalidate the root node's update list.  This is
			*  called whenever a spatial is attached/detached as well as
			*  when a control is added/removed from a Spatial in a way
			*  that would change state.
			*/
			void invalidateUpdateList();
			std::vector<Spatial*>& getUpdateList();
			
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
			class SpatialManager* m_pSpatialManager;
		};
	}
}

