#pragma once

namespace HellEngine
{
	class EditorBuffer
	{
	public:
		EditorBuffer(int width, int height);
		EditorBuffer();

		unsigned int ID;
		unsigned int HoverTexture;// , SelectedTexture;

		void Configure(int width, int height);
	};
}
