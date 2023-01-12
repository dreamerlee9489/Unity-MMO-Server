#ifndef TEAM_H
#define TEAM_H
#include "../../apps/game/world_proxy.h"
#include "player.h"
#include <list>

class Team
{
public:
	Team(uint64 captain) 
	{
		_captain = captain;
		_members.emplace_back(captain); 
	}

	~Team() {}

	void AddMember(uint64 sn) { _members.emplace_back(sn); }

	void RemoveMember(uint64 sn)
	{
		for (auto& iter = _members.begin(); iter != _members.end(); ++iter)
		{
			if (sn == *iter)
			{
				_members.erase(iter);
				break;
			}
		}
	}

	void Clear() { _members.clear(); }

	bool IsCaptain(uint64 sn) { return sn == _captain; }

	void SetCaptain(uint64 sn) { _captain = sn; }

	uint64 GetCaptain() { return _captain; }

	void SetProxy(WorldProxy* proxy) { _proxy = proxy; }

	WorldProxy* GetProxy() { return _proxy; }

private:
	uint64 _captain = 0;
	WorldProxy* _proxy = nullptr;
	std::list<uint64> _members{};
};

#endif // !TEAM_H

