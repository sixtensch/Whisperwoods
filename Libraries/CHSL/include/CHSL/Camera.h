#pragma once

namespace cs
{

	template <class T_vector>
	class Camera
	{
	public:
		Camera(float scale, T_vector position, T_vector resolution)
			:
			m_position(position),
			m_scale(0),
			m_invScale(0)
		{
			SetScale(scale);
			SetResolution(resolution);
		}



		float ScreenToWorld(float distance) const
		{
			return distance * GetInvScaleFactor();
		}

		T_vector ScreenToWorld(T_vector position) const
		{
			return ((position - m_resolution * 0.5f) * GetInvScaleFactor()) + m_position;
		}



		float WorldToScreen(float distance) const
		{
			return distance * GetScaleFactor();
		}

		T_vector WorldToScreen(T_vector position, float parallaxFactor = 1.0f) const
		{
			return (position - m_position * parallaxFactor) * GetScaleFactor() + m_resolution * 0.5f;
		}



		float GetScaleFactor() const
		{
			return m_scale * m_resFactor;
		}

		float GetInvScaleFactor() const
		{
			return m_invScale * m_invResFactor;
		}



		float GetScale() const
		{
			return m_scale;
		}

		float GetInvScale() const
		{
			return m_invScale;
		}

		T_vector GetPosition() const
		{
			return m_position;
		}



		void Move(T_vector movement)
		{
			m_position += movement;
		}

		void Zoom(float zoom, bool factor = false)
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



		void SetResolution(T_vector resolution)
		{
			m_resolution = resolution;

			m_resFactor = (resolution.x + resolution.y) / (2.0f * c_averageWUWidth);
			m_invResFactor = 1.0f / m_resFactor;
		}

		void SetScale(float scale)
		{
			if (scale == 0.0f)
			{
				return;
			}

			m_scale = scale;
			m_invScale = 1.0f / scale;
		}

		void SetPosition(T_vector position)
		{
			m_position = position;
		}



	private:
		static const int c_averageWUWidth = 10;

		float m_resFactor;
		float m_invResFactor;

		T_vector m_resolution;
		T_vector m_position;
		float m_scale;
		float m_invScale;
	};

}