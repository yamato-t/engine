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
    ~SharedSpinLock() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	排他ロック ( unique_lock ) を取得する
     */
    void lock() noexcept {
        int32_t e = 0;
        while (!state_.compare_exchange_weak(e, -1)) {
            e = 0;
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
        int32_t e = 0;
        int32_t d = e + 1;

        while (!state_.compare_exchange_weak(e, d)) {
            if (e < 0) {
                // 排他ロック中なのでリセット
                e = 0;
            }
            d = e + 1;
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
