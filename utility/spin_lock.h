#pragma once

#include <atomic>

#include "utility/noncopyable.h"

namespace utility {

//---------------------------------------------------------------------------------
/**
 * @brief
 * スピンロック同期オブジェクト
 */
class SharedSpinLock final : Noncopyable {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンストラクタ
     */
    SharedSpinLock() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デストラクタ
     */
    ~SharedSpinLock() = delete;

    //---------------------------------------------------------------------------------
    /**
     * @brief	排他ロック ( unique_lock ) を取得する
     */
    void lock() noexcept {
        int32_t expected = 0;
        while (!state_.compare_exchange_weak(expected, -1)) {
            _mm_pause();
        }
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	排他ロック ( unique_lock ) を開放する
     */
    void unlock() noexcept {
        state_.store(0);
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	共有ロック ( shared_lock ) を取得する
     */
    void lockShared() noexcept {
        int32_t expected = state_.load();
        while ((expected < 0) || !state_.compare_exchange_weak(expected, expected + 1)) {
            _mm_pause();
        }
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	共有ロック ( shared_lock ) を開放する
     */
    void unlockShared() noexcept {
        state_.fetch_sub(1);
    }


private:
    std::atomic<int32_t> state_{};
};
}  // namespace utility
