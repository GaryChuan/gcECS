#pragma once

namespace core::types
{
	template <typename>
	struct process_tuple;

	template <template <typename...> typename Tuple, typename... TArgs>
	struct process_tuple<Tuple<TArgs...>>
	{
		template <typename TFunc>
		constexpr void operator()(TFunc&& func) const noexcept
		{
			(func. template operator()<TArgs>(), ...);
		}
	};

    namespace details
    {
        template <typename T, typename TArg>
        struct tuple_type_to_index;

        template <typename T, typename... TArgs>
        struct tuple_type_to_index<T, std::tuple<T, TArgs...>>
        {
            static constexpr std::size_t value = 0;
        };

        template <typename T, typename U, typename... TArgs>
        struct tuple_type_to_index<T, std::tuple<U, TArgs...>>
        {
            static constexpr std::size_t value = 1 + tuple_type_to_index<T, std::tuple<TArgs...>>::value;
        };
    }

    template<typename T, typename Tuple>
    static constexpr auto tuple_type_to_index_v = details::tuple_type_to_index<T, Tuple>::value;

    template <typename T>
    struct full_decay
    {
        using type = std::remove_cvref_t<std::remove_pointer_t<T>>;
    };

    template <typename T>
    using full_decay_t = full_decay<T>::type;
}