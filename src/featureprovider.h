/* The base interface for filetype plugin feature providers */

#ifndef GEANY_FEATUREPROVIDER_H
#define GEANY_FEATUREPROVIDER_H

#include "filetypes.h"
#include <glib-object.h>


G_BEGIN_DECLS


#define GEANY_TYPE_FILETYPE_FEATURE (geany_filetype_feature_get_type())
#define GEANY_TYPE_FEATURE_PROVIDER (geany_feature_provider_get_type())


G_DECLARE_INTERFACE(GeanyFeatureProvider, geany_feature_provider, GEANY, FEATURE_PROVIDER, GObject)


typedef enum
{
	GEANY_FILETYPE_FEATURE_AUTO_COMPLETE,
	GEANY_FILETYPE_FEATURE_CALLTIPS,
	GEANY_FILETYPE_FEATURE_DIAGNOSTICS,
	GEANY_FILETYPE_FEATURE_HIGHLIGHTING,
	GEANY_FILETYPE_FEATURE_SYMBOLS,
	// ...
	GEANY_NUM_FILETYPE_FEATURES
}
GeanyFiletypeFeature;


struct _GeanyFeatureProviderInterface
{
	GTypeInterface parent_iface;

	/*
	 * The activate function is called when a document with one of the
	 * supported filetypes is activated (current). This tells the
	 * plugin it should be prepared to start providing one or more of
	 * the features it provides.
	 */
	gboolean (*activate) (GeanyFeatureProvider *self, GeanyFiletypeID ft, GError **error);

	/*
	 * The deactivate function is called when a document which the
	 * plugin is activated for becomes no longer active (current). This
	 * tells the plugin it no longer needs to provide any of the
	 * features it provides until re-activated again.
	 */
	gboolean (*deactivate) (GeanyFeatureProvider *self, GeanyFiletypeID ft, GError **error);
};


GType geany_filetype_feature_get_type(void) G_GNUC_CONST;


#ifdef GEANY_PRIVATE

gboolean geany_feature_provider_activate(GeanyFeatureProvider *self, GeanyFiletypeID ft, GError **error);
gboolean geany_feature_provider_deactivate(GeanyFeatureProvider *self, GeanyFiletypeID ft, GError **error);
GeanyFiletypeFeature geany_feature_provider_get_filetype_feature(GeanyFeatureProvider *self);
void geany_feature_provider_set_filetype_feature(GeanyFeatureProvider *self, GeanyFiletypeFeature feature);

#endif


G_END_DECLS

#endif // GEANY_FEATUREPROVIDER_H
