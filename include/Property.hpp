#pragma once

#include <concepts>
#include <memory>

// Property for Movable Class
template <typename T>
concept not_pointer = std::negation_v<std::is_pointer<T>>;

template <not_pointer value_type, bool = std::copyable<value_type>, bool = std::movable<value_type>>
class Property final {
  public:
    // Copy Constructor
    Property(const Property &o) : value(std::make_unique<value_type>(*o.value)) {}
    Property(const value_type &t) : value(std::make_unique<value_type>(t)) {}
    // Move Constructor
    Property(Property &&o) : value(std::move(o.value)) {}
    Property(value_type &&t) : value(std::make_unique<value_type>(std::move(t))) {}

    // Destructor
    ~Property() = default;

    // Getter
    inline value_type &operator()() { return *value; }
    inline value_type &operator()(const value_type &t) { return *value = t; }
    inline value_type &operator()(value_type &&t) { return *value = std::move(t); }
    inline operator value_type() const { return *value; }
    // Setter
    inline void operator=(const Property &o) { *value = *o.value; }
    inline void operator=(const value_type &t) { *value = t; }
    inline void operator=(Property &&o) { value = std::move(o.value); }
    inline void operator=(value_type &&t) { *value = std::move(t); }

  private:
    std::unique_ptr<value_type> value;
};

// Property for Copy-Only Class
template <not_pointer value_type>
class Property<value_type, true, false> final {
  public:
    // Copy Constructor
    Property(const Property &o) : value(std::make_unique<value_type>(*o.value)) {}
    Property(const value_type &t) : value(std::make_unique<value_type>(t)) {}
    // Move Constructor
    Property(Property &&o) : value(std::make_unique<value_type>(o.value)) {}
    Property(value_type &&t) : value(std::make_unique<value_type>(std::move(t))) {}

    // Destructor
    ~Property() = default;

    // Getter
    inline value_type &operator()() { return *value; }
    inline value_type &operator()(const value_type &t) { return *value = t; }
    inline value_type &operator()(value_type &&t) { return *value = t; }
    inline operator value_type() const { return *value; }
    // Setter
    inline void operator=(const Property &o) { *value = *o.value; }
    inline void operator=(const value_type &t) { *value = t; }
    inline void operator=(Property &&o) { value = std::move(o.value); }
    inline void operator=(value_type &&t) { *value = t; }

  private:
    std::unique_ptr<value_type> value;
};

// Property for Move-Only Class
template <not_pointer value_type>
class Property<value_type, false, true> final {
  public:
    // Copy Constructor
    Property(const Property &o) = delete;
    Property(const value_type &t) = delete;
    // Move Constructor
    Property(Property &&o) : value(std::move(o.value)) {}
    Property(value_type &&t) : value(std::make_unique<value_type>(std::move(t))) {}

    // Destructor
    ~Property() = default;

    // Getter
    inline value_type &operator()() { return *value; }
    inline value_type &operator()(const value_type &t) { return *value = t; }
    inline value_type &operator()(value_type &&t) { return *value = std::move(t); }
    inline operator value_type() const { return *value; }
    // Setter
    inline void operator=(const Property &o) = delete;
    inline void operator=(const value_type &t) = delete;
    inline void operator=(Property &&o) { value = std::move(o.value); }
    inline void operator=(value_type &&t) { *value = std::move(t); }

  private:
    std::unique_ptr<value_type> value;
};

// Property for Identity Class
template <not_pointer value_type>
class Property<value_type, false, false> final {
  public:
    Property(Property &&other): value(std::move(other.value)){}

    // Destructor
    ~Property() = default;

    // Getter
    inline value_type &operator()() { return *value; }
    inline value_type &operator()(const value_type &t) { return *value = t; }
    inline value_type &operator()(value_type &&t) { return value = std::make_unique<value_type>(t); }
    // Setter
    inline void operator=(Property &&o) { value = std::move(o.value); }

  private:
    std::unique_ptr<value_type> value;
};
