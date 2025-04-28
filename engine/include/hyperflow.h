#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "hexception.h"
#include "hgenericexception.h"

namespace hf
{
	void Preload();
	void Shutdown();

	void Run(const EngineData& engineData);
	bool IsRunning();
	Ref<Window> GetMainWindow();
	const std::string& GetApplicationTitle();
	void Terminate();

	namespace input
	{
		bool IsDown(const Ref<Window> &window, Key key);
		bool IsDownContinues(const Ref<Window> &window, Key key);
		bool IsUp(const Ref<Window> &window, Key key);

		bool IsDown(const Ref<Window> &window, Button button);
		bool IsDownContinues(const Ref<Window> &window, Button button);
		bool IsUp(const Ref<Window> &window, Button button);

		bool IsDown(Key key);
		bool IsDownContinues(Key key);
		bool IsUp(Key key);

		bool IsDown(Button button);
		bool IsDownContinues(Button button);
		bool IsUp(Button button);

		KeyState GetState(Key key);
		ButtonState GetState(Button button);
		const std::string& GetWrite();

		ivec2 GetPointerPosition();
		ivec2 GetPointerDelta();
		vec2 GetScrollDelta();

		KeyState GetState(const Ref<Window> &window, Key key);
		ButtonState GetState(const Ref<Window> &window, Button button);
		const std::string& GetWrite(const Ref<Window> &window);

		ivec2 GetPointerPosition(const Ref<Window> &window);
		ivec2 GetPointerDelta(const Ref<Window> &window);
		vec2 GetScrollDelta(const Ref<Window> &window);
	}

	namespace window
	{
		Ref<Window> Open(const WindowData& data, const Ref<Window>& parent);
		bool Close(const Ref<Window> &window);
		void CloseAll();

		const std::string& GetTitle(const Ref<Window> &window);
		ivec2 GetSize(const Ref<Window> &window);
		ivec2 GetPosition(const Ref<Window> &window);
		IRect GetRect(const Ref<Window> &window);
		WindowFlags GetFlags(const Ref<Window> &window);
		WindowStyle GetStyle(const Ref<Window> &window);
		void* GetHandle(const Ref<Window> &window);

		void SetTitle(const Ref<Window> &window, const std::string& title);
		void SetSize(const Ref<Window> &window, ivec2 size);
		void SetPosition(const Ref<Window> &window, ivec2 position);
		void SetRect(const Ref<Window> &window, IRect rect);

		bool IsClosing(const Ref<Window> &window);
		void SetFlags(const Ref<Window> &window, WindowFlags flags);
		void Focus(const Ref<Window> &window);
	}

	namespace time
	{
		uint64_t GetFrameCount();
		double GetDeltaTime();
		double GetTimePassed();
		double GetAbsoluteTimePassed();
		int16_t GetTargetFrameRate();
		double GetSystemTime();
		int32_t GetFrameRate();
		void SetTargetFrameRate(int16_t targetFrameRate);
	}

	namespace renderer
	{
		Ref<Renderer> Create(uvec2 size);
		void Destroy(Ref<Renderer> rn);
		bool IsRunning(const Ref<Renderer>& rn);

		bool IsApiSupported(RenderingApiType targetApi);
		void QuerySupportedApis(std::vector<RenderingApiType>& apis);

		//Destroy every renderer which is not connected to the window, before you try to change api
		void ChangeApi(RenderingApiType targetApi);

		void Resize(Ref<Renderer> rn, uvec2 size);
		void Draw(const DrawCallInfo& info);
		void UnloadAllResources();
	}

	namespace shader
	{
		Ref<Shader> Create(const ShaderCreationInfo& info);
		void Destroy(const Ref<Shader>& shader);
		void Destroy(const Ref<Shader>* pShaders, uint32_t count);
		void DestroyAll();
		bool IsRunning(const Ref<Shader>& shader);

		void Bind(const Ref<Shader>& shader, BufferAttrib attrib);
		void Bind(const Ref<Renderer>& renderer, const Ref<Shader>& shader, BufferAttrib attrib);
	}

	namespace vertbuffer
	{
		BufferAttrib CreateAttrib(const BufferAttribCreateInfo& info);
		Ref<VertBuffer> Create(const VertBufferCreationInfo& info);
		void Destroy(const Ref<VertBuffer>& buffer);
		void Destroy(const Ref<VertBuffer>* pBuffers, uint32_t count);
		void DestroyAll();
		bool IsRunning(const Ref<VertBuffer>& buffer);
		void Upload(const VertBufferUploadInfo& info);
	}

	namespace utils
	{
		ivec3 ConvertVersion(const char* version);
		bool ReadFile(const std::string& filename, std::vector<char>& result);
		bool FileExists(const char* path);

		[[nodiscard]] void* Allocate(std::size_t n);
		[[nodiscard]] void* AllocateAligned(std::size_t n, std::align_val_t align);
		void Deallocate(void* p);
		void DeallocateAligned(void* p, std::align_val_t align);
	}
}

#endif //HYPERFLOW_H
