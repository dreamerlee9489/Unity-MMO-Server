#ifndef EDBT_PARALLEL
#define EDBT_PARALLEL
#include "bt_composite.h"

class BtEdParallel : public BtComposite
{
public:
	BtEdParallel(Npc* npc) : BtComposite(npc) {}

	~BtEdParallel() = default;

private:

};

#endif // !EDBT_PARALLEL
