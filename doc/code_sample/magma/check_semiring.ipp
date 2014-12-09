std::vector <float> examples;
examples.push_back (-.3f);
examples.push_back (0.f);
examples.push_back (4.f);
math::check_semiring <float, math::either> (math::times, math::plus, examples);
