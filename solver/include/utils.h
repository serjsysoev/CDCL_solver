#ifndef CDCL_SOLVER_UTILS_H
#define CDCL_SOLVER_UTILS_H


namespace utils
{
	template<typename T>
	struct Maybe
	{
		T value;
		bool has_value;

		explicit Maybe(T value) : value(value), has_value(true)
		{}

		Maybe() : has_value(false)
		{}
	};
}


#endif //CDCL_SOLVER_UTILS_H
