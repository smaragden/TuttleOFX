%include <tuttle/host/global.i>
%include <tuttle/host/HostDescriptor.i>
%include <tuttle/host/INode.i>
%include <tuttle/host/attribute/Attribute.i>
%include <tuttle/host/attribute/ClipImage.i>
%include <tuttle/host/ofx/OfxhException.i>
%include <tuttle/host/ofx/property/OfxhPropertyTemplate.i>
%include <tuttle/host/ofx/property/OfxhSet.i>
%include <tuttle/host/ofx/property/OfxhNotifyHook.i>
%include <tuttle/host/ofx/property/OfxhGetHook.i>
%include <tuttle/host/ofx/attribute/OfxhParamSet.i>
%include <tuttle/host/ofx/attribute/OfxhClipImageSet.i>
%include <tuttle/host/ofx/attribute/OfxhClipImage.i>
%include <tuttle/host/ofx/OfxhImageEffectPlugin.i>
//%include <tuttle/host/ofx/OfxhImageEffectPluginCache.i>
%include <tuttle/host/ofx/OfxhImageEffectNode.i>

#include <std_vector.i>
#include <std_string.i>

%{
#include <tuttle/host/ImageEffectNode.hpp>
%}

namespace std {
%template(IntVector) vector<int>;
}

%include <tuttle/host/ImageEffectNode.hpp>

%extend tuttle::host::ImageEffectNode
{
	ofx::attribute::OfxhParam& __getitem__( const std::string& name )
	{
		return self->getParam( name );
		//			return self->getProcessAttribute(name); //< @todo tuttle: can be clip or params !
	}

	std::string __str__() const
	{
		std::stringstream s;

		s << *self;
		return s.str();
	}

}

