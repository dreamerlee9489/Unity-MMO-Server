#ifndef TEAM_H
#define TEAM_H
#include "world_proxy.h"
#include "world_component_teleport.h"
#include "libplayer/player.h"
#include <list>

class Team
{
public:
	int dungeonId = 0;
	uint64 dungeonSn = 0;

	Team() {}

	Team(uint64 captain) { _captain = captain; }

	~Team() {}

	void AddMember(uint64 sn) { _members.emplace_back(sn); }

	void RemoveMember(uint64 sn)
	{
		for (auto iter = _members.begin(); iter != _members.end(); ++iter)
		{
			if (sn == *iter)
			{
				_members.erase(iter);
				break;
			}
		}
		if (_members.size() >= 1)
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

#endif // !TEAM_H

