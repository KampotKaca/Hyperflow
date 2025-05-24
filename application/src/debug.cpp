#include "debug.h"
#include "application.h"
#include "resources/renderpasses.h"

#define RYML_USE_CUSTOM_ALLOCATOR
#define RYML_STD_STRING
#include <ryml.hpp>
#include <ryml_std.hpp>

namespace app
{
    struct DebugInfo
    {
        uint32_t count = 0;
        uint32_t reqCount = 0;
        int32_t lastReq = -1;
        hf::Ref<hf::Window> wn{};
    };

    static DebugInfo DEBUG_INFO{};
    const std::string DEBUG_WINDOW_NAMES[5] =
    {
        "N1",
        "N2",
        "N3",
        "N4",
        "N5",
    };

    static hf::RenderPass CreateSubWindowRenderPass(const hf::Ref<hf::Renderer>& rn)
    {
        hf::renderpass::Bind(rn, APP_RENDER_PASSES.mainPresentPass);
        return APP_RENDER_PASSES.mainPresentPass;
    }

    void DebugDefine()
    {

    }

    void DebugLoad()
    {
        uint32_t count = 0;
        // Parse YAML code in place, potentially mutating the buffer:
        char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
        ryml::Tree tree = ryml::parse_in_place(yml_buf);
        ryml::NodeRef wroot = tree.rootref();

        // ryml has a two-level API:
        //
        // The lower level index API is based on the indices of nodes,
        // where the node's id is the node's position in the tree's data
        // array. This API is very efficient, but somewhat difficult to use:
        size_t root_id = tree.root_id();
        size_t bar_id = tree.find_child(root_id, "bar"); // need to get the index right
        count += !tree.is_map(root_id); // all of the index methods are in the tree
        count += !tree.is_seq(bar_id);  // ... and receive the subject index

        // The node API is a lightweight abstraction sitting on top of the
        // index API, but offering a much more convenient interaction:
        ryml::ConstNodeRef root = tree.rootref();  // a const node reference
        ryml::ConstNodeRef bar = tree["bar"];
        count += !root.is_map();
        count += !bar.is_seq();

        // The resulting tree stores only string views to the YAML source buffer.
        count += root["foo"].val() != "1";
        count += root["foo"].key().str != yml_buf + 1;
        count += bar[0].val() != "2";
        count += root["john"].val() != "doe";

        //------------------------------------------------------------------
        // To get actual values, you need to deserialize the nodes.
        // Deserializing: use operator>>
        {
            int foo = 0, bar0 = 0, bar1 = 0;
            std::string john_str;
            std::string bar_str;
            root["foo"] >> foo;
            root["bar"][0] >> bar0;
            root["bar"][1] >> bar1;
            root["john"] >> john_str; // requires from_chars(std::string). see API doc.
            root["bar"] >> ryml::key(bar_str); // to deserialize the key, use the tag function ryml::key()
            count += foo != 1;
            count += bar0 != 2;
            count += bar1 != 3;
            count += john_str != "doe";
            count += bar_str != "bar";
        }

        //------------------------------------------------------------------
        // To modify existing nodes, use operator= or operator<<.

        // operator= assigns an existing string to the receiving node.
        // The contents are NOT copied, and this pointer will be in effect
        // until the tree goes out of scope! So BEWARE to only assign from
        // strings outliving the tree.
        wroot["foo"] = "says you";
        wroot["bar"][0] = "-2";
        wroot["bar"][1] = "-3";
        wroot["john"] = "ron";
        // Now the tree is _pointing_ at the memory of the strings above.
        // In this case it is OK because those are static strings and will
        // outlive the tree.
        count += root["foo"].val() != "says you";
        count += root["bar"][0].val() != "-2";
        count += root["bar"][1].val() != "-3";
        count += root["john"].val() != "ron";
        // But WATCHOUT: do not assign from temporary objects:
        // {
        //     std::string crash("will dangle");
        //     root["john"] = ryml::to_csubstr(crash);
        // }
        // CHECK(root["john"] == "dangling"); // CRASH! the string was deallocated

        // operator<< first serializes the input to the tree's arena, then
        // assigns the serialized string to the receiving node. This avoids
        // constraints with the lifetime, since the arena lives with the tree.
        count += !tree.arena().empty();
        wroot["foo"] << "says who";  // requires to_chars(). see serialization samples below.
        wroot["bar"][0] << 20;
        wroot["bar"][1] << 30;
        wroot["john"] << "deere";
        count += root["foo"].val() != "says who";
        count += root["bar"][0].val() != "20";
        count += root["bar"][1].val() != "30";
        count += root["john"].val() != "deere";
        count += tree.arena() != "says who2030deere"; // the result of serializations to the tree arena


        //------------------------------------------------------------------
        // Adding new nodes:

        // adding a keyval node to a map:
        count += root.num_children() != 5;
        wroot["newkeyval"] = "shiny and new"; // using these strings
        wroot.append_child() << ryml::key("newkeyval (serialized)") << "shiny and new (serialized)"; // serializes and assigns the serialization
        count += root.num_children() != 7;
        count += root["newkeyval"].key() != "newkeyval";
        count += root["newkeyval"].val() != "shiny and new";
        count += root["newkeyval (serialized)"].key() != "newkeyval (serialized)";
        count += root["newkeyval (serialized)"].val() != "shiny and new (serialized)";


        //------------------------------------------------------------------
        // Emitting:

        ryml::csubstr expected_result = R"(foo: says who
        bar:
        - 20
        - 30
        - oh so nice
        - oh so nice (serialized)
        john: in_scope
        float: 2.4
        digits: 2.400000
        newkeyval: shiny and new
        newkeyval (serialized): shiny and new (serialized)
        newseq: []
        newseq (serialized): []
        newmap: {}
        newmap (serialized): {}
        I am something: indeed
        )";

