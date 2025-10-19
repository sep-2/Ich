# pragma once
//# define NO_S3D_USING
# include <Siv3D.hpp>

# if(SIV3D_BUILD(DEBUG))
#	define PRINT Print
# else
struct NullPrint_impl
{
  constexpr const NullPrint_impl& operator <<(const auto& _) const noexcept { return *this; }
};
inline constexpr NullPrint_impl NullPrint{};
# define PRINT NullPrint
# endif
