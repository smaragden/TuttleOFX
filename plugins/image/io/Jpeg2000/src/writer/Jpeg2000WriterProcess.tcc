#include "Jpeg2000WriterPlugin.hpp"

#include <boost/assert.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace writer {

template<class View>
Jpeg2000WriterProcess<View>::Jpeg2000WriterProcess( Jpeg2000WriterPlugin &instance )
: ImageGilFilterProcessor<View>( instance, eImageOrientationFromTopToBottom )
, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
Jpeg2000WriterProcess<View>::~Jpeg2000WriterProcess()
{
	_writer.close();
}

template<class View>
void Jpeg2000WriterProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.time );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void Jpeg2000WriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	using namespace terry;
	BOOST_ASSERT( procWindowRoW == this->_srcPixelRod );

	_writer.setCinemaMode( (OPJ_CINEMA_MODE)_params._cineProfil );
	_writer.setLossless( _params._lossless );

	View srcView = this->_srcView;

//	TUTTLE_COUT_VAR( this->_srcView.dimensions() );
//	TUTTLE_COUT_VAR( srcView.dimensions() );

	switch(_params._bitDepth)
	{
		case eTuttlePluginBitDepth8:
		{
			rgb8_image_t img( srcView.dimensions() );
			rgb8_view_t vw( view(img) );

			// Convert pixels in PIX_FMT_RGB24
			copy_and_convert_pixels(srcView, vw );

			uint8_t* pixels = (uint8_t*)boost::gil::interleaved_view_get_raw_data( vw );

			_writer.open( _params._filepath, srcView.width(), srcView.height(), num_channels<rgb8_view_t::value_type>::type::value, 8 );
			_writer.encode(pixels, 8);

			break;
		}
		case eTuttlePluginBitDepth12:
		case eTuttlePluginBitDepth16:
		{
			rgb16_image_t img( srcView.dimensions() );
			rgb16_view_t vw( view(img) );

//			TUTTLE_COUT_VAR( vw.dimensions() );

			copy_and_convert_pixels( srcView, vw );

			uint8_t* pixels = (uint8_t*)boost::gil::interleaved_view_get_raw_data( vw );

			_writer.open( _params._filepath, srcView.width(), srcView.height(), num_channels<rgb16_view_t::value_type>::type::value, 16 );
			_writer.encode(pixels, 16);

			break;
		}
		case eTuttlePluginBitDepth32:
		{
			rgb32_image_t img( srcView.dimensions() );
			rgb32_view_t vw( view(img) );

			// Convert pixels in PIX_FMT_RGB24
			copy_and_convert_pixels(srcView, vw );

			uint8_t* pixels = (uint8_t*)boost::gil::interleaved_view_get_raw_data( vw );

			_writer.open( _params._filepath, srcView.width(), srcView.height(), num_channels<rgb32_view_t::value_type>::type::value, 32 );
			_writer.encode(pixels, 32);
			break;
		}
		default:
		{
			BOOST_THROW_EXCEPTION( exception::ImageFormat() );
		}
	}
	// Convert pixels to destination
	copy_and_convert_pixels( this->_srcView, this->_dstView );
}

}
}
}
}
