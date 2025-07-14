#ifndef HHYPERFLOWEDITOR_H
#define HHYPERFLOWEDITOR_H

#include "hshared.h"
#include "hcomponents.h"

namespace hf::editor
{
    struct LoadInfo
    {
        void* windowHandle{};
        void* renderApiHandles{};
        MultisampleMode multisampleMode = MultisampleMode::MSAA_1X;
        TextureFormat* pColorFormats{};
        uint8_t colorFormatCount = 0;
        TextureFormat depthFormat = TextureFormat::D32_Sfloat;
        TextureFormat stencilFormat = TextureFormat::Undefined;
    };

    enum class WindowFlags
    {
        None = 0, NoTitleBar = 1 << 0, NoResize = 1 << 1, NoMove = 1 << 2, NoScrollbar = 1 << 3,
        NoScrollWithMouse = 1 << 4, NoCollapse = 1 << 5, AlwaysAutoResize = 1 << 6,
        NoBackground = 1 << 7, NoSavedSettings = 1 << 8, NoMouseInputs = 1 << 9,
        MenuBar  = 1 << 10, HorizontalScrollbar = 1 << 11, NoFocusOnAppearing = 1 << 12,
        NoBringToFrontOnFocus = 1 << 13, AlwaysVerticalScrollbar = 1 << 14,
        AlwaysHorizontalScrollbar = 1 << 15, NoNavInputs = 1 << 16,
        NoNavFocus = 1 << 17, UnsavedDocument = 1 << 18,
    };

    enum class Condition { None = 0, Always = 1 << 0, Once = 1 << 1, FirstUseEver = 1 << 2, Appearing = 1 << 3 };

#if DEBUG

    void Load(const LoadInfo& info);
    void Unload();

    void StartFrame();
    void EndFrame();
    void DrawFrame(void* cmd);

    bool StartWindow(const char* name = "New Window", bool* isOpen = nullptr, WindowFlags flags = WindowFlags::None);
    void EndWindow();

    bool DrawComponent(const char* label = "Component");

    void SetNextWindowSize(vec2 size, Condition cond = Condition::None);
    void SetNextWindowPos(vec2 pos, Condition cond = Condition::None);

    bool Draw(const char* label, Transform& trs);
    bool Draw(const char* label, DirectionalLight& dl);
    bool Draw(const char* label, Camera3DCore& cam);

    bool DrawString(const char* label, std::string& v);
    bool DrawStringBox(const char* label, std::string& v);

    bool Draw(const char* label, bool& v);
    bool Draw(const char* label, int16_t& v);
    bool Draw(const char* label, uint16_t& v);
    bool Draw(const char* label, int32_t& v);
    bool Draw(const char* label, uint32_t& v);
    bool Draw(const char* label, int64_t& v);
    bool Draw(const char* label, uint64_t& v);
    bool Draw(const char* label, float_t& v);
    bool Draw(const char* label, double_t& v);

    bool Draw(const char* label, vec2& vec, float_t resetValue = 0.0f);
    bool Draw(const char* label, vec3& vec, float_t resetValue = 0.0f);
    bool Draw(const char* label, vec4& vec, float_t resetValue = 0.0f);

    bool Draw(const char* label, ivec2& vec, int32_t resetValue = 0.0f);
    bool Draw(const char* label, ivec3& vec, int32_t resetValue = 0.0f);
    bool Draw(const char* label, ivec4& vec, int32_t resetValue = 0.0f);

    bool DrawColor(const char* label, vec4& vec);
    bool DrawColor(const char* label, vec3& vec);

    bool Draw(const char* label, int16_t& v, int16_t from, int16_t to);
    bool Draw(const char* label, uint16_t& v, uint16_t from, uint16_t to);
    bool Draw(const char* label, int32_t& v, int32_t from, int32_t to);
    bool Draw(const char* label, uint32_t& v, uint32_t from, uint32_t to);
    bool Draw(const char* label, int64_t& v, int64_t from, int64_t to);
    bool Draw(const char* label, uint64_t& v, uint64_t from, uint64_t to);
    bool Draw(const char* label, float_t& v, float_t from, float_t to);
    bool Draw(const char* label, double_t& v, double_t from, double_t to);

