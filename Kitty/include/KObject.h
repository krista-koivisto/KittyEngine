/**
 * Kitty engine
 * KObject.h
 *
 * Regular object with a mesh and material. This class extends the
 * generic IObject scene node.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KOBJECT_H
#define KENGINE_KOBJECT_H

#include "IObject.h"

namespace Kitty
{
	class KObject : public IObject
	{
	private:

	public:
		KObject(KScene *contextScene, KMesh *model);
	};
}


#endif //KENGINE_KOBJECT_H
