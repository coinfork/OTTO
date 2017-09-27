#pragma once

#include <iterator>
#include <cmath>
#include <functional>
#include <type_traits>

namespace top1 {

  namespace detail {

    // Difference type

    template<typename Impl, typename T = void>
    struct difference_type {
      using type = std::ptrdiff_t;
    };

    template<typename Impl>
    struct difference_type<Impl, std::void_t<typename Impl::difference_type>> {
      using type = typename Impl::difference_type;
    };

    // Value type

    template<typename Impl, typename T = void>
    struct value_type {
      using type = typename Impl::value_type;
    };

    template<typename Impl>
    struct value_type<Impl,
      std::void_t<typename std::iterator_traits<Impl>::value_type>>
    {
      using type = typename std::iterator_traits<Impl>::value_type;
    };

    // Pointer type

    template<typename Impl, typename T = void>
    struct pointer {
      using type = typename value_type<Impl>::type*;
    };

    template<typename Impl>
    struct pointer<Impl, std::void_t<typename Impl::pointer>> {
      using type = typename Impl::pointer;
    };

    // Reference type

    template<typename Impl, typename T = void>
    struct reference {
      using type = typename value_type<Impl>::type&;
    };

    template<typename Impl>
    struct reference<Impl, std::void_t<typename Impl::reference>>
    {
      using type = typename Impl::reference;
    };

    // Iterator category

    template<typename Impl, typename T = void>
    struct iterator_category
    {
      using type = typename Impl::iterator_category;
    };

    template<typename Impl>
    struct iterator_category<Impl,
      std::void_t<typename std::iterator_traits<Impl>::iterator_category>>
    {
      using type = typename std::iterator_traits<Impl>::iterator_category;
    };
  }


  /*
   * Iterator Adaptor
   */
  
  template<typename Impl, typename Category = void>
  class iterator_adaptor_impl : public Impl {
  public:
    // For std::type_traits
    using difference_type = typename detail::difference_type<Impl>::type;
    using value_type = typename detail::value_type<Impl>::type;
    using pointer = typename detail::pointer<Impl>::type;
    using reference = typename detail::reference<Impl>::type;
    using iterator_category = typename detail::iterator_category<Impl>::type;

    /* Initialization */

    using Impl::Impl;

    template<typename... Args>
    iterator_adaptor_impl(Args&&... args)
      : Impl {std::forward<Args>(args)...} {}

    iterator_adaptor_impl(const iterator_adaptor_impl& r)
      : Impl {static_cast<Impl>(r)} {}

    iterator_adaptor_impl(iterator_adaptor_impl&& r)
      : Impl {static_cast<Impl>(std::move(r))} {}

    virtual ~iterator_adaptor_impl() = default;

    using Impl::operator=;

    /* Operators */

    // Increment (Any)

    iterator_adaptor_impl operator++()
    {
      auto old = *this;
      Impl::advance(1);
      return old;
    }

    iterator_adaptor_impl& operator++(int)
    {
      Impl::advance(1);
      return *this;
    }

    // Dereference (Any)
    decltype(auto) operator*()
    {
      return Impl::dereference();
    }

    decltype(auto) operator*() const
    {
      return Impl::dereference();
    }

    decltype(auto) operator->()
    {
      return Impl::dereference();
    }

    decltype(auto) operator->() const
    {
      return Impl::dereference();
    }

    // Comparison (Any)
    bool operator==(const iterator_adaptor_impl& r)
    {
      return Impl::equal(r);
    }

    bool operator!=(const iterator_adaptor_impl& r)
    {
      return !Impl::equal(r);
    }

  };

  template<typename Impl>
  class iterator_adaptor_impl<Impl, std::bidirectional_iterator_tag>
    : public iterator_adaptor_impl<Impl, void>
  {
    using Super = iterator_adaptor_impl<Impl, void>;
  public:
    using typename Super::difference_type;
    using typename Super::value_type;
    using typename Super::pointer;
    using typename Super::reference;
    using typename Super::iterator_category;

    template<typename... Args>
    iterator_adaptor_impl(Args&&... args)
      : Super {std::forward<Args>(args)...} {}

    iterator_adaptor_impl(const iterator_adaptor_impl&) = default;
    iterator_adaptor_impl(iterator_adaptor_impl&&) = default;

    using Super::operator=;

    using Super::operator++;
    using Super::operator*;
    using Super::operator->;
    using Super::operator==;
    using Super::operator!=;

    // Decrement (Bidirectional)

    iterator_adaptor_impl operator--()
    {
      auto old = *this;
      Impl::advance(-1);
      return old;
    }

    iterator_adaptor_impl& operator--(int)
    {
      Impl::advance(-1);
      return *this;
    }
  };

