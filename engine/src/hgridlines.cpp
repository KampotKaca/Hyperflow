#include "hyperflow.h"
#include "hinternal.h"

namespace hf::gridlines
{
    void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info)
    {
        Start_ShaderLayout(rn, inter::HF.staticResources.shaderLayouts.axisLines);
        {
            primitives::BindGlobalUniformBuffer(rn);

            Start_Shader(rn, inter::HF.staticResources.shaders.axisLines);
            {
                Start_Material(rn, nullptr);
                {
                    Start_DrawGroup(rn);
                    {
                        DrawGroupSet_PushConstant(rn, info);
                        DrawGroupAdd_DrawCall(rn, nullptr, inter::HF.staticResources.quadBuffer);
                    }
                    End_DrawGroup(rn);
                }
                End_Material(rn);
            }
            End_Shader(rn);
        }
        End_ShaderLayout(rn);
    }
}