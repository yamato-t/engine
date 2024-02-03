#include "dx12/descriptor_heap.h"
#include <format>

namespace dx12 {

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタヒープを生成する
 * @param	type			ヒープが管理する対象の種類
 * @param	maxNum			登録最大数
 * @param	flags			フラグ設定
 * @return	作成に成功した場合は true
 */
bool DescriptorHeap::create(uint32_t type, uint32_t maxNum, uint32_t flags) noexcept {
    // ディスクリプタヒープ作成
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type                       = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type);
    heapDesc.NumDescriptors             = maxNum;
    heapDesc.Flags                      = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(flags);

    auto res = dx12::Device::instance().device()->CreateDescriptorHeap(
        &heapDesc,
        IID_PPV_ARGS(heap_.GetAddressOf()));

    if (FAILED(res)) {
        ASSERT(false, "ディスクリプタヒープの作成に失敗");
        return false;
    }
    heap_->SetName(std::format(L"DescriptorHeap type:{} maxNum:{} flags:{} ", type, maxNum, flags).data());

    maxNum_ = maxNum;
    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ヒープにバッファを登録する
 * @param	num			登録数
 * @return	登録ハンドル
 */
DescriptorHeap::RegisterHandle DescriptorHeap::registerBuffer(uint32_t num) noexcept {
    const auto size = dx12::Device::instance().device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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
