#pragma once

#include <tuple>
#include <string>

template <typename... Containers>
class indexedZipperIterator
{
    public:
        using valueType = std::tuple<size_t, typename Containers::referenceType...>;
        using reference = valueType;
        using pointer = void;
        using differenceType = std::ptrdiff_t;
        using iteratorCategory = std::input_iterator_tag;

        indexedZipperIterator(size_t idx, size_t max, Containers&... containers)
            : _idx(idx), _max(max), _containers(std::tie(containers...))
        {
            if (!allSet() &&_idx < _max) {
                ++(*this);
            }
        }

        indexedZipperIterator &operator++()
        {
            ++_idx;
            while (_idx < _max &&!allSet()){
                ++_idx;
            }
            return *this;
        }

        indexedZipperIterator operator++(int)
        {
            indexedZipperIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        reference operator*() const
        {
            return dereference();
        }

        friend bool operator==(const indexedZipperIterator &lhs, const indexedZipperIterator &rhs)
        {
            return lhs._idx == rhs._idx;
        }

        friend bool operator!=(const indexedZipperIterator &lhs, const indexedZipperIterator &rhs)
        {
            return !(lhs == rhs);
        }

    private:
        bool allSet() const
        {
            return std::apply([this](const Containers&... containers)
            {
                return (... && (_idx < containers.size() && containers[_idx].has_value()));
            }, _containers);
        }

        reference dereference() const
        {
            return std::apply([this](Containers&... containers) -> reference
            {
                return std::tuple_cat(std::make_tuple(_idx), std::tie(containers[_idx]...));
            }, _containers);
        }

        size_t _idx;
        size_t _max;
        std::tuple<Containers&...> _containers;
};

template <typename... Containers>
class indexedZipper
{
    public:
        using iterator = indexedZipperIterator<Containers...>;

        indexedZipper(Containers&... containers)
            : _containers(std::tie(containers...)), _maxSize(_computeMaxSize(containers...)) {}

        iterator begin()
        {
            return iterator(0, _maxSize, std::get<Containers&>(_containers)...);
        }

        iterator end()
        {
            return iterator(_maxSize, _maxSize, std::get<Containers&>(_containers)...);
        }

    private:
        static size_t _computeMaxSize(const Containers&... containers)
        {
            return std::max({containers.size()...});
        }

        std::tuple<Containers&...> _containers;
        size_t _maxSize;
};