    bool Draw(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue = 0.0f);
    bool Draw(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue = 0.0f);
    bool Draw(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue = 0.0f);

    bool Draw(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue = 0.0f);
    bool Draw(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue = 0.0f);
    bool Draw(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue = 0.0f);

#else

    inline void Load(const LoadInfo& info){}
    inline void Unload(){}

    inline void StartFrame(){}
    inline void EndFrame(){}
    inline void DrawFrame(void* cmd){}

    bool DrawComponent(const char* label = "Component") { return false; }

    inline bool StartWindow(const char* name = "New Window", bool* isOpen = nullptr, WindowFlags flags = WindowFlags::None) { return false; }
    inline void EndWindow(){}

    inline void SetNextWindowSize(vec2 size, Condition cond = Condition::None){}
    inline void SetNextWindowPos(vec2 pos, Condition cond = Condition::None){}

    inline bool Draw(const char* label, Transform& trs)       { return false; }
    inline bool Draw(const char* label, DirectionalLight& dl) { return false; }
    inline bool Draw(const char* label, Camera3DCore& cam)    { return false; }

    inline bool DrawString(const char* label, std::string& v)    { return false; }
    inline bool DrawStringBox(const char* label, std::string& v) { return false; }

    inline bool Draw(const char* label, bool& v)     { return false; }
    inline bool Draw(const char* label, int16_t& v)  { return false; }
    inline bool Draw(const char* label, uint16_t& v) { return false; }
    inline bool Draw(const char* label, int32_t& v)  { return false; }
    inline bool Draw(const char* label, uint32_t& v) { return false; }
    inline bool Draw(const char* label, int64_t& v)  { return false; }
    inline bool Draw(const char* label, uint64_t& v) { return false; }
    inline bool Draw(const char* label, float_t& v)  { return false; }
    inline bool Draw(const char* label, double_t& v) { return false; }

    inline bool Draw(const char* label, vec2& vec, float_t resetValue = 0.0f) { return false; }
    inline bool Draw(const char* label, vec3& vec, float_t resetValue = 0.0f) { return false; }
    inline bool Draw(const char* label, vec4& vec, float_t resetValue = 0.0f) { return false; }

    inline bool Draw(const char* label, ivec2& vec, int32_t resetValue = 0.0f) { return false; }
    inline bool Draw(const char* label, ivec3& vec, int32_t resetValue = 0.0f) { return false; }
    inline bool Draw(const char* label, ivec4& vec, int32_t resetValue = 0.0f) { return false; }

    inline bool DrawColor(const char* label, vec4& vec) { return false; }
    inline bool DrawColor(const char* label, vec3& vec) { return false; }

    inline bool Draw(const char* label, int16_t& v, int16_t from, int16_t to)    { return false; }
    inline bool Draw(const char* label, uint16_t& v, uint16_t from, uint16_t to) { return false; }
    inline bool Draw(const char* label, int32_t& v, int32_t from, int32_t to)    { return false; }
    inline bool Draw(const char* label, uint32_t& v, uint32_t from, uint32_t to) { return false; }
    inline bool Draw(const char* label, int64_t& v, int64_t from, int64_t to)    { return false; }
    inline bool Draw(const char* label, uint64_t& v, uint64_t from, uint64_t to) { return false; }
    inline bool Draw(const char* label, float_t& v, float_t from, float_t to)    { return false; }
    inline bool Draw(const char* label, double_t& v, double_t from, double_t to) { return false; }

    inline bool Draw(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue = 0.0f) { return false; }
    inline bool Draw(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue = 0.0f) { return false; }
    inline bool Draw(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue = 0.0f) { return false; }

    inline bool Draw(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue = 0.0f) { return false; }
    inline bool Draw(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue = 0.0f) { return false; }
    inline bool Draw(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue = 0.0f) { return false; }

#endif
}

#endif //HHYPERFLOWEDITOR_H
