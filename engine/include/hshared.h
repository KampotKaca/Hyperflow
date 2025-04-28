#ifndef HSHARED_H
#define HSHARED_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <set>
#include <map>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <ranges>
#include <algorithm>

#include <bitset>
#include <queue>
#include <optional>

extern "C"
{
#include "log.h"
};

#include <sys/stat.h>
#include <sys/types.h>
#include <memory>
#include <atomic>

#include "hmath.h"

#if PLATFORM_LINUX
#ifdef None
#undef None;
#endif
#endif

namespace hf
{
	//region Aditionalls
	typedef void (*ActionCallback)(void* params);

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T>
	using URef = std::unique_ptr<T>;

	template <typename T, typename... Args>
	Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	URef<T> MakeURef(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	struct DllApi
	{
		void* handle;
		T api;
	};

#define TO_RES_PATH(x) (std::string(RES_PATH) + (x))

#if PLATFORM_WINDOWS
#define API extern "C" __declspec(dllexport)
#elif PLATFORM_LINUX
#define API extern "C" __attribute__((visibility("default")))
#endif
	//endregion

	//region Definitions

	enum class Key : uint8_t
	{
		None,
		Backspace,
		Tab,
		Enter,
		Pause,
		CapsLock,
		Escape,
		Space,
		PageUp, PageDown,
		End, Home,
		Left, Up, Right, Down,
		PrintScreen,
		Insert,
		Delete,
		Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		LeftSuper, RightSuper,
		Pad0, Pad1, Pad2, Pad3, Pad4, Pad5, Pad6, Pad7, Pad8, Pad9,
		PadMultiply,
		PadAdd,
		PadEqual,
		PadSubtract,
		PadDecimal,
		PadDivide,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		LeftBracket,
		Backslash,
		RightBracket,
		Apostrophe,
		NumLock,
		ScrollLock,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		LeftShift, RightShift,
		LeftControl, RightControl,
		LeftAlt, RightAlt,
		Semicolon,
		Equal,
		Comma,
		Minus,
		Period,
		Slash,
		Grave,
		Count
	};

	enum class Button : uint8_t { None, Left, Right, Wheel, Extra1, Extra2, Count };

	enum class KeyState { None, Down, DownContinues, Up };
	typedef KeyState ButtonState;

	struct Rect
	{
		vec2 position;
		vec2 size;
	};

	struct IRect
	{
		ivec2 position;
		ivec2 size;
	};

	//endregion

	//region Rendering

	struct Renderer;
	struct Shader;
	struct VertBuffer;
	typedef uint32_t BufferAttrib;

	enum class VertBufferDataType { U8, I8, U16, I16, U32, I32, U64, I64, F16, F32, F64, Count };
	enum class VertBufferMemoryType { Static, DynamicWrite, Count };

	struct BufferAttribFormat
	{
		VertBufferDataType type = VertBufferDataType::F32;
		uint32_t size = 1;
		//Do not implement lSize
		uint32_t lSize = 1;
	};

	struct BufferAttribCreateInfo
	{
		uint32_t bindingId = 0;
		uint32_t formatCount = 0;
		BufferAttribFormat* pFormats = nullptr;
	};

	struct VertBufferCreationInfo
	{
		BufferAttrib bufferAttrib = 0;
		VertBufferMemoryType memoryType = VertBufferMemoryType::Static;
		uint32_t vertexCount = 0;
		void* pVertices = nullptr;
	};

	struct VertBufferUploadInfo
	{
		const Ref<VertBuffer>& buffer;
		const void* data;
		uint32_t offset;
		uint32_t vertCount;
	};

	struct ShaderCreationInfo
	{
		uint32_t supportedAttribCount{};
		const BufferAttrib* pSupportedAttribs{};
		const std::string& vertexShaderLoc{};
		const std::string& fragmentShaderLoc{};
	};

	enum class RenderingApiType { None, Vulkan, Direct3D };

	struct DrawCallInfo
	{
		Ref<Renderer> renderer;
		Ref<VertBuffer>* pVertBuffers;
		uint32_t bufferCount = 0;
		uint32_t instanceCount = 0;
	};

	//endregion

	//region Window

	enum class WindowStyle
	{
		Default
	};

	enum class WindowFlags
	{
		Visible   = 1 << 0,
		Minimized = 1 << 1,
		Maximized = 1 << 2,

		Default = Visible
	};

	struct WindowData
	{
		std::string title = "Untitled";
		WindowFlags flags = WindowFlags::Default;
		WindowStyle style = WindowStyle::Default;
		ivec2 position = ivec2{ 300, 300 };
		ivec2 size = ivec2{ 200, 200 };

		void (*onPreRenderCallback)(const Ref<Renderer>&){};
		void (*onRenderCallback)(const Ref<Renderer>&){};
	};

	//endregion

	//region Engine

	enum class EngineUpdateType
	{
		Continues,   //Updates as often as is allowed, good for games or simulations, try to avoid it if you it's possible
		EventRaised  //Waits updates only on OS events, good for apps which doesn't change without user interaction
	};

	struct EngineLifecycleCallbacks
	{
		void (*onResourcesLoad)(){};
		void (*onStartCallback)(){};
		void (*onUpdateCallback)(){};
		void (*onQuitCallback)(){};
	};

	struct EngineData
	{
		std::string appTitle = "Hyperflow";
		RenderingApiType renderingApi = RenderingApiType::Vulkan;
		EngineUpdateType updateType = EngineUpdateType::EventRaised;
		EngineLifecycleCallbacks lifecycleCallbacks{};
		WindowData windowData{};
	};

	struct Window;

	//endregion

	//region Helpers

	uint32_t TrailingZeros64(uint64_t n);

	//endregion
}

#endif //HSHARED_H
