#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"
#include "EXRReaderProcess.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <ImfInputFile.h>
#include <ImathBox.h>
#include <ImfChannelList.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

namespace bfs = boost::filesystem;
using namespace Imf;
using namespace boost::gil;

EXRReaderPlugin::EXRReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
	, _channels ( 0 )
	, _par      ( 1.0 )
{
	_outComponents   = fetchChoiceParam( kTuttlePluginChannel );
	_redComponents   = fetchChoiceParam( kParamOutputRedIs );
	_greenComponents = fetchChoiceParam( kParamOutputGreenIs );
	_blueComponents  = fetchChoiceParam( kParamOutputBlueIs );
	_alphaComponents = fetchChoiceParam( kParamOutputAlphaIs );
	
	_outputData      = fetchChoiceParam( kParamOutputData );
	
	_vChannelChoice.push_back( fetchChoiceParam( kParamOutputRedIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kParamOutputGreenIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kParamOutputBlueIs ) );
	_vChannelChoice.push_back( fetchChoiceParam( kParamOutputAlphaIs ) );

	updateCombos();
}

EXRReaderProcessParams EXRReaderPlugin::getProcessParams( const OfxTime time )
{
	EXRReaderProcessParams params;

	params._filepath       = getAbsoluteFilenameAt( time );
	params._outComponents  = _outComponents->getValue();
	params._fileComponents = _channels;
	
	
	params._redChannelIndex   = _redComponents->getValue();
	params._greenChannelIndex = _greenComponents->getValue();
	params._blueChannelIndex  = _blueComponents->getValue();
	params._alphaChannelIndex = _alphaComponents->getValue();
	
	params._displayWindow     = ( _outputData->getValue() == 0 );
	
	return params;
}

void EXRReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kTuttlePluginFilename )
	{
		ReaderPlugin::changedParam( args, paramName );
		updateCombos();
	}
	else if( paramName == kTuttlePluginChannel )
	{
		switch( _outComponents->getValue() )
		{
			case eParamReaderChannelGray:
			{
				for( std::size_t j = 0; j < _vChannelChoice.size() - 1; ++j )
				{
					_vChannelChoice[j]->setIsSecret( true );
				}
				_vChannelChoice[3]->setIsSecret( false );
				break;
			}
			case eParamReaderChannelRGB:
			{
				for( std::size_t j = 0; j < _vChannelChoice.size() - 1; ++j )
				{
					_vChannelChoice[j]->setIsSecret( false );
				}
				_vChannelChoice[3]->setIsSecret( true );
				break;
			}
			case eParamReaderChannelAuto:
			case eParamReaderChannelRGBA:
			{	
				for( std::size_t j = 0; j < _vChannelChoice.size(); ++j )
				{
					_vChannelChoice[j]->setIsSecret( false );
				}
				break;
			}
		}
	}
	else
	{
		ReaderPlugin::changedParam( args, paramName );
	}
}

void EXRReaderPlugin::updateCombos()
{
	const std::string filename( getAbsoluteFirstFilename() );
	//TUTTLE_LOG_VAR("update Combo");
	if( bfs::exists( filename ) )
	{
		// read dims
		InputFile in( filename.c_str() );
		const Header& h  = in.header();
		const ChannelList& cl = h.channels();
		
		_par = h.pixelAspectRatio();
		
		// Hide output channel selection till we don't select a channel.
		for( std::size_t i = 0; i < _vChannelChoice.size(); ++i )
		{
			//_vChannelChoice[i]->setIsSecret( true );
			_vChannelChoice[i]->resetOptions();
		}
		_vChannelNames.clear();
		for( ChannelList::ConstIterator it = cl.begin(); it != cl.end(); ++it )
		{
			_vChannelNames.push_back( it.name() );
			//TUTTLE_LOG_VAR( it.name() );
			for( std::size_t j = 0; j < _vChannelChoice.size(); ++j )
			{
				_vChannelChoice[j]->appendOption( it.name() );
			}
			++_channels;
			switch( _channels )
			{
				case 1:
				{
					for( std::size_t j = 0; j < _vChannelChoice.size(); j++ )
						_vChannelChoice.at(j)->setValue( 0 );
					break;
				}
				case 3:
				{
					for( std::size_t j = 0; j < _vChannelChoice.size() - 1; j++ )
						_vChannelChoice.at(j)->setValue( 2 - j );
					_vChannelChoice.at(3)->setValue( 0 );
					break;
				}
				case 4:
				{
					for( std::size_t j = 0; j < _vChannelChoice.size(); j++ )
						_vChannelChoice.at(j)->setValue( 3 - j );
					break;
				}
			}
		}
	}
}

void EXRReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	
	if( getExplicitChannelConversion() == eParamReaderChannelAuto )
	{
		switch( _channels )
		{
			case 1:
			{
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentAlpha );
				break;
			}
			case 3:
			{
				if( OFX::getImageEffectHostDescription()->supportsPixelComponent( OFX::ePixelComponentRGB ) )
					clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGB );
				else
					clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
				break;
			}
			case 4:
			{
				clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
				break;
			}
			default:
			{
				std::string msg = "EXR: not support ";
				msg += _channels;
				msg += " channels.";
				BOOST_THROW_EXCEPTION( exception::FileNotExist()
									   << exception::user( msg ) );
				break;
			}
		}
	}

	clipPreferences.setPixelAspectRatio( *this->_clipDst, _par );
}

bool EXRReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	try
	{
		InputFile in( getAbsoluteFilenameAt( args.time ).c_str() );
		const Header&      h      = in.header();
		const Imath::Box2i window( ( _outputData->getValue() == 0 ) ? h.displayWindow() : h.dataWindow() );

		std::cout << "getRegionOfDefinition " << ( window.size().x + 1 ) * _par << " x " << window.size().y + 1 << std::endl;
		
		rod.x1 = 0;
		rod.x2 = window.size().x + 1;
		rod.y1 = 0;
		rod.y2 = window.size().y + 1;

	}
	catch( ... )
	{
		BOOST_THROW_EXCEPTION( exception::FileInSequenceNotExist()
		<< exception::user( "EXR: Unable to open file." )
		<< exception::filename( getAbsoluteFilenameAt( args.time ) ) );
	}
	return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRReaderPlugin::render( const OFX::RenderArguments& args )
{
	ReaderPlugin::render( args );
	doGilRender<EXRReaderProcess>( *this, args );
}


}
}
}
}
