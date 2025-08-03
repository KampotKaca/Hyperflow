#include "hyperflow.h"
#include "hinternal.h"

namespace hf::gridlines
{
    void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info)
    {
        Start_ShaderLayout(rn, inter::HF.staticResources.axisLinesShaderLayout);
        {
            primitives::BindGlobalUniformBuffer(rn);

            Start_Shader(rn, inter::HF.staticResources.axisLinesShader);
            {
                Start_Material(rn, inter::HF.staticResources.emptyMaterial);
                {
                    Start_Draw(rn);
                    {
                        DrawSet_PushConstant(rn, info);

                        DrawCallInfo drawCallInfo{};
                        drawCallInfo.pVertBuffers = &inter::HF.staticResources.quadBuffer;
                        drawCallInfo.bufferCount = 1;
                        drawCallInfo.instanceCount = 1;

                        DrawAdd_DrawCall(rn, drawCallInfo);
                    }
                    End_Draw(rn);
                }
                End_Material(rn);
            }
            End_Shader(rn);
        }
        End_ShaderLayout(rn);
    }
}