#ifndef CDCL_SOLVER_UTILS_H
#define CDCL_SOLVER_UTILS_H


namespace utils
{
	template<typename T>
	struct Maybe
	{
		T value;
		bool has_value;

		Maybe(T value = -1, bool hasValue = false) : value(value), has_value(hasValue)
		{}
	};
}


#endif //CDCL_SOLVER_UTILS_H
