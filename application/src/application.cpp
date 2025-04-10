#include "application.h"
#include <hyperflow.h>
#include <sstream>

namespace app
{
	int count;
	hf::Ref<hf::Window> wn;
	int reqCount = 0;
	int32_t lastReq = -1;

	void Application::Start()
	{
		count = 0;
	}

	void Application::Update()
	{
		const std::string names[5] =
		{
			"N1",
			"N2",
			"N3",
			"N4",
			"N5",
		};

		if (hf::input::IsDown(hf::Key::A))
		{
			hf::WindowData data =
			{
				.title = names[count % 5],
				.flags = hf::WindowFlags::Default,
				.style = hf::WindowStyle::Default,
				.position = { 100, 100 },
				.size = { 200, 200 }
			};
			wn = hf::OpenWindow(data, nullptr);
			count++;
		}
		if (hf::input::IsDown(hf::Key::Space) && wn)
		{
			hf::window::Close(wn);
		}

		// auto delta = hf::input::GetScrollDelta();
		// if (delta != hf::vec2(0, 0)) LOG_INFO("Scroll (X: %f, Y: %f)", delta.x, delta.y);
		//
		// auto mDelta = hf::input::GetPointerDelta();
		// if (mDelta != hf::ivec2(0, 0)) LOG_INFO("Move: (X: %i, Y: %i)", mDelta.x, mDelta.y);

		auto cReq = (int32_t)(hf::time::GetTimePassed() / 0.2);
		if (cReq != reqCount)
		{
			std::ostringstream oss;
			oss << "[Hyperflow] " << hf::time::GetFrameRate();
			// oss << "[Hyperflow] " << hf::Time::GetTimePassed();
			hf::window::SetTitle(hf::GetMainWindow(), oss.str().c_str());
			reqCount = cReq;
		}
	}

	void Application::Render()
	{

	}

	void Application::Quit()
	{

	}
}