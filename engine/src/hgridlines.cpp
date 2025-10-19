#include "hyperflow.h"
#include "hinternal.h"

namespace hf::dp
{
    void Draw(const Ref<Renderer>& rn, const GridLinesInfo& info)
    {
        StartShaderLayout(rn, inter::HF.staticResources.shaderLayouts.axisLines);
        {
            BindGlobalUniformBuffer(rn);

            StartShader(rn, inter::HF.staticResources.shaders.axisLines);
            {
                StartMat(rn, nullptr);
                {
                    StartDrawGroup(rn);
                    {
                        DrawGroupSetPushConstant(rn, info);
                        DrawGroupAddDrawCall(rn, nullptr, inter::HF.staticResources.quadBuffer);
                    }
                    EndDrawGroup(rn);
                }
                EndMat(rn);
            }
            EndShader(rn);
        }
        EndShaderLayout(rn);
    }
}