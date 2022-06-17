#pragma once

#include "../core/bitarray.h"
#include "../core/functions.h"
#include "ecs_component.h"
#include "ecs_settings.h"

namespace ecs
{
	struct Query
	{
		using ArchetypeSignature = core::bitarray<settings::max_component_types>;

		Query() = default;

		template <typename TFunction>
		Query(TFunction&& func) noexcept
		{
			Set<TFunction>(func);
		}

		bool Compare(const ArchetypeSignature& archetypeSignature) const noexcept
		{
			std::uint64_t c = 0;
			std::uint64_t a = 0;

			for (int i = 0, end = static_cast<int>(archetypeSignature.Size()); i < end; ++i)
			{
				if (mNoneOf[i] & archetypeSignature[i])
				{
					return false;
				}

				if ((mMust[i] & archetypeSignature[i]) != mMust[i])
				{
					return false;
				}
				
				c |= (mOneOf[i] & archetypeSignature[i]);
				a |= mOneOf[i];
			}

			assert(!(!a && c));
;
			return !a || c;
		}
		
		template <typename TFunction>
		void Set(TFunction&& func) noexcept
		{
			using argument_types =
				core::function::traits<TFunction>::argument_types;

			[this] <typename... TComponents>(std::tuple<TComponents...>*)
			{
				([this]<component::is_valid_type TComponent>(std::tuple<TComponent>*)
				{
					if constexpr (std::is_pointer_v<TComponent>)
					{
						mOneOf.SetBit(component::info_v<TComponent>.mUID);
					}
					else if constexpr (std::is_reference_v<TComponent>)
					{
						mMust.SetBit(component::info_v<TComponent>.mUID);
					}
				}(static_cast<std::tuple<TComponents>*>(nullptr)), ...);
			}(static_cast<argument_types*>(nullptr));
		}

	private:
		ArchetypeSignature mMust{};
		ArchetypeSignature mOneOf{};
		ArchetypeSignature mNoneOf{};
	};
}