#pragma once

/* CHSL

	|	Quaternion
	|
	|	- Quaternion implementation.

*/









namespace cs
{

	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(const Quaternion&);
		~Quaternion();



	private:
		float m_real;
		float m_i;
		float m_j;
		float m_k;
	};

}

