#ifndef SPACE_TEAM_H
#define SPACE_TEAM_H
#include "world.h"
#include <list>

class SpaceTeam
{
public:
	int dungeonId = 0;
	uint64 dungeonSn = 0;

	SpaceTeam() {}

	SpaceTeam(uint64 captain) { _captain = captain; }

	~SpaceTeam() {}

	void AddMember(uint64 sn) { _members.emplace_back(sn); }

	void RemoveMember(uint64 sn)
	{
		_members.remove(sn);
		if (!_members.empty())
			_captain = _members.front();
	}

	void Clear() { _members.clear(); }

	bool IsCaptain(uint64 sn) { return sn == _captain; }

	void SetCaptain(uint64 sn) { _captain = sn; }

	uint64 GetCaptain() { return _captain; }

	std::list<uint64>& GetMembers() { return _members; }

private:
	uint64 _captain = 0;	
	std::list<uint64> _members{};
};

#endif // !SPACE_TEAM_H

