/**
 *  @file
 *  @copyright defined in snax/LICENSE.txt
 */

#include <snax/utilities/tempdir.hpp>

#include <cstdlib>

namespace snax { namespace utilities {

fc::path temp_directory_path()
{
   const char* snax_tempdir = getenv("SNAX_TEMPDIR");
   if( snax_tempdir != nullptr )
      return fc::path( snax_tempdir );
   return fc::temp_directory_path() / "snax-tmp";
}

} } // snax::utilities
