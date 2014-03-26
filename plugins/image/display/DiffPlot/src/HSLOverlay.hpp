#ifndef HSLOVERLAY_HPP
#define	HSLOVERLAY_HPP

#include "DiffPlotPlugin.hpp"
#include "DiffPlotDisplay.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/InteractInfos.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

namespace tuttle {
namespace plugin {
namespace diffPlot {
	
/*
 *Enum used to know which RGB channel are selected
 */
enum ESelectedChannelHSL
{
	eSelectedChannelMoreHSL,		//More than one RGB channel are selected
	eSelectedChannelH,				//Only red is selected
	eSelectedChannelS,				//Only green is selected
	eSelectedChannelL,				//Only blue is selected
	eSelectedChannelNoneHSL			//None channel is selected
};

/// @todo class HSLOverlay : public OFX::OverlayInteract (when Nuke overlay works)
class HSLOverlay
{
public:
	/*Class arguments*/
	DiffPlotPlugin* _plugin;	//plugin reference
	//interact::InteractInfos _infos;  (when Nuke overlay works)
		
	/*Creator*/
	//HSLOverlay(OfxInteractHandle handle, OFX::ImageEffect* effect); (when Nuke overlay works)
	HSLOverlay(DiffPlotPlugin* plugin); //temporary
	
	/*Destructor*/
	~HSLOverlay();
	
	/*draw main function*/
	bool draw(const OFX::DrawArgs& args);
	
	/*get selected channels*/
	ESelectedChannelHSL getOnlyChannelSelectedHSL()const;
	
	/*get overlay data*/
	OverlayData& getOverlayData();
};

}
}
}
#endif	/* HSLOVERLAY_HPP */
