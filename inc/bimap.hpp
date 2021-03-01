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
    std::vector<value_type> items_;
    /// Represents two ordered collections.
    /// To reduce allocation, we merge two collection into the single array.
    /// First half (0..<size()) contains sorted key indices.
    /// Second half (size ()..<(2 * size()) contains sorted value indices.
    mutable std::unique_ptr<uint32_t[]> indices_;  // NOLINT(modernize-avoid-c-arrays)
    [[no_unique_address]] key_compare   cmp_key_    = KeyComparator_ {};
    [[no_unique_address]] value_compare cmp_value_  = ValueComparator_ {};

public:
    BidirectionalMap () = default;

    void                 clear () noexcept { items_.clear (); }
    [[nodiscard]] bool   empty () const noexcept { return items_.empty (); }
    [[nodiscard]] size_t size () const noexcept { return items_.size (); }

    self_t &add (value_type &&item) {
        if (dehydrated ()) {
            throw std::runtime_error {"attempt to modify the dehydrated instance"};
        }
        items_.template emplace_back (std::move (item));
        return *this;
    }

    self_t &add (const value_type &item) { return this->add (value_type {item}); }

    std::optional<std::reference_wrapper<const V_>> find_value (const K_ &key) const {
        if (! dehydrated ()) {
            dehydrate ();
        }
        auto const *keys  = key_indices ();
        int64_t     left  = -1;
        int64_t     right = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (cmp_key_ (items_[keys[mid]].first, key)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        auto const &k = items_[keys[right]].first;
        if (cmp_key_ (key, k) || cmp_key_ (k, key)) {
            return {};
        }
        return {std::cref (items_[keys[right]].second)};
    }

    template<typename U_>
    std::enable_if_t<is_transparent<key_compare, U_>::value, std::optional<std::reference_wrapper<const V_>>> find_value (const U_ &key) const {
        if (! dehydrated ()) {
            dehydrate ();
        }
        auto const *keys  = key_indices ();
        int64_t     left  = -1;
        int64_t     right = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (cmp_key_ (items_[keys[mid]].first, key)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        auto const &k = items_[keys[right]].first;
        if (cmp_key_ (key, k) || cmp_key_ (k, key)) {
            return {};
        }
        return {std::cref (items_[keys[right]].second)};
    }

    std::optional<std::reference_wrapper<const K_>> find_key (const V_ &value) const {
        if (! dehydrated ()) {
            dehydrate ();
        }
        auto const *values = value_indices ();
        int64_t     left   = -1;
        int64_t     right  = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (cmp_value_ (items_[values[mid]].second, value)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        auto const &v = items_[values[right]].second;
        if (cmp_value_ (value, v) || cmp_value_ (v, value)) {
            return {};
        }
        return {std::cref (items_[values[right]].first)};
    }

    template<typename U_>
    std::enable_if_t<is_transparent<value_compare, U_>::value, std::optional<std::reference_wrapper<const K_>>>
    find_key (const U_ &value) const {
        if (! dehydrated ()) {
            dehydrate ();
        }
        auto const *values = value_indices ();
        int64_t     left   = -1;
        int64_t     right  = items_.size ();
        while (1 < (right - left)) {
            int64_t mid = left + (right - left) / 2u;
            if (cmp_value_ (items_[values[mid]].second, value)) {
                left = mid;
            }
            else {
                right = mid;
            }
        }
        auto const &v = items_[values[right]].second;
        if (cmp_value_ (value, v) || cmp_value_ (v, value)) {
            return {};
        }
        return {std::cref (items_[values[right]].first)};
    }

    void dehydrate () const {
        if (dehydrated()) {
            return;
        }
        indices_ = std::make_unique<uint32_t[]> (2u * size ());  // NOLINT(modernize-avoid-c-arrays)
        auto *k  = &indices_[0];
        auto *v  = &indices_[size ()];
        for (size_t i = 0; i < items_.size (); ++i) {
            k[i] = i;
            v[i] = i;
        }
        std::sort (&k[0], &k[size ()], [this] (auto const &a, auto const &b) -> bool {
            return cmp_key_ (this->items_[a].first, this->items_[b].first);
        });
        std::sort (&v[0], &v[size ()], [this] (auto const &a, auto const &b) -> bool {
            return cmp_value_ (this->items_[a].second, this->items_[b].second);
        });
    }

    [[nodiscard]] bool dehydrated () const noexcept { return indices_ != nullptr; }

    void hydrate () const {
        indices_.reset ();
    }

private:
    const uint32_t *key_indices () const { return &indices_[0]; }
    const uint32_t *value_indices () const { return &indices_[size ()]; }
};
