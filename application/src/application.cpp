#include "application.h"
#include <hyperflow.h>
#include <sstream>

namespace app
{
	int count;
	hf::Ref<hf::Window> wn;
	int reqCount = 0;

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

		if (hf::Input::IsDown(hf::Key::A))
		{
			hf::WindowData data =
			{
				.title = names[count % 5],
				.flags = hf::WindowFlags::Default,
				.style = hf::WindowStyle::Default,
				.position = { 100, 100 },
				.size = { 200, 200 }
			};
			wn = hf::Hyperflow::OpenWindow(data, hf::Hyperflow::MainWindow());
			count++;
		}
		if (hf::Input::IsDown(hf::Key::Space) && wn)
		{
			wn->Close();
		}

		if (reqCount == 0 && hf::Time::GetTimePassedAfterLaunch() > 2.0)
		{
			auto window = hf::Hyperflow::MainWindow();
			window->SetFlags((hf::WindowFlags)((int32_t)hf::WindowFlags::Visible | (int32_t)hf::WindowFlags::Maximized));
			reqCount++;
		}

		if (reqCount == 1 && hf::Time::GetTimePassedAfterLaunch() > 4.0)
		{
			auto window = hf::Hyperflow::MainWindow();
			window->SetFlags((hf::WindowFlags)((int32_t)hf::WindowFlags::Visible));
			reqCount++;
		}

		if (reqCount == 2 && hf::Time::GetTimePassedAfterLaunch() > 6.0)
		{
			auto window = hf::Hyperflow::MainWindow();
			window->SetFlags((hf::WindowFlags)((int32_t)hf::WindowFlags::Visible | (int32_t)hf::WindowFlags::Minimized));
			reqCount++;
		}
	}

	void Application::Render()
	{

	}

	void Application::Quit()
	{

	}
}