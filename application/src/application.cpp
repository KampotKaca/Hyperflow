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
			wn = hf::Hyperflow::OpenWindow(data, nullptr);
			count++;
		}
		if (hf::Input::IsDown(hf::Key::Space) && wn)
		{
			wn->Close();
		}
	}

	void Application::Render()
	{

	}

	void Application::Quit()
	{

	}
}