#include "dx12/descriptor_heap.h"
#include <format>

namespace dx12 {

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタヒープを生成する
 * @param	type			ヒープが管理する対象の種類
 * @param	capacity		最大管理数
 * @param	flags			フラグ設定
 * @return	作成に成功した場合は true
 */
bool DescriptorHeap::create(uint32_t type, uint32_t capacity, uint32_t flags) noexcept {
    // ディスクリプタヒープ作成
    desc_ = {};

    desc_.Type           = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type);
    desc_.NumDescriptors = capacity;
    desc_.Flags          = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(flags);

    auto res = dx12::Device::instance().device()->CreateDescriptorHeap(
        &desc_,
        IID_PPV_ARGS(heap_.GetAddressOf()));

    if (FAILED(res)) {
        ASSERT(false, "ディスクリプタヒープの作成に失敗");
        return false;
    }
    heap_->SetName(std::format(L"DescriptorHeap type:{} capacity:{} flags:{} ", type, capacity, flags).data());

    capacity_ = capacity;
    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ヒープにバッファを登録する
 * @param	num			登録数
 * @return	登録ハンドル
 */
DescriptorHeap::RegisterHandle DescriptorHeap::registerBuffer(uint32_t num) noexcept {
    const auto size = dx12::Device::instance().device()->GetDescriptorHandleIncrementSize(desc_.Type);

    auto cpuHandle = heap_->GetCPUDescriptorHandleForHeapStart();
    auto gpuHandle = heap_->GetGPUDescriptorHandleForHeapStart();

    auto temp = currentIndex_;
    cpuHandle.ptr += (currentIndex_ * size);
    gpuHandle.ptr += (currentIndex_ * size);

    currentIndex_ += num;

    return {temp, cpuHandle.ptr, gpuHandle.ptr, size};
}

//---------------------------------------------------------------------------------
/**
 * @brief	ヒープをコマンドリストに設定する
 * @param	commandList				設定先のコマンドリスト
 */
void DescriptorHeap::setToCommandList(dx12::CommandList& commandList) noexcept {
    // ヒープの設定
    ID3D12DescriptorHeap* p[] = {heap_.Get()};
    commandList.get()->SetDescriptorHeaps(_countof(p), p);
}

}  // namespace dx12