        // emit to a FILE*
        ryml::emit_yaml(tree, stdout);
        auto stream_result = ryml::emitrs_yaml<std::string>(tree);
        // emit to a buffer:
        auto str_result = ryml::emitrs_yaml<std::string>(tree);
        // can emit to any given buffer:
        char buf[1024];
        ryml::csubstr buf_result = ryml::emit_yaml(tree, buf);
        // now check
        count += buf_result != expected_result;
        count += str_result != expected_result;
        count += stream_result != expected_result;

        //------------------------------------------------------------------
        // UTF8
        ryml::Tree langs = ryml::parse_in_arena(R"(
        en: Planet (Gas)
        fr: Planète (Gazeuse)
        ru: Планета (Газ)
        ja: 惑星（ガス）
        zh: 行星（气体）
        # UTF8 decoding only happens in double-quoted strings,
        # as per the YAML standard
        decode this: "\u263A \xE2\x98\xBA"
        and this as well: "\u2705 \U0001D11E"
        not decoded: '\u263A \xE2\x98\xBA'
        neither this: '\u2705 \U0001D11E'
        )");
        // in-place UTF8 just works:
        count += langs["en"].val() != "Planet (Gas)";
        count += langs["fr"].val() != "Planète (Gazeuse)";
        count += langs["ru"].val() != "Планета (Газ)";
        count += langs["ja"].val() != "惑星（ガス）";
        count += langs["zh"].val() != "行星（气体）";
        // and \x \u \U codepoints are decoded, but only when they appear
        // inside double-quoted strings, as dictated by the YAML
        // standard:
        count += langs["decode this"].val() != "☺ ☺";
        count += langs["and this as well"].val() != "✅ 𝄞";
        count += langs["not decoded"].val() != "\\u263A \\xE2\\x98\\xBA";
        count += langs["neither this"].val() != "\\u2705 \\U0001D11E";


