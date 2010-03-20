#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>


namespace tuttle {
namespace plugin {
namespace colorspace {
namespace log2lin {

using namespace boost::gil;

static const float cineonBlackOffset = powf(10.0f, (95.0f-685.0f) / 300.0f);

inline float fromCineon(float x)
{
	return (powf(10.0f,(1023.0f*x-685.0f)/300.0f)-cineonBlackOffset)/(1.0f-cineonBlackOffset);
}

struct log2liner
{
	inline rgba32f_pixel_t operator()( const rgba32f_pixel_t& p ) const
	{
		rgba32f_pixel_t p2;
		for( int n = 0; n < 3; ++n )
		{
			p2[n] = fromCineon(p[n]);
		}
		p2[3] = p[3];
		return p2;
	}
};

template<class View>
Log2LinProcess<View>::Log2LinProcess( Log2LinPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

/**
 * @brief Function called by rendering thread each time 
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void Log2LinProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	
	transform_pixels( color_converted_view<rgba32f_pixel_t>(src),  color_converted_view<rgba32f_pixel_t>(dst), log2liner() );
}

}
}
}
}