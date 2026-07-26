#include <catch2/catch_test_macros.hpp>
#include <Core/LayerStack.h>
#include <Core/Layer.h>
#include <Core/Logger.h>
#include <vector>
#include <string>

using namespace Engine;

namespace {
    struct LayerLogFixture {
        Logger logger{ Logger::LogLevel::Info };
    };

    // Test double that records onAttach/onDetach calls so ordering can be asserted on,
    // and exposes its debug name (protected on Layer) for iteration-order checks.
    class RecordingLayer : public Layer {
    public:
        RecordingLayer(const std::string& name, std::vector<std::string>& record)
            : Layer(name), m_record(record) {
        }

        void onAttach() override { m_record.push_back(m_debugName + ":attach"); }
        void onDetach() override { m_record.push_back(m_debugName + ":detach"); }

        const std::string& name() const { return m_debugName; }

    private:
        std::vector<std::string>& m_record;
    };

    std::vector<std::string> namesInOrder(LayerStack& stack) {
        std::vector<std::string> names;
        for (Layer* layer : stack) {
            names.push_back(static_cast<RecordingLayer*>(layer)->name());
        }
        return names;
    }
}

TEST_CASE_METHOD(LayerLogFixture, "pushLayer and pushOverlay call onAttach immediately", "[LayerStack]") {
    std::vector<std::string> record;
    LayerStack stack{ LayerStackDesc{ {logger} } };

    stack.pushLayer(new RecordingLayer("Layer1", record));
    logger.log(Logger::LogLevel::Info, "Attach record after pushLayer(Layer1): [{}]", record.back());
    REQUIRE(record == std::vector<std::string>{"Layer1:attach"});

    stack.pushOverlay(new RecordingLayer("Overlay1", record));
    logger.log(Logger::LogLevel::Info, "Attach record after pushOverlay(Overlay1): [{}, {}]", record[0], record[1]);
    REQUIRE(record == std::vector<std::string>{"Layer1:attach", "Overlay1:attach"});
}

TEST_CASE_METHOD(LayerLogFixture, "pushLayer inserts before the overlay section, pushOverlay always appends after it", "[LayerStack]") {
    std::vector<std::string> record;
    LayerStack stack{ LayerStackDesc{ {logger} } };

    stack.pushLayer(new RecordingLayer("LayerA", record));
    stack.pushOverlay(new RecordingLayer("OverlayA", record));
    stack.pushLayer(new RecordingLayer("LayerB", record)); // must land after LayerA but before OverlayA

    auto order = namesInOrder(stack);
    logger.log(Logger::LogLevel::Info, "LayerStack order after pushLayer(A), pushOverlay(Overlay), pushLayer(B): [{}, {}, {}]",
        order[0], order[1], order[2]);

    REQUIRE(order == std::vector<std::string>{"LayerA", "LayerB", "OverlayA"});
}

TEST_CASE_METHOD(LayerLogFixture, "popLayer detaches and removes only the targeted layer, leaving others intact", "[LayerStack]") {
    std::vector<std::string> record;
    LayerStack stack{ LayerStackDesc{ {logger} } };

    auto* layerA = new RecordingLayer("LayerA", record);
    stack.pushLayer(layerA);
    stack.pushLayer(new RecordingLayer("LayerB", record));

    stack.popLayer(layerA);
    logger.log(Logger::LogLevel::Info, "Detach record after popLayer(LayerA): [{}]", record.back());
    REQUIRE(record.back() == "LayerA:detach");
    REQUIRE(namesInOrder(stack) == std::vector<std::string>{"LayerB"});

    delete layerA; // popLayer does not take ownership of the layer it removes
}

TEST_CASE_METHOD(LayerLogFixture, "popOverlay detaches and removes only the targeted overlay, leaving others intact", "[LayerStack]") {
    std::vector<std::string> record;
    LayerStack stack{ LayerStackDesc{ {logger} } };

    auto* overlayA = new RecordingLayer("OverlayA", record);
    stack.pushOverlay(overlayA);
    stack.pushOverlay(new RecordingLayer("OverlayB", record));

    stack.popOverlay(overlayA);
    logger.log(Logger::LogLevel::Info, "Detach record after popOverlay(OverlayA): [{}]", record.back());
    REQUIRE(record.back() == "OverlayA:detach");
    REQUIRE(namesInOrder(stack) == std::vector<std::string>{"OverlayB"});

    delete overlayA; // popOverlay does not take ownership of the overlay it removes
}

TEST_CASE_METHOD(LayerLogFixture, "LayerStack destructor detaches and frees every layer still owned when it goes out of scope", "[LayerStack]") {
    std::vector<std::string> record;
    {
        LayerStack stack{ LayerStackDesc{ {logger} } };
        stack.pushLayer(new RecordingLayer("LayerA", record));
        stack.pushOverlay(new RecordingLayer("OverlayA", record));
        // record already holds the two attach events pushLayer/pushOverlay just triggered.
    } // ~LayerStack runs here: detaches + deletes both, in insertion order, appending two more entries

    logger.log(Logger::LogLevel::Info, "Full record after push+destruction: [{}, {}, {}, {}]",
        record[0], record[1], record[2], record[3]);
    REQUIRE(record.size() == 4);
    REQUIRE(record[0] == "LayerA:attach");
    REQUIRE(record[1] == "OverlayA:attach");
    REQUIRE(record[2] == "LayerA:detach");
    REQUIRE(record[3] == "OverlayA:detach");
}
