#include "game/halley_statics.h"
#include <halley/entity/type_deleter.h>
#include <halley/data_structures/vector.h>
#include <halley/entity/family_mask.h>
#include <halley/os/os.h>
#include <halley/concurrency/executor.h>
#include <thread>

using namespace Halley;

namespace Halley {
	class HalleyStaticsPimpl
	{
	public:
		HalleyStaticsPimpl()
		{
			maskStorage = MaskStorageInterface::createMaskStorage();
			os = OS::createOS();

			executors = std::make_unique<Executors>();
			executors->set(*executors);
		}

		Vector<TypeDeleterBase*> typeDeleters;
		void* maskStorage;
		OS* os;
		
		std::unique_ptr<Executors> executors;
		std::unique_ptr<ThreadPool> threadPool;
	};
}

HalleyStatics::HalleyStatics()
	: pimpl(std::make_unique<HalleyStaticsPimpl>())
{
}

HalleyStatics::~HalleyStatics()
{
	suspend();
}

void HalleyStatics::setup()
{
	Executors::set(*pimpl->executors);
	pimpl->threadPool = std::make_unique<ThreadPool>(pimpl->executors->getCPU(), std::thread::hardware_concurrency());

	ComponentDeleterTable::getDeleters() = &pimpl->typeDeleters;
	MaskStorageInterface::setMaskStorage(pimpl->maskStorage);
	OS::setInstance(pimpl->os);
}

void HalleyStatics::suspend()
{
	pimpl->threadPool.reset();
}
