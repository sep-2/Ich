#include <memory>

#include "System/Renderer/Renderer.h"

std::shared_ptr<Renderer> Renderer::instance_ = nullptr;

Renderer::Renderer()
  : render_list_() {
}

Renderer::~Renderer() {
}

void Renderer::Init() {
  decltype(render_list_)().swap(render_list_);
}

void Renderer::Push(int priority, std::shared_ptr<Task> image) {
  if (!image) return;
  render_list_.push(std::make_pair(priority, std::move(image)));
}

void Renderer::Update(float delta_time) {
}

void Renderer::Render() {
  while (!render_list_.empty()) {
    auto task = render_list_.top().second; // shared_ptr コピー
    if (task) {
      task->Render();
    }
    render_list_.pop();
  }
}

void Renderer::Delete() {
  decltype(render_list_)().swap(render_list_);
}