  template<typename Impl>
  class iterator_adaptor_impl<Impl, std::random_access_iterator_tag>
    : public iterator_adaptor_impl<Impl, std::bidirectional_iterator_tag>
  {
    using Super = iterator_adaptor_impl<Impl, std::bidirectional_iterator_tag>;
  public:
    using typename Super::difference_type;
    using typename Super::value_type;
    using typename Super::pointer;
    using typename Super::reference;
    using typename Super::iterator_category;

    template<typename... Args>
    iterator_adaptor_impl(Args&&... args)
      : Super {std::forward<Args>(args)...} {}

    iterator_adaptor_impl(const iterator_adaptor_impl&) = default;
    iterator_adaptor_impl(iterator_adaptor_impl&&) = default;

    using Super::operator=;

    using Super::operator++;
    using Super::operator*;
    using Super::operator->;
    using Super::operator==;
    using Super::operator!=;

    using Super::operator--;

    // Arithmetics (Random access)

    iterator_adaptor_impl operator+(difference_type d) const
    {
      iterator_adaptor_impl res {*this};
      res.Impl::advance(d);
      return res;
    }

    iterator_adaptor_impl operator-(difference_type d) const
    {
      iterator_adaptor_impl res {*this};
      res.Impl::advance(-d);
      return res;
    }

    difference_type operator-(const iterator_adaptor_impl& d) const
    {
      return Impl::difference(d);
    }

    // Inequality (Random access)

    bool operator<(const iterator_adaptor_impl& d) const
    {
      return Impl::difference(d) < 0;
    }

    bool operator>(const iterator_adaptor_impl& d) const
    {
      return Impl::difference(d) > 0;
    }

    bool operator<=(const iterator_adaptor_impl& d) const
    {
      return Impl::difference(d) <= 0;
    }

    bool operator>=(const iterator_adaptor_impl& d) const
    {
      return Impl::difference(d) >= 0;
    }

    // Compound assignment (Random access)

    iterator_adaptor_impl& operator+=(difference_type d)
    {
      Impl::advance(d);
      return *this;
    }

    iterator_adaptor_impl& operator-=(difference_type d)
    {
      Impl::advance(-d);
      return *this;
    }

    // Offset dereference (Random access)

    reference operator[](difference_type d)
    {
      return *(*this + d);
    }

  };

  ///
  /// Zero overhead wrapper to create iterators
  ///
  /// @Impl must define the following member functions:
  /// ```
  /// void advance(difference_type);
  /// reference dereference();
  /// bool equal(const Impl&);
  /// ```
  /// For random access iterators, it must also define
  /// ```
  /// std::ptrdiff_t difference(const Impl&);
  /// ```
  /// Other than those, it must define a copy constructor, and the member
  /// types `iterator_category` and `value_type`, Any other of the five
  /// `iterator_traits` types will also be used from `Impl` if avaliable
  ///
  template<typename Impl>
  using iterator_adaptor = iterator_adaptor_impl<Impl,
    typename detail::iterator_category<Impl>::type>;

  template<typename I, typename V = typename detail::value_type<I>::type>
  class FloatStepIterImpl {
  public:
    /*
     * Member types
     */

    using wrapped_type = I;

    // for `std::iterator_traits`
    using value_type = V;
    using iterator_category = typename detail::iterator_category<I>::type;

    /*
     * Initialization
     */

    /// Construct an iterator, pointing to `ptr`
    FloatStepIterImpl(const wrapped_type& iter, float step = 1.f)
      : iter {iter}, step {step}
    {}

    /// Copy constructor
    FloatStepIterImpl(const FloatStepIterImpl& r)
      : iter {r.iter},
        step {r.step},
        _error {r._error}
    {}

    /// Move constructor
    FloatStepIterImpl(FloatStepIterImpl&& r)
      : iter {std::move(r.iter)},
        step {std::move(r.step)},
        _error {std::move(r._error)}
    {}

    // Default assignment operator
    FloatStepIterImpl& operator=(const FloatStepIterImpl&) = default;

    // Default assignment operator
    FloatStepIterImpl& operator=(FloatStepIterImpl&&) = default;


    /* Iterator implementation */
  protected:

    /// Dereference the iterator
    ///
    /// Propagates to the dereference operator of <ptr>
    decltype(auto) dereference() {return *iter;}
    decltype(auto) dereference() const {return *iter;}

