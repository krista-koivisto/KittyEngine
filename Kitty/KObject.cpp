/**
 * Kitty engine
 * KObject.cpp
 *
 * Regular object with a mesh and material. This class extends the
 * generic IObject scene node.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KObject.h"

namespace Kitty
{
	KObject::KObject(KScene *contextScene, KMesh *model)
	{
		context = contextScene;
		mesh = model;
	}
}
