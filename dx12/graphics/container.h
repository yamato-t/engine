#pragma once

#include "utility/noncopyable.h"

namespace dx12::graphics {
//---------------------------------------------------------------------------------
/**
 * @brief
 * 描画オブジェクトのコンテナ
 */
template <class T>
class Container final : public utility::Noncopyable {
private:
    Container()                                 = default;
    Container(Container<T>&& c)                 = delete;
    Container& operator=(Container<T>&& c)      = delete;

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	インスタンスを取得する
     */
    static Container<T>& instance() noexcept {
        static Container<T> instance;
        return instance;
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	オブジェクトを登録する
     * @param	args	初期設定に利用する情報
     */
    template <class... Args>
    void registerObj(uint32_t key, Args&&... args) {
        if (container_.find(key) != container_.end()) {
            return;
        }

        auto o = std::make_unique<T>(std::forward<Args>(args)...);
        container_.emplace(key, std::move(o));
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	オブジェクトを取得する
     * @return	オブジェクトのポインタ
     */
    [[nodiscard]] T* get(uint32_t key) const noexcept {
        if (container_.find(key) == container_.end()) {
            return nullptr;
        }

        return container_.at(key).get();
    }

private:
    std::unordered_map<uint32_t, std::unique_ptr<T>> container_;
};
}  // namespace dx12::graphics
