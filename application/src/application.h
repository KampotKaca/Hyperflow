#ifndef APPLICATION_H
#define APPLICATION_H

namespace app
{
	class Application
	{
	public:

	private:
		static void Start();
		static void Update();
		static void Quit();

		static void OnMainWindowRender();
	};
}

#endif //APPLICATION_H
