static_assert (math::has <
    math::callable::identity <double, math::callable::times>()>::value,
    "'identity' should be defined for double and operation 'times'.");
static_assert (math::has <
    math::callable::times (math::callable::plus (double, double const), double)
    >::value,
    "All operations in the sequence of operations should be defined.");
static_assert (math::is::associative <
    math::callable::times (double, double &)>::value,
    "'times' is associative.");
static_assert (math::is::commutative <
    math::callable::plus (double , double)>::value,
    "'plus' is commutative.");
static_assert (math::is::distributive <
    math::callable::times (math::callable::plus (double, double const), double)
    >::value, "'times' distributes over 'plus'.");
