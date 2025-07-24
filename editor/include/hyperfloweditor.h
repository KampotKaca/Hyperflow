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
    enum class DrawStateFlag { None = 0, Nameless = 1 << 0, ButtonLess = 1 << 1, DontStretchWidth = 1 << 2, DontUseDropdown = 1 << 3 };
    DEFINE_ENUM_FLAGS(Condition)
    DEFINE_ENUM_FLAGS(DrawStateFlag)

#if DEBUG

    void Load(const LoadInfo& info);
    void Unload();

    void StartFrame();
    void EndFrame();
    void DrawFrame(void* cmd);

    bool StartWindow(const char* name = "New Window", bool* isOpen = nullptr, WindowFlags flags = WindowFlags::None);
    void EndWindow();

    bool StartDropdown(const char* label = "Dropdown");
    void EndDropdown();

    bool StartComponent(const char* label = "Component", DrawStateFlag flags = DrawStateFlag::None);
    void EndComponent();

    void SetNextWindowSize(vec2 size, Condition cond = Condition::None);
    void SetNextWindowPos(vec2 pos, Condition cond = Condition::None);

    bool Draw(const char* label, Transform& trs, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, DirectionalLight& dl, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, Camera3DCore& cam, DrawStateFlag flags = DrawStateFlag::None);
    
    bool Draw(const char* label, const Ref<AudioPlayer>& pl, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, const Ref<AudioPlayer3D>& pl, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, const Ref<AudioListener>& ls, DrawStateFlag flags = DrawStateFlag::None);

    bool DrawString(const char* label, std::string& v, DrawStateFlag flags = DrawStateFlag::None);
    bool DrawStringBox(const char* label, std::string& v, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, bool& v,     DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int8_t& v,   DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint8_t& v,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int16_t& v,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint16_t& v, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int32_t& v,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint32_t& v, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int64_t& v,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint64_t& v, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, float_t& v,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, double_t& v, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i8vec2& vec, int8_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i8vec3& vec, int8_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i8vec4& vec, int8_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u8vec2& vec, uint8_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u8vec3& vec, uint8_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u8vec4& vec, uint8_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i16vec2& vec, int16_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i16vec3& vec, int16_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i16vec4& vec, int16_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u16vec2& vec, uint16_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u16vec3& vec, uint16_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u16vec4& vec, uint16_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, ivec2& vec, int32_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, ivec3& vec, int32_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, ivec4& vec, int32_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, uvec2& vec, uint32_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uvec3& vec, uint32_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uvec4& vec, uint32_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i64vec2& vec, int64_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i64vec3& vec, int64_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i64vec4& vec, int64_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u64vec2& vec, uint64_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u64vec3& vec, uint64_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u64vec4& vec, uint64_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, vec2& vec, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, vec3& vec, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, vec4& vec, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, dvec2& vec, double_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, dvec3& vec, double_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, dvec4& vec, double_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool DrawColor(const char* label, vec4& vec, DrawStateFlag flags = DrawStateFlag::None);
    bool DrawColor(const char* label, vec3& vec, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, int8_t& v,   int8_t from,   DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint8_t& v,  uint8_t from,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int16_t& v,  int16_t from,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint16_t& v, uint16_t from, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int32_t& v,  int32_t from,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint32_t& v, uint32_t from, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int64_t& v,  int64_t from,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint64_t& v, uint64_t from, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, float_t& v,  float_t from,  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, double_t& v, double_t from, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i8vec2& vec, i8vec2 from, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i8vec3& vec, i8vec3 from, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i8vec4& vec, i8vec4 from, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u8vec2& vec, u8vec2 from, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u8vec3& vec, u8vec3 from, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u8vec4& vec, u8vec4 from, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i16vec2& vec, i16vec2 from, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i16vec3& vec, i16vec3 from, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i16vec4& vec, i16vec4 from, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u16vec2& vec, u16vec2 from, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u16vec3& vec, u16vec3 from, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u16vec4& vec, u16vec4 from, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, ivec2& vec, ivec2 from, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, ivec3& vec, ivec3 from, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, ivec4& vec, ivec4 from, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, uvec2& vec, uvec2 from, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uvec3& vec, uvec3 from, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uvec4& vec, uvec4 from, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i64vec2& vec, i64vec2 from, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i64vec3& vec, i64vec3 from, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i64vec4& vec, i64vec4 from, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u64vec2& vec, u64vec2 from, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u64vec3& vec, u64vec3 from, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u64vec4& vec, u64vec4 from, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, vec2& vec, vec2 from, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, vec3& vec, vec3 from, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, vec4& vec, vec4 from, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, dvec2& vec, dvec2 from, double_t resetValue = 0.0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, dvec3& vec, dvec3 from, double_t resetValue = 0.0, DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, dvec4& vec, dvec4 from, double_t resetValue = 0.0, DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, int8_t& v,   int8_t from,   int8_t to,   const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint8_t& v,  uint8_t from,  uint8_t to,  const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int16_t& v,  int16_t from,  int16_t to,  const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint16_t& v, uint16_t from, uint16_t to, const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int32_t& v,  int32_t from,  int32_t to,  const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint32_t& v, uint32_t from, uint32_t to, const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, int64_t& v,  int64_t from,  int64_t to,  const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uint64_t& v, uint64_t from, uint64_t to, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, float_t& v,  float_t from,  float_t to,  const char* format = "%.2f",  DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, double_t& v, double_t from, double_t to, const char* format = "%.3d",  DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i8vec2& vec, i8vec2 from, i8vec2 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i8vec3& vec, i8vec3 from, i8vec3 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i8vec4& vec, i8vec4 from, i8vec4 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u8vec2& vec, u8vec2 from, u8vec2 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u8vec3& vec, u8vec3 from, u8vec3 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u8vec4& vec, u8vec4 from, u8vec4 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i16vec2& vec, i16vec2 from, i16vec2 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i16vec3& vec, i16vec3 from, i16vec3 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i16vec4& vec, i16vec4 from, i16vec4 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u16vec2& vec, u16vec2 from, u16vec2 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u16vec3& vec, u16vec3 from, u16vec3 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u16vec4& vec, u16vec4 from, u16vec4 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, uvec2& vec, uvec2 from, uvec2 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uvec3& vec, uvec3 from, uvec3 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, uvec4& vec, uvec4 from, uvec4 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, i64vec2& vec, i64vec2 from, i64vec2 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i64vec3& vec, i64vec3 from, i64vec3 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, i64vec4& vec, i64vec4 from, i64vec4 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, u64vec2& vec, u64vec2 from, u64vec2 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u64vec3& vec, u64vec3 from, u64vec3 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, u64vec4& vec, u64vec4 from, u64vec4 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);

    bool Draw(const char* label, dvec2& vec, dvec2 from, dvec2 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, dvec3& vec, dvec3 from, dvec3 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None);
    bool Draw(const char* label, dvec4& vec, dvec4 from, dvec4 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, int8_t& v,   int8_t from,   int8_t to,   const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, uint8_t& v,  uint8_t from,  uint8_t to,  const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, int16_t& v,  int16_t from,  int16_t to,  const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, uint16_t& v, uint16_t from, uint16_t to, const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, int32_t& v,  int32_t from,  int32_t to,  const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, uint32_t& v, uint32_t from, uint32_t to, const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, int64_t& v,  int64_t from,  int64_t to,  const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, uint64_t& v, uint64_t from, uint64_t to, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, float_t& v,  float_t from,  float_t to,  const char* format = "%.2f",  DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, double_t& v, double_t from, double_t to, const char* format = "%.3d",  DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, i8vec2& vec, i8vec2 from, i8vec2 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, i8vec3& vec, i8vec3 from, i8vec3 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, i8vec4& vec, i8vec4 from, i8vec4 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, u8vec2& vec, u8vec2 from, u8vec2 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, u8vec3& vec, u8vec3 from, u8vec3 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, u8vec4& vec, u8vec4 from, u8vec4 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, i16vec2& vec, i16vec2 from, i16vec2 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, i16vec3& vec, i16vec3 from, i16vec3 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, i16vec4& vec, i16vec4 from, i16vec4 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, u16vec2& vec, u16vec2 from, u16vec2 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, u16vec3& vec, u16vec3 from, u16vec3 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, u16vec4& vec, u16vec4 from, u16vec4 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, uvec2& vec, uvec2 from, uvec2 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, uvec3& vec, uvec3 from, uvec3 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, uvec4& vec, uvec4 from, uvec4 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, i64vec2& vec, i64vec2 from, i64vec2 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, i64vec3& vec, i64vec3 from, i64vec3 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, i64vec4& vec, i64vec4 from, i64vec4 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, u64vec2& vec, u64vec2 from, u64vec2 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, u64vec3& vec, u64vec3 from, u64vec3 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, u64vec4& vec, u64vec4 from, u64vec4 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None);

    bool DrawSlider(const char* label, dvec2& vec, dvec2 from, dvec2 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, dvec3& vec, dvec3 from, dvec3 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None);
    bool DrawSlider(const char* label, dvec4& vec, dvec4 from, dvec4 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None);

#else

    inline void Load(const LoadInfo& info){}
    inline void Unload(){}

    inline void StartFrame(){}
    inline void EndFrame(){}
    inline void DrawFrame(void* cmd){}

    inline bool StartDropdown(const char* label = "Dropdown") { return false; }
    inline void EndDropdown() {}

    inline bool StartComponent(const char* label = "Component", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline void EndComponent() {}

    inline bool StartWindow(const char* name = "New Window", bool* isOpen = nullptr, WindowFlags flags = WindowFlags::None) { return false; }
    inline void EndWindow(){}

    inline void SetNextWindowSize(vec2 size, Condition cond = Condition::None){}
    inline void SetNextWindowPos(vec2 pos, Condition cond = Condition::None){}

    inline bool Draw(const char* label, Transform& trs,         DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, DirectionalLight& dl,   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, Camera3DCore& cam,      DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, const Ref<AudioPlayer>& pl,   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, const Ref<AudioPlayer3D>& pl, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, const Ref<AudioListener>& ls, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawString   (const char* label, std::string& v, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawStringBox(const char* label, std::string& v, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, bool& v,     DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int8_t& v,   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint8_t& v,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int16_t& v,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint16_t& v, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int32_t& v,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint32_t& v, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int64_t& v,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint64_t& v, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, float_t& v,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, double_t& v, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i8vec2& vec, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i8vec3& vec, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i8vec4& vec, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u8vec2& vec, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u8vec3& vec, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u8vec4& vec, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i16vec2& vec, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i16vec3& vec, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i16vec4& vec, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u16vec2& vec, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u16vec3& vec, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u16vec4& vec, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, ivec2& vec, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, ivec3& vec, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, ivec4& vec, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, uvec2& vec, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uvec3& vec, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uvec4& vec, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i64vec2& vec, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i64vec3& vec, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i64vec4& vec, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u64vec2& vec, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u64vec3& vec, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u64vec4& vec, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, vec2& vec, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, vec3& vec, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, vec4& vec, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, dvec2& vec, double_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, dvec3& vec, double_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, dvec4& vec, double_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawColor(const char* label, vec4& vec, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawColor(const char* label, vec3& vec, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, int8_t& v,   int8_t from,   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint8_t& v,  uint8_t from,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int16_t& v,  int16_t from,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint16_t& v, uint16_t from, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int32_t& v,  int32_t from,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint32_t& v, uint32_t from, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int64_t& v,  int64_t from,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint64_t& v, uint64_t from, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, float_t& v,  float_t from,  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, double_t& v, double_t from, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i8vec2& vec, i8vec2 from, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i8vec3& vec, i8vec3 from, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i8vec4& vec, i8vec4 from, int8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u8vec2& vec, u8vec2 from, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u8vec3& vec, u8vec3 from, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u8vec4& vec, u8vec4 from, uint8_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i16vec2& vec, i16vec2 from, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i16vec3& vec, i16vec3 from, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i16vec4& vec, i16vec4 from, int16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u16vec2& vec, u16vec2 from, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u16vec3& vec, u16vec3 from, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u16vec4& vec, u16vec4 from, uint16_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, ivec2& vec, ivec2 from, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, ivec3& vec, ivec3 from, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, ivec4& vec, ivec4 from, int32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, uvec2& vec, uvec2 from, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uvec3& vec, uvec3 from, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uvec4& vec, uvec4 from, uint32_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i64vec2& vec, i64vec2 from, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i64vec3& vec, i64vec3 from, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i64vec4& vec, i64vec4 from, int64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u64vec2& vec, u64vec2 from, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u64vec3& vec, u64vec3 from, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u64vec4& vec, u64vec4 from, uint64_t resetValue = 0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, vec2& vec, vec2 from, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, vec3& vec, vec3 from, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, vec4& vec, vec4 from, float_t resetValue = 0.0f, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, dvec2& vec, dvec2 from, double_t resetValue = 0.0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, dvec3& vec, dvec3 from, double_t resetValue = 0.0, DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, dvec4& vec, dvec4 from, double_t resetValue = 0.0, DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, int8_t& v,   int8_t from,   int8_t to,   const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint8_t& v,  uint8_t from,  uint8_t to,  const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int16_t& v,  int16_t from,  int16_t to,  const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint16_t& v, uint16_t from, uint16_t to, const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int32_t& v,  int32_t from,  int32_t to,  const char* format = "%.i",   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint32_t& v, uint32_t from, uint32_t to, const char* format = "%.u",   DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, int64_t& v,  int64_t from,  int64_t to,  const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uint64_t& v, uint64_t from, uint64_t to, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, float_t& v,  float_t from,  float_t to,  const char* format = "%.2f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, double_t& v, double_t from, double_t to, const char* format = "%.3d",  DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i8vec2& vec, i8vec2 from, i8vec2 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i8vec3& vec, i8vec3 from, i8vec3 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i8vec4& vec, i8vec4 from, i8vec4 to, int8_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u8vec2& vec, u8vec2 from, u8vec2 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u8vec3& vec, u8vec3 from, u8vec3 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u8vec4& vec, u8vec4 from, u8vec4 to, uint8_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i16vec2& vec, i16vec2 from, i16vec2 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i16vec3& vec, i16vec3 from, i16vec3 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i16vec4& vec, i16vec4 from, i16vec4 to, int16_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u16vec2& vec, u16vec2 from, u16vec2 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u16vec3& vec, u16vec3 from, u16vec3 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u16vec4& vec, u16vec4 from, u16vec4 to, uint16_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue = 0, const char* format = "%.i", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, uvec2& vec, uvec2 from, uvec2 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uvec3& vec, uvec3 from, uvec3 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, uvec4& vec, uvec4 from, uvec4 to, uint32_t resetValue = 0, const char* format = "%.u", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, i64vec2& vec, i64vec2 from, i64vec2 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i64vec3& vec, i64vec3 from, i64vec3 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, i64vec4& vec, i64vec4 from, i64vec4 to, int64_t resetValue = 0, const char* format = "%.ill", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, u64vec2& vec, u64vec2 from, u64vec2 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u64vec3& vec, u64vec3 from, u64vec3 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, u64vec4& vec, u64vec4 from, u64vec4 to, uint64_t resetValue = 0, const char* format = "%.ull", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool Draw(const char* label, dvec2& vec, dvec2 from, dvec2 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, dvec3& vec, dvec3 from, dvec3 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool Draw(const char* label, dvec4& vec, dvec4 from, dvec4 to, double_t resetValue = 0.0, const char* format = "%.3d", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, int8_t& v,   int8_t from,   int8_t to,   const char* format = "%.f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, uint8_t& v,  uint8_t from,  uint8_t to,  const char* format = "%.f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, int16_t& v,  int16_t from,  int16_t to,  const char* format = "%.f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, uint16_t& v, uint16_t from, uint16_t to, const char* format = "%.f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, int32_t& v,  int32_t from,  int32_t to,  const char* format = "%.f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, uint32_t& v, uint32_t from, uint32_t to, const char* format = "%.f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, int64_t& v,  int64_t from,  int64_t to,  const char* format = "%.f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, uint64_t& v, uint64_t from, uint64_t to, const char* format = "%.f",  DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, float_t& v,  float_t from,  float_t to,  const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, double_t& v, double_t from, double_t to, const char* format = "%.3f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, i8vec2& vec, i8vec2 from, i8vec2 to, int8_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, i8vec3& vec, i8vec3 from, i8vec3 to, int8_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, i8vec4& vec, i8vec4 from, i8vec4 to, int8_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, u8vec2& vec, u8vec2 from, u8vec2 to, uint8_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, u8vec3& vec, u8vec3 from, u8vec3 to, uint8_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, u8vec4& vec, u8vec4 from, u8vec4 to, uint8_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, i16vec2& vec, i16vec2 from, i16vec2 to, int16_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, i16vec3& vec, i16vec3 from, i16vec3 to, int16_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, i16vec4& vec, i16vec4 from, i16vec4 to, int16_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, u16vec2& vec, u16vec2 from, u16vec2 to, uint16_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, u16vec3& vec, u16vec3 from, u16vec3 to, uint16_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, u16vec4& vec, u16vec4 from, u16vec4 to, uint16_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, ivec2& vec, ivec2 from, ivec2 to, int32_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, ivec3& vec, ivec3 from, ivec3 to, int32_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, ivec4& vec, ivec4 from, ivec4 to, int32_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, uvec2& vec, uvec2 from, uvec2 to, uint32_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, uvec3& vec, uvec3 from, uvec3 to, uint32_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, uvec4& vec, uvec4 from, uvec4 to, uint32_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, i64vec2& vec, i64vec2 from, i64vec2 to, int64_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, i64vec3& vec, i64vec3 from, i64vec3 to, int64_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, i64vec4& vec, i64vec4 from, i64vec4 to, int64_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, u64vec2& vec, u64vec2 from, u64vec2 to, uint64_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, u64vec3& vec, u64vec3 from, u64vec3 to, uint64_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, u64vec4& vec, u64vec4 from, u64vec4 to, uint64_t resetValue = 0, const char* format = "%.f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, vec2& vec, vec2 from, vec2 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, vec3& vec, vec3 from, vec3 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, vec4& vec, vec4 from, vec4 to, float_t resetValue = 0.0f, const char* format = "%.2f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

    inline bool DrawSlider(const char* label, dvec2& vec, dvec2 from, dvec2 to, double_t resetValue = 0.0, const char* format = "%.3f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, dvec3& vec, dvec3 from, dvec3 to, double_t resetValue = 0.0, const char* format = "%.3f", DrawStateFlag flags = DrawStateFlag::None) { return false; }
    inline bool DrawSlider(const char* label, dvec4& vec, dvec4 from, dvec4 to, double_t resetValue = 0.0, const char* format = "%.3f", DrawStateFlag flags = DrawStateFlag::None) { return false; }

#endif
}

#endif //HHYPERFLOWEDITOR_H
