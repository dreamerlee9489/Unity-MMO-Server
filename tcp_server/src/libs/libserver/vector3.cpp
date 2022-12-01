#include "vector3.h"
#include <cmath>

Vector3 Vector3::Zero = Vector3(0, 0, 0);

void Vector3::ParserFromProto(Proto::Vector3 position)
{
	this->X = position.x();
	this->Y = position.y();
	this->Z = position.z();
}

void Vector3::SerializeToProto(Proto::Vector3* pProto) const
{
	pProto->set_x(X);
	pProto->set_y(Y);
	pProto->set_z(Z);
}

float Vector3::GetDistance(Vector3 point) const
{
	const auto xv = point.X - this->X;
	const auto zv = point.Z - this->Z;
	return sqrt(xv * xv + zv * zv);
}

float Vector3::SqrDistance(Vector3& src, Vector3& dst)
{
	float dx = dst.X - src.X;
	float dz = dst.Z - src.Z;
	return (dx * dx + dz * dz);
}

Vector3 Vector3::Normalize() const
{
	Vector3 res{ 0, 0, 0 };
	float len = GetDistance(res);
	res.X = X / len;
	res.Z = Z / len;
	return res;
}

std::ostream& operator<<(std::ostream& os, Vector3 v)
{
	os << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
	return os;
}

#if ENGINE_PLATFORM == PLATFORM_WIN32
log4cplus::tostream& operator<<(log4cplus::tostream& os, Vector3 v)
{
	os << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
	return os;
}
#endif