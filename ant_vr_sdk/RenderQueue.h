#pragma once

#include <vector>
#include <string>
#include "Transform.h"
#include <memory>

namespace jet
{
	namespace util
	{
		/**
		* The render queue <code>Bucket</code> specifies the bucket
		* to which the spatial will be placed when rendered.
		* <p>
		* The behavior of the rendering will differ depending on which
		* bucket the spatial is placed. A spatial's queue bucket can be set
		* via {@link Spatial#setQueueBucket(com.jme3.renderer.queue.RenderQueue.Bucket) }.
		*/
		enum class Bucket {
			/**
			* The renderer will try to find the optimal order for rendering all
			* objects using this mode.
			* You should use this mode for most normal objects, except transparent
			* ones, as it could give a nice performance boost to your application.
			*/
			OPAQUEA,

			/**
			* This is the mode you should use for object with
			* transparency in them. It will ensure the objects furthest away are
			* rendered first. That ensures when another transparent object is drawn on
			* top of previously drawn objects, you can see those (and the object drawn
			* using Opaque) through the transparent parts of the newly drawn
			* object.
			*/
			TRANSPARENT,

			/**
			* A special mode used for rendering really far away, flat objects -
			* e.g. skies. In this mode, the depth is set to infinity so
			* spatials in this bucket will appear behind everything, the downside
			* to this bucket is that 3D objects will not be rendered correctly
			* due to lack of depth testing.
			*/
			SKY,

			/**
			* A special mode used for rendering transparent objects that
			* should not be effected by {@link SceneProcessor}.
			* Generally this would contain translucent objects, and
			* also objects that do not write to the depth buffer such as
			* particle emitters.
			*/
			TRANSLUCENT,

			/**
			* This is a special mode, for drawing 2D object
			* without perspective (such as GUI or HUD parts).
			* The spatial's world coordinate system has the range
			* of [0, 0, -1] to [Width, Height, 1] where Width/Height is
			* the resolution of the screen rendered to. Any spatials
			* outside of that range are culled.
			*/
			GUI,

			/**
			* A special mode, that will ensure that this spatial uses the same
			* mode as the parent Node does.
			*/
			INHERIT,
		};

		/**
		* <code>ShadowMode</code> is a marker used to specify how shadow
		* effects should treat the spatial.
		*/
		enum class ShadowMode {
			/**
			* Disable both shadow casting and shadow receiving for this spatial.
			* Generally used for special effects like particle emitters.
			*/
			OFF,

			/**
			* Enable casting of shadows but not receiving them.
			*/
			CAST,

			/**
			* Enable receiving of shadows but not casting them.
			*/
			RECEIVE,

			/**
			* Enable both receiving and casting of shadows.
			*/
			CASTANDRECEIVE,

			/**
			* Inherit the <code>ShadowMode</code> from the parent node.
			*/
			INHERIT
		};

		class RenderQueue
		{
		public:
			RenderQueue();
			~RenderQueue();

		protected:
			std::vector<GeometryAssembly*> m_CastShadowObjs;
			std::vector<GeometryAssembly*> m_OpaqueObjsWithDepth;
			std::vector<GeometryAssembly*> m_OpaqueObjsWithoutDepth;
			std::vector<GeometryAssembly*> m_TranslucentObjsWithDepth;
			std::vector<GeometryAssembly*> m_TranslucentObjsWithoutDepth;
		};
	}
}

