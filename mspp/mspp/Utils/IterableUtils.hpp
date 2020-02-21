#pragma once

namespace IterableUtils
{
	template <typename TIterable>
	inline constexpr bool starts_with(const TIterable& iterable, const TIterable& sub_iterable)
	{
		auto iterable_iter = iterable.cbegin();
		auto sub_iterable_iter = sub_iterable.cbegin();
		for (;; ++iterable_iter, ++sub_iterable_iter)
		{
			if ((iterable.cend() == iterable_iter) || (sub_iterable.cend() == sub_iterable_iter))
			{
				break;
			}
			if(*iterable_iter != *sub_iterable_iter)
			{
				return false;
			}
		}
		return sub_iterable.cend() == sub_iterable_iter;
	}
}