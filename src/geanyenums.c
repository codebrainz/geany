
/* Generated data (by glib-mkenums) */

#include "geanyenums.h"

#include <glib/gi18n.h>

/* enumerations from "geanyscintilla.h" */
GType
geany_scintilla_edge_mode_get_type (void)
{
  static GType etype = 0;
  if (G_UNLIKELY(etype == 0)) {
    static const GEnumValue values[] = {
      { GEANY_SCINTILLA_EDGE_MODE_NONE, "GEANY_SCINTILLA_EDGE_MODE_NONE", N_("none") },
      { GEANY_SCINTILLA_EDGE_MODE_LINE, "GEANY_SCINTILLA_EDGE_MODE_LINE", N_("line") },
      { GEANY_SCINTILLA_EDGE_MODE_BACKGROUND, "GEANY_SCINTILLA_EDGE_MODE_BACKGROUND", N_("background") },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static (g_intern_static_string ("GeanyScintillaEdgeMode"), values);
  }
  return etype;
}


/* Generated data ends here */