        //------------------------------------------------------------------
        // Getting the location of nodes in the source:
        //
        // Location tracking is opt-in:
        // c4::yml::Tree tree2;
        // c4::yml::ParserOptions opts;
        // c4::yml::EventHandlerTree handler(&tree, 0);
        // c4::yml::ParseEngine parser(&handler, opts);
        //
        // // ... and use it when querying
        // ryml::Location loc = parser.location(tree2["bar"][1]);
        // count += !parser.location_contents(loc).begins_with("30");
        // count += loc.line != 3u;
        // count += loc.col != 4u;
        LOG_WARN("YAML Debug count: %i", count);
    }

    void DebugStart()
    {
        DEBUG_INFO.wn = hf::GetMainWindow();
        hf::time::SetTargetFrameRate(-1);
        DEBUG_INFO.count = 0;
    }

    void DebugUpdate()
    {
        if (hf::input::IsDown(hf::Key::A))
        {
        	hf::WindowCreationInfo data =
        	{
        		.title = DEBUG_WINDOW_NAMES[DEBUG_INFO.count % 5],
        		.flags = hf::WindowFlags::Default,
        		.style = hf::WindowStyle::Default,
        		.position = { 100, 100 },
        		.size = { 200, 200 },
        		.vsyncMode = hf::VsyncMode::Relaxed,
        		.rnEventInfo =
        		{
        			.onPassCreationCallback = CreateSubWindowRenderPass,
        			.onPreRenderCallback = AppPreRender,
        			.onRenderCallback = AppRender
        		},
        	};
        	DEBUG_INFO.wn = hf::window::Open(data, nullptr);
        	DEBUG_INFO.count++;
        }

        if (hf::input::IsDown(hf::Key::Space) && DEBUG_INFO.wn)
        {
            hf::window::Close(DEBUG_INFO.wn);
        }

        if (hf::input::IsDown(hf::Key::K))
        {
            hf::window::SetVSyncMode(DEBUG_INFO.wn, (hf::VsyncMode)(((uint32_t)hf::window::GetVSyncMode(DEBUG_INFO.wn) + 1) % (uint32_t)hf::VsyncMode::Count));
        }

        // auto delta = hf::input::GetScrollDelta();
        // if (delta != hf::vec2(0, 0)) LOG_INFO("Scroll (X: %f, Y: %f)", delta.x, delta.y);
        //
        // auto mDelta = hf::input::GetPointerDelta();
        // if (mDelta != hf::ivec2(0, 0)) LOG_INFO("Move: (X: %i, Y: %i)", mDelta.x, mDelta.y);

        auto cReq = (int32_t)(hf::time::GetTimePassed() / 0.2);
        if (cReq != DEBUG_INFO.reqCount)
        {
            // std::ostringstream oss;
            // oss << "[Hyperflow] " << hf::time::GetFrameRate();
            // oss << "[Hyperflow] " << hf::Time::GetTimePassed();
            std::string str = std::string("[Hyperflow] ") + std::to_string(hf::time::GetFrameRate());
            hf::window::SetTitle(hf::GetMainWindow(), str);
            DEBUG_INFO.reqCount = cReq;
        }

        if (hf::input::IsDown(hf::Key::T))
        {
            switch (hf::renderer::GetApiType())
            {
                case hf::RenderingApiType::None:
                    break;
                case hf::RenderingApiType::Vulkan:
                    hf::renderer::ChangeApi(hf::RenderingApiType::Direct3D);
                    break;
                case hf::RenderingApiType::Direct3D:
                    hf::renderer::ChangeApi(hf::RenderingApiType::Vulkan);
                    break;
            }
        }
    }

    void DebugQuit()
    {

    }

    void DebugPreRender(const hf::Ref<hf::Renderer>& rn)
    {

    }

    void DebugRender(const hf::Ref<hf::Renderer>& rn)
    {

    }
}

void* ryml_alloc(size_t size, void* user)
{
    return hf::utils::Allocate(size);
}

void ryml_free(void* p, void* user)
{
    hf::utils::Deallocate(p);
}
