#pragma once
#include "packet.h"

class IMessageCallBack : public Component<IMessageCallBack>
{
public:
	virtual ~IMessageCallBack() = default;
	virtual bool ProcessPacket(Packet* packet) = 0;
};

using MsgCallbackFun = std::function<void(Packet*)>;
/// <summary>
/// 只要挂载了MessageCallBack的实体, ProcessPacket()无差别的处理包裹
/// </summary>
class MessageCallBack :public IMessageCallBack, public IAwakeFromPoolSystem<MsgCallbackFun>
{
public:
	void Awake(MsgCallbackFun fun) override;
	void BackToPool() override;
	virtual bool ProcessPacket(Packet* pPacket) override;

private:
	MsgCallbackFun _handleFunction;
};

/// <summary>
/// ProcessPacket()首先通过包裹中的NetIdentify数据找到唯一实体再处理包裹
/// </summary>
/// <typeparam name="T"></typeparam>
template<class T>
class MessageCallBackFilter :public IMessageCallBack, public IAwakeFromPoolSystem<>
{
public:
	void Awake() override {}
	void BackToPool() override
	{
		HandleFunction = nullptr;
		GetFilterObj = nullptr;
	}

	std::function<void(T*, Packet*)> HandleFunction{ nullptr };
	/// <summary>
	/// 根据包裹中NetIdentify数据获取处理包裹的实体
	/// </summary>
	std::function<T* (NetIdentify*)> GetFilterObj{ nullptr };

	virtual bool ProcessPacket(Packet* pPacket) override
	{
		auto pObj = GetFilterObj(pPacket);
		if (pObj == nullptr)
			return false;

#ifdef LOG_TRACE_COMPONENT_OPEN
		const google::protobuf::EnumDescriptor* descriptor = Proto::MsgId_descriptor();
		const auto name = descriptor->FindValueByNumber(pPacket->GetMsgId())->name();

		const auto traceMsg = std::string("process. ")
			.append(" sn:").append(std::to_string(pPacket->GetSN()))
			.append(" msgId:").append(name);
		ComponentHelp::GetTraceComponent()->Trace(TraceType::Packet, pPacket->GetSocketKey()->Socket, traceMsg);
#endif

		HandleFunction(pObj, pPacket);
		return true;
	}
};

