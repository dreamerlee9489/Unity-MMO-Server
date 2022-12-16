#pragma once

#include "common.h"
#include <log4cplus/streams.h>

struct Vector3
{
	Vector3& operator+=(const Vector3& other)
	{
		this->X += other.X;
		this->Y += other.Y;
		this->Z += other.Z;
		return *this;
	}

	Vector3& operator-=(const Vector3& other)
	{
		this->X -= other.X;
		this->Y -= other.Y;
		this->Z -= other.Z;
		return *this;
	}

	friend Vector3 operator+(Vector3 lhs, const Vector3& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	friend Vector3 operator-(Vector3 lhs, const Vector3& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	Vector3() { X = 0; Y = 0; Z = 0; }
	Vector3(const float x, const float y, const float z) :X(x), Y(y), Z(z) {}
	Vector3(Net::Vector3& postion) { ParserFromProto(postion); }

	static float SqrDistance(Vector3& src, Vector3& dst);
	void ParserFromProto(const Net::Vector3& position);
	void SerializeToProto(Net::Vector3* pProto) const;
	float GetDistance(Vector3& point) const;
	float GetManhaDist(Vector3& point) const;
	Vector3 Normalize() const;

	float X{ 0 };
	float Y{ 0 };
	float Z{ 0 };

	static Vector3 Zero;
};

std::ostream& operator <<(std::ostream& os, Vector3 v);

#if ENGINE_PLATFORM == PLATFORM_WIN32
log4cplus::tostream& operator <<(log4cplus::tostream& os, Vector3 v);
#endif