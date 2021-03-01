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

template<typename K_, typename V_, typename KeyComparator_ = std::less<K_>, typename ValueComparator_ = std::less<V_>>
class BidirectionalMap {
public:
    using self_t = BidirectionalMap<K_, V_, KeyComparator_, ValueComparator_>;

    using key_type      = K_;
    using mapped_type   = V_;
    using value_type    = std::pair<const K_, const V_>;
    using key_compare   = KeyComparator_;
    using value_compare = ValueComparator_;

private:
    template<typename, typename, typename = void>
    struct is_transparent : std::false_type {};
    template<typename T_, typename T2_>
    struct is_transparent<T_, T2_, std::void_t<typename T_::is_transparent>> : std::true_type {};

private:
    std::vector<value_type>             items_;
    mutable std::vector<uint32_t>       keys_;
    mutable std::vector<uint32_t>       values_;
    mutable bool                        dehydrated_ = false;
    [[no_unique_address]] key_compare   cmp_key_    = KeyComparator_ {};
    [[no_unique_address]] value_compare cmp_value_  = ValueComparator_ {};

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
            if (cmp_key_ (items_[keys_[mid]].first, key)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        auto const &k = items_[keys_[right]].first;
        if (cmp_key_ (key, k) || cmp_key_ (k, key)) {
            return {};
        }
        return {std::cref (items_[keys_[right]].second)};
    }

    template<typename U_>
    std::enable_if_t<is_transparent<key_compare, U_>::value, std::optional<std::reference_wrapper<const V_>>> find_value (const U_ &key) const {
        if (! dehydrated_) {
            dehydrate ();
        }
        int64_t left  = -1;
        int64_t right = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (cmp_key_ (items_[keys_[mid]].first, key)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        auto const &k = items_[keys_[right]].first;
        if (cmp_key_ (key, k) || cmp_key_ (k, key)) {
            return {};
        }
        return {std::cref (items_[keys_[right]].second)};
    }

    std::optional<std::reference_wrapper<const K_>> find_key (const V_ &value) const {
        if (! dehydrated_) {
            dehydrate ();
        }
        int64_t left  = -1;
        int64_t right = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (cmp_value_ (items_[values_[mid]].second, value)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        auto const &v = items_[values_[right]].second;
        if (cmp_value_ (value, v) || cmp_value_ (v, value)) {
            return {};
        }
        return {std::cref (items_[values_[right]].first)};
    }

    template<typename U_>
    std::enable_if_t<is_transparent<value_compare, U_>::value, std::optional<std::reference_wrapper<const K_>>>
    find_key (const U_ &value) const {
        if (! dehydrated_) {
            dehydrate ();
        }
        int64_t left  = -1;
        int64_t right = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (cmp_value_ (items_[values_[mid]].second, value)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        auto const &v = items_[values_[right]].second;
        if (cmp_value_ (value, v) || cmp_value_ (v, value)) {
            return {};
        }
        return {std::cref (items_[values_[right]].first)};
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
            return cmp_key_ (this->items_[a].first, this->items_[b].first);
        });
        std::sort (values_.begin (), values_.end (), [this] (auto const &a, auto const &b) -> bool {
            return cmp_value_ (this->items_[a].second, this->items_[b].second);
        });
        dehydrated_ = true;
    }

    bool dehydrated () const noexcept { return dehydrated_; }

    void hydrate () const { dehydrated_ = false; }
};
