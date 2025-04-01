#pragma once

#include <vector>
#include <memory>
#include <cstddef>
#include <optional>

namespace ECS 
{
    template <typename Component>
    class SparseArray
    {
        public:
            using valueType = std::optional<Component>;
            using referenceType = valueType&;
            using constReferenceType = const valueType&;
            using containerT = std::vector<valueType>;
            using sizeType = typename containerT::size_type;
            using iterator = typename containerT::iterator;
            using constIterator = typename containerT::const_iterator;

            SparseArray() = default;
            SparseArray(SparseArray const&) = default; // Copy constructor
            SparseArray(SparseArray&&) noexcept = default; // Move constructor
            ~SparseArray() = default;

            SparseArray& operator=(SparseArray const&) = default; // Copy assignment operator
            SparseArray& operator=(SparseArray&&) = default;      // Move assignment operator

            referenceType operator[](size_t idx);
            constReferenceType operator[](size_t idx) const;

            iterator begin();
            constIterator begin() const;
            constIterator cbegin() const;

            iterator end();
            constIterator end() const;
            constIterator cend() const;

            sizeType size() const;

            referenceType insertAt(sizeType pos, const Component &value);
            referenceType insertAt(sizeType pos, Component &&value);

            template <class... Params>
            referenceType emplaceAt(sizeType pos, Params&&... params);

            void erase(sizeType pos);

            sizeType getIndex(const valueType &value) const;

            bool has(sizeType pos) const;

        private:
            containerT _data;
    };

    template <typename Component>
    typename SparseArray<Component>::referenceType SparseArray<Component>::operator[](size_t idx)
    {
        if (idx >= _data.size())
            _data.resize(idx + 1);
        return _data[idx];
    }

    template <typename Component>
    typename SparseArray<Component>::constReferenceType SparseArray<Component>::operator[](size_t idx) const 
    {
        if (idx >= _data.size())
            throw std::out_of_range("Sparse array idx out of range.");
        return _data[idx];
    }

    template <typename Component>
    typename SparseArray<Component>::iterator SparseArray<Component>::begin()
    {
        return _data.begin();
    }

    template <typename Component>
    typename SparseArray<Component>::constIterator SparseArray<Component>::begin() const
    {
        return _data.begin();
    }

    template <typename Component>
    typename SparseArray<Component>::constIterator SparseArray<Component>::cbegin() const
    {
        return _data.cbegin();
    }

    template <typename Component>
    typename SparseArray<Component>::iterator SparseArray<Component>::end()
    {
        return _data.end();
    }

    template <typename Component>
    typename SparseArray<Component>::constIterator SparseArray<Component>::end() const
    {
        return _data.end();
    }

    template <typename Component>
    typename SparseArray<Component>::constIterator SparseArray<Component>::cend() const
    {
        return _data.cend();
    }

    template <typename Component>
    typename SparseArray<Component>::sizeType SparseArray<Component>::size() const
    {
        return _data.size();
    }

    template <typename Component>
    typename SparseArray<Component>::referenceType SparseArray<Component>::insertAt(sizeType pos, const Component &value)
    {
        if (pos >= _data.size())
            _data.resize(pos + 1);
        _data[pos] = value;
        return _data[pos];
    }

    template <typename Component>
    typename SparseArray<Component>::referenceType SparseArray<Component>::insertAt(sizeType pos, Component &&value)
    {
        if (pos >= _data.size())
            _data.resize(pos + 1);
        _data[pos] = std::move(value);
        return _data[pos];
    }

    template <typename Component>
    template <class ... Params>
    typename SparseArray<Component>::referenceType SparseArray<Component>::emplaceAt(sizeType pos, Params&&... params)
    {
        if (pos >= _data.size())
            _data.resize(pos + 1);
        _data[pos].emplace(std::forward<Params>(params)...);
        return _data[pos];      
    }

    template <typename Component>
    void SparseArray<Component>::erase(sizeType pos)
    {
        if (pos < _data.size())
            _data[pos].reset();
    }

    template <typename Component>
    typename SparseArray<Component>::sizeType SparseArray<Component>::getIndex(const valueType &value) const
    {
        for (sizeType i = 0; i < _data.size(); ++i) {
            if (_data[i] == value)
                return i;
        }
        throw std::invalid_argument("Value not found in sparse array.");
    }

    template <typename Component>
    bool SparseArray<Component>::has(sizeType pos) const
    {
        if (pos >= _data.size()) {
            return false;
        }
        return _data[pos].has_value();
    }
}