    /// Compare equal
    ///
    /// Requires members <ptr> and <_error> to be equal.
    /// Ignores <step>, as it has no effect on the dereferenced value.
    bool equal(const FloatStepIterImpl& r) const
    {
      return iter == r.iter && _error == r._error;
    }

    /// Get the real difference between this and `o`
    ///
    /// Takes the error values into account.
    std::ptrdiff_t difference(const FloatStepIterImpl& o) const
    {
      return (float(iter - o.iter) + (_error - o._error)) / step;
    }

    /// Increment this by `n`
    ///
    /// Guarrantied to be equal to calling <operator++> `n` times
    void advance(int n)
    {
      float intPart;
      _error = std::modf(_error + step * n, &intPart);
      if (_error < 0) {
        intPart -= 1;
        _error += 1;
      }
      std::advance(iter, intPart);
    }

    /* Member functions */
  public:

    /// Get a copy of the underlying pointer.
    wrapped_type data() const {
      return iter;
    }

    /// The inaccuracy of the element this iterator points to.
    ///
    /// While <step> is an integer, this value will be constant.
    /// Otherwise it is in the range `[0, 1)`, signifying the
    /// fractional part of the real index. I.e.
    /// `ptr + error() == real_index`.
    float error() const {
      return _error;
    }

    /*
     * Member Variables
     */

    /// The size of one step
    ///
    /// When incrementing, <ptr> will be effectively be incremented by
    /// this value.
    ///
    /// Changing this value does not invalidate any iterator, but be
    /// aware that an end iterator previously created using
    /// `*this + n` might no longer be reachable by incrementing this.
    /// If you need to change the iterator step while looping, prefer
    /// using `iter < last` over `iter != last`
    float step = 1.f;

  private:

    /// See getter, <error>
    float _error = 0.f;

    /// The underlying pointer
    ///
    /// For public, read only access, use <data>
    wrapped_type iter;
  };


  /// An iterator wrapper to iterate with a non-integer ratio
  ///
  /// This iterates through contiguous data, or simply increments an
  /// integer value, with floating point steps. It keeps track of the
  /// error, and corrects it while iterating.
  ///
  /// The most common use case is iterating over data at a non-integer
  /// rate ratio. In the TOP-1 it is used to read sound samples at a
  /// different rate than how they were recorded.
  ///
  /// When used to iterator through data, it is preferred to use
  /// `first < last` as the loop condition, as opposed to the
  /// conventional `first != last`. The need for doing this depends on
  /// the relationship between the `first` and `last` iterators. If
  /// one was created from the other, using <operator+> or
  /// <operator->, they are guarantied to be reachable from the other
  /// using <operator++> or <operator--> as applicable, as long as the
  /// <step> member variable on the mutating iterator is unchanged.
  ///
  /// `float_step_iterator` nearly models a random access iterator,
  /// missing only a few operations, that may be added in the future.
  /// but should not be used as such. It does however model a
  /// Bidirectional iterator.
  ///
  /// @I A random access iterator that will be wrapped.
  /// @V The value type.
  template<typename I, typename V = typename detail::value_type<I>::type>
  using float_step_iterator = iterator_adaptor<FloatStepIterImpl<I, V>>;

  /// Create a float_step_iterator
  template<typename I, typename V =
    typename detail::value_type<std::remove_reference_t<I>>::type>
  auto float_step(I&& iter, float step = 1)
  {
    return float_step_iterator<std::remove_reference_t<I>,
      V>{std::forward<I>(iter), step};
  }


  ///
  /// Generating iterator
  ///
  template<typename Generator>
  class GeneratingIterImpl {
  public:
    using value_type = std::invoke_result_t<Generator>;
    using iterator_category = std::input_iterator_tag;

    GeneratingIterImpl(Generator generator)
      : generator {generator}
    {}

    void advance(int n)
    {
      for (int i = 0; i < n; i++) {
        val = std::invoke(generator);
      }
    }

    value_type& dereference()
    {
      return val;
    }

    bool equal(const value_type& o) const
    {
      return o.val == val;
    }

    value_type val;
    Generator generator;
  };


  ///
  /// Generating iterator
  ///
  /// Supplied with a generator function, this iterator will generate a value
  /// each time its called.
  template<typename Generator>
  using generating_iterator = iterator_adaptor<GeneratingIterImpl<Generator>>;

  /// Create a generating iterator
  template<typename Generator>
  generating_iterator<Generator> generator(Generator&& gen) {
    return generating_iterator<Generator>(std::forward<Generator>(gen));
  }

}
