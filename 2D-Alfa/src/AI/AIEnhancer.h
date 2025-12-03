#pragma once
#include <string>
#include <vector>
#include <memory>
#include <onnxruntime_cxx_api.h>

class AIEnhancer {
public:
    // modelPath: ścieżka do pliku .onnx (lub .ort jeśli używasz ORT format)
    AIEnhancer(const std::string& modelPath);
    ~AIEnhancer();

    // wejście: float buffer (np. CHW) oraz dims np. {1, C, H, W}
    // output zostanie wypełnione floatami zgodnie z output tensor shape
    bool infer(const std::vector<float>& input, const std::vector<int64_t>& input_dims,
               std::vector<float>& output, std::vector<int64_t>& output_dims);

    bool loaded() const { return session_ != nullptr; }

private:
    Ort::Env env_;
    Ort::SessionOptions session_options_;
    std::unique_ptr<Ort::Session> session_;
    Ort::AllocatorWithDefaultOptions allocator_;
};
