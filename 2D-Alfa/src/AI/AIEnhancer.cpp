#include "AIEnhancer.h"
#include <iostream>
#include <cassert>

AIEnhancer::AIEnhancer(const std::string& modelPath)
: env_(ORT_LOGGING_LEVEL_WARNING, "AIEnhancer"), session_(nullptr)
{
    session_options_.SetIntraOpNumThreads(1);
    session_options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);

    try {
        session_ = std::make_unique<Ort::Session>(env_, modelPath.c_str(), session_options_);
        std::cout << "[AIEnhancer] Model loaded: " << modelPath << std::endl;
    } catch (const Ort::Exception& e) {
        std::cerr << "[AIEnhancer] Failed to create session: " << e.what() << std::endl;
        session_.reset();
    }
}

AIEnhancer::~AIEnhancer() {
    // unique_ptr session_ zniszczy Ort::Session automatycznie
}

bool AIEnhancer::infer(const std::vector<float>& input, const std::vector<int64_t>& input_dims,
                       std::vector<float>& output, std::vector<int64_t>& output_dims)
{
    if(!session_) return false;
    Ort::MemoryInfo mem_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    // nazwy input/output
    size_t num_input_nodes = session_->GetInputCount();
    size_t num_output_nodes = session_->GetOutputCount();

    std::vector<const char*> input_node_names;
    std::vector<const char*> output_node_names;

    input_node_names.reserve(num_input_nodes);
    output_node_names.reserve(num_output_nodes);

    // pobierz nazwy inputów
    for (size_t i = 0; i < num_input_nodes; i++) {
        char* name = session_->GetInputName(i, allocator_);
        input_node_names.push_back(name);
    }
    for (size_t i = 0; i < num_output_nodes; i++) {
        char* name = session_->GetOutputName(i, allocator_);
        output_node_names.push_back(name);
    }

    // Tworzymy input tensor
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(mem_info,
                                                              const_cast<float*>(input.data()),
                                                              input.size(),
                                                              input_dims.data(),
                                                              input_dims.size());

    // wykonujemy inference
    try {
        auto output_tensors = session_->Run(Ort::RunOptions{nullptr},
                                            input_node_names.data(),
                                            &input_tensor,
                                            1,
                                            output_node_names.data(),
                                            num_output_nodes);

        assert(output_tensors.size() == num_output_nodes);

        // zakładamy pojedynczy output dla prostoty
        Ort::Value& out0 = output_tensors.front();
        Ort::TensorTypeAndShapeInfo outInfo = out0.GetTensorTypeAndShapeInfo();
        output_dims = outInfo.GetShape();

        size_t outCount = outInfo.GetElementCount();
        output.resize(outCount);

        float* outData = out0.GetTensorMutableData<float>();
        // kopiujemy dane
        std::copy(outData, outData + outCount, output.begin());

    } catch (const Ort::Exception& e) {
        std::cerr << "[AIEnhancer] Inference failed: " << e.what() << std::endl;
        // free names allocated by GetInputName/GetOutputName
        for (auto n : input_node_names) allocator_.Free(const_cast<char*>(n));
        for (auto n : output_node_names) allocator_.Free(const_cast<char*>(n));
        return false;
    }

    // free allocated names
    for (auto n : input_node_names) allocator_.Free(const_cast<char*>(n));
    for (auto n : output_node_names) allocator_.Free(const_cast<char*>(n));

    return true;
}
