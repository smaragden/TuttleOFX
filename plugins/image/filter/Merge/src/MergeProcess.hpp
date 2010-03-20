#ifndef _MERGE_PROCESS_HPP_
#define _MERGE_PROCESS_HPP_

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
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

/**
 * @brief Merge process
 */
template<class View, class Functor>
class MergeProcess : public ImageGilProcessor<View>
{
protected:
	MergePlugin& _plugin; ///< Rendering plugin
	View _srcViewA; ///< Source view A
	View _srcViewB; ///< Source view B
	boost::scoped_ptr<OFX::Image> _srcA;
	boost::scoped_ptr<OFX::Image> _srcB;
	boost::scoped_ptr<OFX::Image> _dst;

public:
	MergeProcess( MergePlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "MergeProcess.tcc"

#endif