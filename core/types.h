#pragma once

namespace types
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
}