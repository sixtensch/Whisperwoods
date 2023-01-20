#pragma once

/* CHSL

	|	Camera2
	|
	|	- 2D-camera for manipulating 2D vectors.

*/







namespace cs
{

	template <class T_vector>
	class Camera2
	{
	public:
		Camera2(float scale, T_vector position, T_vector resolution);

		float ScreenToWorld(float distance) const;
		T_vector ScreenToWorld(T_vector position) const;

		float WorldToScreen(float distance) const;
		T_vector WorldToScreen(T_vector position, float parallaxFactor = 1.0f) const;

		float GetScaleFactor() const;
		float GetInvScaleFactor() const;

		float GetScale() const;
		float GetInvScale() const;

		T_vector GetPosition() const;

		void Move(T_vector movement);
		void Zoom(float zoom, bool factor = false);

		void SetResolution(T_vector resolution);
		void SetScale(float scale);
		void SetPosition(T_vector position);

	private:
		static const int c_averageWUWidth = 10;

		float m_resFactor;
		float m_invResFactor;

		T_vector m_resolution;
		T_vector m_position;
		float m_scale;
		float m_invScale;
	};



	// Implementation

	template<class T_vector>
	inline Camera2<T_vector>::Camera2(float scale, T_vector position, T_vector resolution)
		:
		m_position(position),
		m_scale(0),
		m_invScale(0)
	{
		SetScale(scale);
		SetResolution(resolution);
	}

	template<class T_vector>
	inline float Camera2<T_vector>::ScreenToWorld(float distance) const
	{
		return distance * GetInvScaleFactor();
	}

	template<class T_vector>
	inline T_vector Camera2<T_vector>::ScreenToWorld(T_vector position) const
	{
		return ((position - m_resolution * 0.5f) * GetInvScaleFactor()) + m_position;
	}

	template<class T_vector>
	inline float Camera2<T_vector>::WorldToScreen(float distance) const
	{
		return distance * GetScaleFactor();
	}

	template<class T_vector>
	inline T_vector Camera2<T_vector>::WorldToScreen(T_vector position, float parallaxFactor) const
	{
		return (position - m_position * parallaxFactor) * GetScaleFactor() + m_resolution * 0.5f;
	}

	template<class T_vector>
	inline float Camera2<T_vector>::GetScaleFactor() const
	{
		return m_scale * m_resFactor;
	}

	template<class T_vector>
	inline float Camera2<T_vector>::GetInvScaleFactor() const
	{
		return m_invScale * m_invResFactor;
	}

	template<class T_vector>
	inline float Camera2<T_vector>::GetScale() const
	{
		return m_scale;
	}

	template<class T_vector>
	inline float Camera2<T_vector>::GetInvScale() const
	{
		return m_invScale;
	}

	template<class T_vector>
	inline T_vector Camera2<T_vector>::GetPosition() const
	{
		return m_position;
	}

	template<class T_vector>
	inline void Camera2<T_vector>::Move(T_vector movement)
	{
		m_position += movement;
	}

	template<class T_vector>
	inline void Camera2<T_vector>::Zoom(float zoom, bool factor)
	{
		if (factor)
		{
			if (zoom == 0)
			{
				return;
			}

			m_scale *= zoom;
		}
		else
		{
			if (zoom == -m_scale)
			{
				return;
			}

			m_scale += zoom;
		}

		m_invScale = 1 / m_scale;
	}

	template<class T_vector>
	inline void Camera2<T_vector>::SetResolution(T_vector resolution)
	{
		m_resolution = resolution;

		m_resFactor = (resolution.x + resolution.y) / (2.0f * c_averageWUWidth);
		m_invResFactor = 1.0f / m_resFactor;
	}

	template<class T_vector>
	inline void Camera2<T_vector>::SetScale(float scale)
	{
		if (scale == 0.0f)
		{
			return;
		}

		m_scale = scale;
		m_invScale = 1.0f / scale;
	}

	template<class T_vector>
	inline void Camera2<T_vector>::SetPosition(T_vector position)
	{
		m_position = position;
	}

}