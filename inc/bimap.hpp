//
// Copyright (c) 2021 Masashi Fujita All rights reserved.
//

#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

template<typename K_, typename V_>
class BidirectionalMap {
public:
    using self_t = BidirectionalMap<K_, V_>;

    using key_type    = K_;
    using mapped_type = V_;
    using value_type  = std::pair<const K_, const V_>;

private:
    std::vector<value_type>       items_;
    mutable std::vector<uint32_t> keys_;
    mutable std::vector<uint32_t> values_;
    mutable bool                  dehydrated_ = false;

public:
    BidirectionalMap () = default;

    void                 clear () noexcept { items_.clear (); }
    [[nodiscard]] bool   empty () const noexcept { return items_.empty (); }
    [[nodiscard]] size_t size () const noexcept { return items_.size (); }

    self_t &add (value_type &&item) {
        if (dehydrated_) {
            throw std::runtime_error {"attempt to modify the dehydrated instance"};
        }
        items_.template emplace_back (std::move (item));
        return *this;
    }

    self_t &add (const value_type &item) { return this->add (value_type {item}); }

    std::optional<std::reference_wrapper<const V_>> find_value (const K_ &key) const {
        if (! dehydrated_) {
            dehydrate ();
        }
        int64_t left  = -1;
        int64_t right = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (key <= items_[keys_[mid]].first) {
                right = mid;
            }
            else {
                left = mid;
            }
        }
        if (key == items_[keys_[right]].first) {
            return {std::cref (items_[keys_[right]].second)};
        }
        return {};
    }

    std::optional<std::reference_wrapper<const V_>> find_key (const V_ &value) const {
        if (! dehydrated_) {
            dehydrate ();
        }
        int64_t left  = -1;
        int64_t right = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (value <= items_[values_[mid]].second) {
                right = mid;
            }
            else {
                left = mid;
            }
        }
        if (value == items_[values_[right]].second) {
            return {std::cref (items_[values_[right]].first)};
        }
        return {};
    }

    void dehydrate () const {
        if (dehydrated_) {
            return;
        }
        keys_.resize (items_.size ());
        values_.resize (items_.size ());
        for (size_t i = 0; i < items_.size (); ++i) {
            keys_[i]   = i;
            values_[i] = i;
        }
        std::sort (keys_.begin (), keys_.end (), [this] (auto const &a, auto const &b) -> bool {
            return this->items_[a].first < this->items_[b].first;
        });
        std::sort (values_.begin (), values_.end (), [this] (auto const &a, auto const &b) -> bool {
            return this->items_[a].second < this->items_[b].second;
        });
        dehydrated_ = true;
    }

    bool dehydrated () const noexcept { return dehydrated_; }

    void hydrate () const { dehydrated_ = false; }
};
