#ifndef _H3DCV_DEBUG_HH_
#define _H3DCV_DEBUG_HH_

#include <stdio.h>
#include <assert.h>
#include <map>

#ifdef NDEBUG
# undef H3DCV_DEBUG_LEVEL
# define H3DCV_DEBUG_LEVEL 0
#endif

#ifndef H3DCV_DEBUG_LEVEL
# define H3DCV_DEBUG_LEVEL 2
#endif

#include <H3DCV/H3DCV.hh>
#include <H3DUtil/Console.h>

namespace H3DCV {
  
  typedef std::pair<void*,std::string> once_key_t;
  typedef std::map<once_key_t,int> once_t;
  
  class H3DCV_API Debug {
  public:
    static once_t once;
  };
  
  class H3DCV_API H3DCVException
    : public H3DUtil::Exception::H3DAPIException,
      public std::exception {
  public:
    H3DCVException( const string &_message = "",
                   const string &_function = "",
                   const string &_filename = "",
                   const int &_linenumber = -1 )
      throw () :
      H3DUtil::Exception::H3DAPIException( _message, _function,
                                           _filename, _linenumber){}
    inline ~H3DCVException() throw() {}
    inline const char* what() const throw() {
      return message.c_str(); }
  protected:
    string className() const { return "H3DCVException"; }
  };
}

/* Example
  #include <sstream>
  ostringstream buffer;
  buffer << " CAM=" << currentCameraIdx
         << " TopX=" << dispOrigin[currentCameraIdx].x
         << " TopY=" << dispOrigin[currentCameraIdx].y 
         << " dispWidth=" << dispWidth 
         << " dispHeight=" << dispHeight 
		 << std::ends;
  H3DCV_DEBUG_LOG_I(buffer.str());

*/



#if H3DCV_DEBUG_LEVEL > 1
# define H3DCV_DEBUG_LOG_I(s) H3DUtil::Console(3) << __FILE__ << ":" << __LINE__ << " (" << __FUNCTION__ << ")" << std::endl << s << std::endl
# define H3DCV_DEBUG_LOG() H3DUtil::Console(3) << __FILE__ << ":" << __LINE__ << " (" << __FUNCTION__ << ")" << std::endl
#else
# define H3DCV_DEBUG_LOG_I(s) // s
# define H3DCV_DEBUG_LOG() // s
#endif

#if H3DCV_DEBUG_LEVEL > 0
# define H3DCV_DEBUG_LOG_W(s) H3DUtil::Console(3) << __FILE__ << ":" << __LINE__ << " (" << __FUNCTION__ << ")" << std::endl << s << std::endl
#else
# define H3DCV_DEBUG_LOG_W(s) // s
#endif

#define H3DCV_DEPRECATED() (H3DCV::Debug::once[std::pair<void*,std::string>(this,__FUNCTION__)]++?(void*)0:( H3DUtil::Console(3) << "Function '" << __FUNCTION__ << "' is deprecated and may be removed in future releases of H3DCV." << std::endl ))
#define H3DCV_DEPRECATED_BY(s) (H3DCV::Debug::once[std::pair<void*,std::string>(this,__FUNCTION__)]++?(void*)0:( H3DUtil::Console(3) << "Function '" << __FUNCTION__ << "' is deprecated and may be removed in future releases of H3DCV. Function '" << s << "' should be used instead.\n" ))

#define H3DCV_RUNTIME_WARNING(w,s) (H3DUtil::Console(3) << w << std::endl << s << std::endl)

#define H3DCV_RUNTIME_WARNING_ONCE(w,s) (H3DCV::Debug::once[std::pair<void*,std::string>(this,w#s)]++?(void*)0:(H3DUtil::Console(3) << w << std::endl << s << std::endl))

#define H3DCV_RUNTIME_WARNING_RESET(w,s) (H3DCV::Debug::once[std::pair<void*,std::string>(this,w#s)] = 0)

#define H3DCV_EXCEPTION(s) H3DCVException(s,__FUNCTION__,__FILE__,__LINE__)

#endif
