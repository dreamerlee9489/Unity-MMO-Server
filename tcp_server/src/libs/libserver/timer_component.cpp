#include "timer_component.h"
#include <utility>
#include "global.h"
#include "log4_help.h"
#include "update_component.h"

struct CompareTimer
{
    constexpr bool operator()(const Timer& _Left, const Timer& _Right) const
    {
        return (_Left.NextTime > _Right.NextTime);
    }
};

void TimerComponent::Add(Timer& data)
{
    //LOG_DEBUG("add time call." << data.Handler.target_type().name());

    _heap.emplace_back(data);
    if (_heap.size() == 1)
    {
        make_heap(_heap.begin(), _heap.end(), CompareTimer());
    }
    else
    {
        push_heap(_heap.begin(), _heap.end(), CompareTimer());
    }
}

void TimerComponent::Awake()
{
    // update
    AddComponent<UpdateComponent>(BindFunP0(this, &TimerComponent::Update));
}

void TimerComponent::BackToPool()
{
    _heap.clear();
}

uint64 TimerComponent::Add(const int total, const int durations, const bool immediateDo, const int immediateDoDelaySecond, TimerHandleFunction handler)
{
    // durations ִ�м����
    // immediateDo �Ƿ�����ִ��
    // immediateDoDelaySecond �״�ִ���뵱ǰʱ��ļ��ʱ��

    Timer data;
    data.SN = Global::GetInstance()->GenerateSN();
    data.CallCountCur = 0;
    data.CallCountTotal = total;
    data.DurationSecond = durations;
    data.Handler = std::move(handler);
    data.NextTime = timeutil::AddSeconds(Global::GetInstance()->TimeTick, durations);

    if (immediateDo)
    {
        data.NextTime = timeutil::AddSeconds(Global::GetInstance()->TimeTick, immediateDoDelaySecond);
    }

    Add(data);
    return data.SN;
}

void TimerComponent::Remove(std::list<uint64>& timers)
{
    for (auto sn : timers)
    {
        auto iter = std::find_if(_heap.begin(), _heap.end(), [sn](const Timer& one)
            {
                return one.SN == sn;
            });

        if (iter == _heap.end())
            return;

        _heap.erase(iter);
    }

    // ���½���heap����
    make_heap(_heap.begin(), _heap.end(), CompareTimer());
}

bool TimerComponent::CheckTime()
{
    if (_heap.empty())
        return false;

    const auto data = _heap.front();
    return data.NextTime <= Global::GetInstance()->TimeTick;
}

Timer TimerComponent::PopTimeHeap()
{
    // ����heap��Ԫ��, �������������ĩβ
    pop_heap(_heap.begin(), _heap.end(), CompareTimer());

    Timer data = _heap.back();
    _heap.pop_back();

    return data;
}

void TimerComponent::Update()
{
    while (CheckTime())
    {
        Timer data = PopTimeHeap();
        data.Handler();

        if (data.CallCountTotal != 0)
            data.CallCountCur++;

        if (data.CallCountTotal != 0 && data.CallCountCur >= data.CallCountTotal)
        {
            //delete pNode; ȡ��֮�󣬲��ټ������
        }
        else
        {
            // ���¼������
            data.NextTime = timeutil::AddSeconds(Global::GetInstance()->TimeTick, data.DurationSecond);
            Add(data);
        }
    }
}
