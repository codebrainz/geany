#include "ftplugins.h"
#include "document.h"
#include "plugindata.h"
#include "pluginprivate.h"


typedef struct
{
	GeanyPlugin *plugin;
	GeanyFiletypeID filetype;
	GeanyFiletypeFeature feature;
	GeanyFtPluginFunc func;
	gpointer user_data;
	GDestroyNotify user_data_free;
}
FtPluginProvider;


static GHashTable *feature_providers[GEANY_NUM_FILETYPE_FEATURES] = { NULL };


static FtPluginProvider *ftplugin_provider_new(GeanyPlugin *plugin,
	GeanyFiletypeID ft, GeanyFiletypeFeature feature, GeanyFtPluginFunc func,
	gpointer user_data, GDestroyNotify user_data_free)
{
	FtPluginProvider *provider = g_slice_new0(FtPluginProvider);
	if (provider != NULL)
	{
		provider->plugin = plugin;
		provider->filetype = ft;
		provider->feature = feature;
		provider->func = func;
		provider->user_data = user_data;
		provider->user_data_free = user_data_free;
	}
	return provider;
}


static void ftplugin_provider_free(gpointer pprovider)
{
	FtPluginProvider *provider = pprovider;
	if (provider != NULL)
	{
		if (provider->user_data_free)
			provider->user_data_free(provider->user_data);
		g_slice_free(FtPluginProvider, provider);
	}
}


static void ftplugin_free_queue(gpointer pqueue)
{
	g_queue_free_full(pqueue, ftplugin_provider_free);
}


void geany_ftplugin_init(void)
{
	for (guint i = 0; i < GEANY_NUM_FILETYPE_FEATURES; i++)
	{
		feature_providers[i] = g_hash_table_new_full(g_direct_hash,
			g_direct_equal, NULL, ftplugin_free_queue);
		for (guint i = 0; i < filetypes_array->len; i++)
		{
			g_hash_table_insert(feature_providers[i],
				GINT_TO_POINTER(i), g_queue_new());
		}
	}
}


void geany_ftplugin_finalize(void)
{
	for (guint i = 0; i < GEANY_NUM_FILETYPE_FEATURES; i++)
		g_hash_table_destroy(feature_providers[i]);
}


// quickly check if any plugin is providing the given feature for
// the specified filetype
gboolean geany_ftplugin_have_provider(GeanyFiletypeID ft,
	GeanyFiletypeFeature feature)
{
	GHashTable *table = feature_providers[feature];
	GQueue *queue = g_hash_table_lookup(table, GINT_TO_POINTER(ft));
	if (queue != NULL)
		return (g_queue_get_length(queue) > 0);
	return FALSE;
}


// when a plugin is unloaded, this should be called to remove it
// from being a provider and getting called into.
void geany_ftplugin_unload(GeanyPlugin *plugin)
{
	for (guint i = 0; i < GEANY_NUM_FILETYPE_FEATURES; i++)
	{
		GHashTable *table = feature_providers[i];
		GList *values = g_hash_table_get_values(table);
		GPtrArray *unload_plugins = g_ptr_array_new();
		for (GList *iter = values; iter != NULL; iter = iter->next)
		{
			GQueue *queue = iter->data;
			GList *iter = g_queue_peek_head(queue);
			while (iter != NULL)
			{
				GList *next = iter->next;
				FtPluginProvider *provider = iter->data;
				if (provider->plugin == plugin)
					g_queue_delete_link(queue, iter);
				iter = next;
			}
		}
	}
}


/**
 * Register a plugin to provide a feature for a given filetype.
 *
 * @plugin The GeanyPlugin providing the feature
 * @ft The ID of the filetype to provide the feature for.
 * @feature The feature to provide.
 * @func The function to call to provide the feature.
 * @user_data Extra user data passed to the callback `func`.
 * @user_data_free A function to call on the `user_data` when the
 * feature has been unregistered, for example when the plugin is
 * unloaded.
 *
 * @return `TRUE` if the registration succeeded, `FALSE` if not.
 */
gboolean geany_ftplugin_register_provider(GeanyPlugin *plugin,
	GeanyFiletypeID ft, GeanyFiletypeFeature feature, GeanyFtPluginFunc func,
	gpointer user_data, GDestroyNotify user_data_free)
{
	GQueue *queue = g_hash_table_lookup(feature_providers[feature],
		GINT_TO_POINTER(ft));
	if (queue != NULL)
	{
		FtPluginProvider *provider = ftplugin_provider_new(plugin, ft,
			feature, func, user_data, user_data_free);
		g_queue_push_tail(queue, provider);
		return TRUE;
	}
	return FALSE;
}


gboolean geany_ftplugin_highlight(GeanyFiletypeID ft, GeanyFiletypeFeature feature,
	GeanyDocument *doc, guint start, guint end)
{
	return FALSE;
}
