
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

GType
geany_scintilla_eol_mode_get_type (void)
{
  static GType etype = 0;
  if (G_UNLIKELY(etype == 0)) {
    static const GEnumValue values[] = {
      { GEANY_SCINTILLA_EOL_MODE_CRLF, "GEANY_SCINTILLA_EOL_MODE_CRLF", N_("crlf") },
      { GEANY_SCINTILLA_EOL_MODE_CR, "GEANY_SCINTILLA_EOL_MODE_CR", N_("cr") },
      { GEANY_SCINTILLA_EOL_MODE_LF, "GEANY_SCINTILLA_EOL_MODE_LF", N_("lf") },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static (g_intern_static_string ("GeanyScintillaEOLMode"), values);
  }
  return etype;
}

GType
geany_scintilla_margin_get_type (void)
{
  static GType etype = 0;
  if (G_UNLIKELY(etype == 0)) {
    static const GEnumValue values[] = {
      { GEANY_SCINTILLA_MARGIN_LINE_NUMBERS, "GEANY_SCINTILLA_MARGIN_LINE_NUMBERS", N_("line-numbers") },
      { GEANY_SCINTILLA_MARGIN_FOLD, "GEANY_SCINTILLA_MARGIN_FOLD", N_("fold") },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static (g_intern_static_string ("GeanyScintillaMargin"), values);
  }
  return etype;
}


/* Generated data ends here */

