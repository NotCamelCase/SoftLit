#pragma once

namespace softlit
{
	class Rasterizer
	{
	public:
		Rasterizer();
		~Rasterizer();

		const std::vector<uchar>& getColorBuffer() const
		{
			return m_colorBuffer;
		}

		const std::vector<uchar>& getZBuffer() const
		{
			return m_zBuffer;
		}

	private:
		std::vector<uchar> m_colorBuffer; // Used to hold rasterized frame
		std::vector<uchar> m_zBuffer; // Used as depth buffer
	};
}