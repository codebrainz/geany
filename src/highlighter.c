#include "highlighter.h"


static void geany_highlighter_feature_provider_interface_init(GeanyFeatureProviderInterface *iface);


G_DEFINE_ABSTRACT_TYPE_WITH_CODE(GeanyHighlighter, geany_highlighter, GEANY_TYPE_FEATURE_PROVIDER,
	G_IMPLEMENT_INTERFACE(GEANY_TYPE_FEATURE_PROVIDER, geany_highlighter_feature_provider_interface_init)
	G_ADD_PRIVATE(TN))


void geany_highlighter_init_styles(GeanyHighlighter *self, guint start_id)
{
	GeanyHighlighterInterface *iface;
	g_return_if_fail(GEANY_IS_HIGHLIGHTER(self));
	iface = GEANY_HIGHLIGHTER_GET_IFACE(self);
	g_return_if_fail(iface->init_styles != NULL);
	iface->init_styles(self, start_id);
}


void geany_highlighter_highlight(GeanyHighlighter *self, guint start, guint end)
{
	GeanyHighlighterInterface *iface;
	g_return_if_fail(GEANY_IS_HIGHLIGHTER(self));
	g_return_if_fail(end > start);
	iface = GEANY_HIGHLIGHTER_GET_IFACE(self);
	g_return_if_fail(iface->highlight != NULL);
	iface->highlight(self, start, end);
}


static void geany_highlighter_feature_provider_interface_init(GeanyFeatureProviderInterface *iface)
{
}
