#include "hyperflow.h"
#include "hinternal.h"

namespace hf::gridlines
{
    void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info)
    {
        dp::StartShaderLayout(rn, inter::HF.staticResources.shaderLayouts.axisLines);
        {
            primitives::BindGlobalUniformBuffer(rn);

            dp::StartShader(rn, inter::HF.staticResources.shaders.axisLines);
            {
                dp::StartMat(rn, nullptr);
                {
                    dp::StartDrawGroup(rn);
                    {
                        dp::DrawGroupSetPushConstant(rn, info);
                        dp::DrawGroupAddDrawCall(rn, nullptr, inter::HF.staticResources.quadBuffer);
                    }
                    dp::EndDrawGroup(rn);
                }
                dp::EndMat(rn);
            }
            dp::EndShader(rn);
        }
        dp::EndShaderLayout(rn);
    }
}