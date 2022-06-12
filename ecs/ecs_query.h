#pragma once

#include "../core/bitarray.h"
#include "../core/functions.h"
#include "ecs_component.h"
#include "ecs_settings.h"

namespace ecs
{
	struct Query
	{
		using ArchetypeSignature = core::BitArray<settings::max_component_types>;
		ArchetypeSignature mMust;					   
		ArchetypeSignature mOneOf;
		ArchetypeSignature mNoneOf;
		
		bool Compare(const ArchetypeSignature& archetypeSignature) const noexcept
		{
			std::uint64_t c = 0;
			std::uint64_t a = 0;

			for (int i = 0; i < archetypeSignature.Size(); ++i)
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

			assert(!(!a || c));

			return !a || c;
		}
		
		template <typename TFunction>
		void Set(TFunction&& func) noexcept
		{
			using argument_types =
				core::function::traits<TFunction>::argument_types;

			[this]<typename... TComponents>(std::tuple<TComponents...>*)
			{
				([this]<component::is_valid_type TComponent>(TComponent*)
				{
					if constexpr (std::is_pointer_v<TComponent>)
					{
						mOneOf.SetBit(component::info_v<TComponent>.mUID);
					}
					else if constexpr (std::is_reference_v<TComponent>)
					{
						mMust.SetBit(component::info_v<TComponent>.mUID);
					}
				}(static_cast<TComponents*>(nullptr)), ...);
			}(static_cast<argument_types*>(nullptr));

		}

		template <template <typename... TQueries> typename Tuple>
		void Set() noexcept
		{

		}

	private:

	};
}