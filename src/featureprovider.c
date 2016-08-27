#include "featureprovider.h"


G_DEFINE_INTERFACE(GeanyFeatureProvider, geany_feature_provider, G_TYPE_OBJECT);


GType geany_filetype_feature_get_type(void)
{
	static GType type = 0;
	if (type == 0)
	{
		static const GEnumValue values[] =
		{
			{ GEANY_FILETYPE_FEATURE_AUTO_COMPLETE, "GEANY_FILETYPE_FEATURE_AUTOCOMPLETE", "Auto-complete" },
			{ GEANY_FILETYPE_FEATURE_CALLTIPS, "GEANY_FILETYPE_FEATURE_CALLTIPS", "Calltips" },
			{ GEANY_FILETYPE_FEATURE_DIAGNOSTICS, "GEANY_FILETYPE_FEATURE_DIAGNOSTICS", "Diagnostics" },
			{ GEANY_FILETYPE_FEATURE_HIGHLIGHTING, "GEANY_FILETYPE_FEATURE_HIGHLIGHTING", "Highlighting" },
			{ GEANY_FILETYPE_FEATURE_SYMBOLS, "GEANY_FILETYPE_FEATURE_SYMBOLS", "Symbols" },
			{ 0, NULL, NULL }
		};
		type = g_enum_register_static("GeanyFiletypeFeature", values);
	}
	return type;
}


static void geany_feature_provider_default_init(GeanyFeatureProviderInterface *iface)
{
	g_object_interface_install_property(iface,
		g_param_spec_enum("feature", "Feature", "The feature this provider provides",
			GEANY_TYPE_FILETYPE_FEATURE, 0, G_PARAM_CONSTRUCT_ONLY));
}


gboolean geany_feature_provider_activate(GeanyFeatureProvider *self, GeanyFiletypeID ft, GError **error)
{
	GeanyFeatureProviderInterface *iface;
	g_return_val_if_fail(GEANY_IS_FEATURE_PROVIDER(self), FALSE);
	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
	g_return_val_if_fail(ft >= 0 && ft < filetypes_array->len, FALSE);
	iface = GEANY_FEATURE_PROVIDER_GET_IFACE(self);
	g_return_val_if_fail(iface->activate != NULL, FALSE);
	return iface->activate(self, ft, error);
}


gboolean geany_feature_provider_deactivate(GeanyFeatureProvider *self, GeanyFiletypeID ft, GError **error)
{
	GeanyFeatureProviderInterface *iface;
	g_return_val_if_fail(GEANY_IS_FEATURE_PROVIDER(self), FALSE);
	g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
	g_return_val_if_fail(ft >= 0 && ft < filetypes_array->len, FALSE);
	iface = GEANY_FEATURE_PROVIDER_GET_IFACE(self);
	g_return_val_if_fail(iface->deactivate != NULL, FALSE);
	return iface->deactivate(self, ft, error);
}


GeanyFiletypeFeature geany_feature_provider_get_filetype_feature(GeanyFeatureProvider *self)
{
	GeanyFiletypeFeature feature = 0;
	g_return_val_if_fail(GEANY_IS_FEATURE_PROVIDER(self), 0);
	g_object_get(self, "feature", &feature, NULL);
	return feature;
}


void geany_feature_provider_set_filetype_feature(GeanyFeatureProvider *self,
	GeanyFiletypeFeature feature)
{
	GeanyFiletypeFeature old_feature = 0;
	g_return_if_fail(GEANY_IS_FEATURE_PROVIDER(self));
	g_return_if_fail(feature >=0 && feature < GEANY_NUM_FILETYPE_FEATURES);
	old_feature = geany_feature_provider_get_filetype_feature(self);
	if (feature != old_feature)
	{
		g_object_set(self, "feature", feature, NULL);
		g_object_notify(G_OBJECT(self), "feature");
	}
}
