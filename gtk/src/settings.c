/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * settings.c
 * Copyright (C) John Stebbins 2008-2018 <stebbins@stebbins>
 *
 * settings.c is free software.
 *
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * settings.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with main.c.  If not, write to:
 *  The Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor
 *  Boston, MA  02110-1301, USA.
 */

#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include "ghbcompat.h"
#include "settings.h"
#include "hb-backend.h"
#include "values.h"

void dump_settings(GhbValue *settings);
void ghb_pref_audio_init(signal_user_data_t *ud);

GObject*
debug_get_object(GtkBuilder* b, const gchar *n)
{
    g_message("name %s\n", n);
    return gtk_builder_get_object(b, n);
}

gint
ghb_settings_combo_int(const GhbValue *settings, const gchar *key)
{
    return ghb_lookup_combo_int(key, ghb_dict_get_value(settings, key));
}

gdouble
ghb_settings_combo_double(const GhbValue *settings, const gchar *key)
{
    return ghb_lookup_combo_double(key, ghb_dict_get_value(settings, key));
}

gchar*
ghb_settings_combo_option(const GhbValue *settings, const gchar *key)
{
    return ghb_lookup_combo_option(key, ghb_dict_get_value(settings, key));
}

GhbValue *ghb_get_title_settings(GhbValue *settings)
{
    GhbValue *title;
    title = ghb_dict_get(settings, "Title");
    return title;
}

GhbValue *ghb_get_title_audio_list(GhbValue *settings)
{
    GhbValue *title_dict = ghb_get_title_settings(settings);
    GhbValue *audio_list = ghb_dict_get(title_dict, "AudioList");
    return audio_list;
}

GhbValue *ghb_get_title_subtitle_list(GhbValue *settings)
{
    GhbValue *title_dict = ghb_get_title_settings(settings);
    GhbValue *subtitle_list = ghb_dict_get(title_dict, "SubtitleList");
    return subtitle_list;
}

GhbValue *ghb_get_job_settings(GhbValue *settings)
{
    GhbValue *job;
    job = ghb_dict_get(settings, "Job");
    if (job == NULL)
    {
        job = ghb_dict_new();
        ghb_dict_set(settings, "Job", job);
    }
    return job;
}

GhbValue* ghb_get_job_dest_settings(GhbValue *settings)
{
    GhbValue *job  = ghb_get_job_settings(settings);
    GhbValue *dest = ghb_dict_get(job, "Destination");
    if (dest == NULL)
    {
        dest = ghb_dict_new();
        ghb_dict_set(job, "Destination", dest);
    }
    return dest;
}

GhbValue* ghb_get_job_chapter_list(GhbValue *settings)
{
    GhbValue *dest     = ghb_get_job_dest_settings(settings);
    GhbValue *chapters = ghb_dict_get(dest, "ChapterList");
    if (chapters == NULL)
    {
        chapters = ghb_array_new();
        ghb_dict_set(dest, "ChapterList", chapters);
    }
    return chapters;
}

GhbValue* ghb_get_job_mp4_settings(GhbValue *settings)
{
    GhbValue *dest = ghb_get_job_dest_settings(settings);
    GhbValue *mp4  = ghb_dict_get(dest, "Mp4Options");
    if (mp4 == NULL)
    {
        mp4 = ghb_dict_new();
        ghb_dict_set(dest, "Mp4Options", mp4);
    }
    return mp4;
}

GhbValue* ghb_get_job_source_settings(GhbValue *settings)
{
    GhbValue *job    = ghb_get_job_settings(settings);
    GhbValue *source = ghb_dict_get(job, "Source");
    if (source == NULL)
    {
        source = ghb_dict_new();
        ghb_dict_set(job, "Source", source);
    }
    return source;
}

GhbValue* ghb_get_job_range_settings(GhbValue *settings)
{
    GhbValue *source = ghb_get_job_source_settings(settings);
    GhbValue *range  = ghb_dict_get(source, "Range");
    if (range == NULL)
    {
        range = ghb_dict_new();
        ghb_dict_set(source, "Range", range);
    }
    return range;
}

GhbValue* ghb_get_job_par_settings(GhbValue *settings)
{
    GhbValue *job = ghb_get_job_settings(settings);
    GhbValue *par = ghb_dict_get(job, "PAR");
    if (par == NULL)
    {
        par = ghb_dict_new();
        ghb_dict_set(job, "PAR", par);
    }
    return par;
}

GhbValue* ghb_get_job_video_settings(GhbValue *settings)
{
    GhbValue *job   = ghb_get_job_settings(settings);
    GhbValue *video = ghb_dict_get(job, "Video");
    if (video == NULL)
    {
        video = ghb_dict_new();
        ghb_dict_set(job, "Video", video);
    }
    return video;
}

GhbValue *ghb_get_job_audio_settings(GhbValue *settings)
{
    GhbValue *job   = ghb_get_job_settings(settings);
    GhbValue *audio = ghb_dict_get(job, "Audio");
    if (audio == NULL)
    {
        audio = ghb_dict_new();
        ghb_dict_set(job, "Audio", audio);
    }
    return audio;
}

GhbValue *ghb_get_job_audio_list(GhbValue *settings)
{
    GhbValue *audio_dict = ghb_get_job_audio_settings(settings);
    GhbValue *audio_list = ghb_dict_get(audio_dict, "AudioList");
    if (audio_list == NULL)
    {
        audio_list = ghb_array_new();
        ghb_dict_set(audio_dict, "AudioList", audio_list);
    }
    return audio_list;
}

GhbValue *ghb_get_job_subtitle_settings(GhbValue *settings)
{
    GhbValue *job = ghb_get_job_settings(settings);
    GhbValue *sub = ghb_dict_get(job, "Subtitle");
    if (sub == NULL)
    {
        sub = ghb_dict_new();
        ghb_dict_set(job, "Subtitle", sub);
    }
    return sub;
}

GhbValue *ghb_get_job_subtitle_list(GhbValue *settings)
{
    GhbValue *sub_dict = ghb_get_job_subtitle_settings(settings);
    GhbValue *sub_list = ghb_dict_get(sub_dict, "SubtitleList");
    if (sub_list == NULL)
    {
        sub_list = ghb_array_new();
        ghb_dict_set(sub_dict, "SubtitleList", sub_list);
    }
    return sub_list;
}

GhbValue *ghb_get_job_subtitle_search(GhbValue *settings)
{
    GhbValue *sub_dict   = ghb_get_job_subtitle_settings(settings);
    GhbValue *sub_search = ghb_dict_get(sub_dict, "Search");
    if (sub_search == NULL)
    {
        sub_search = ghb_dict_new();
        ghb_dict_set(sub_dict, "Search", sub_search);
        ghb_dict_set_bool(sub_search, "Enable", 0);
    }
    return sub_search;
}

GhbValue* ghb_get_job_metadata_settings(GhbValue *settings)
{
    GhbValue *job  = ghb_get_job_settings(settings);
    GhbValue *meta = ghb_dict_get(job, "Metadata");
    if (meta == NULL)
    {
        meta = ghb_dict_new();
        ghb_dict_set(job, "Metadata", meta);
    }
    return meta;
}

GhbValue* ghb_get_job_filter_settings(GhbValue *settings)
{
    GhbValue *job    = ghb_get_job_settings(settings);
    GhbValue *filter = ghb_dict_get(job, "Filters");
    if (filter == NULL)
    {
        filter = ghb_dict_new();
        ghb_dict_set(job, "Filters", filter);
    }
    return filter;
}

GhbValue* ghb_get_job_filter_list(GhbValue *settings)
{
    GhbValue *filter = ghb_get_job_filter_settings(settings);
    GhbValue *list   = ghb_dict_get(filter, "FilterList");
    if (list == NULL)
    {
        list = ghb_dict_new();
        ghb_dict_set(filter, "FilterList", list);
    }
    return list;
}

// Map widget names to setting keys
// Widgets that map to settings have names
// of this format: s_<setting key>
const gchar*
ghb_get_setting_key(GtkWidget *widget)
{
    const gchar *name;

    g_debug("get_setting_key ()\n");
    if (widget == NULL) return NULL;
    name = gtk_buildable_get_name(GTK_BUILDABLE(widget));

    if (name == NULL || !strncmp(name, "Gtk", 3))
    {
        name = gtk_widget_get_name(widget);
    }
    if (name == NULL)
    {
        // Bad widget pointer?  Should never happen.
        g_debug("Bad widget\n");
        return NULL;
    }
    return name;
}

GhbValue*
ghb_widget_value(GtkWidget *widget)
{
    GhbValue *value = NULL;
    const gchar *name;
    GType type;

    if (widget == NULL)
    {
        g_debug("NULL widget\n");
        return NULL;
    }

    type = G_OBJECT_TYPE(widget);
    name = ghb_get_setting_key(widget);
    if (type == GTK_TYPE_ENTRY)
    {
        const gchar *str = gtk_entry_get_text(GTK_ENTRY(widget));
        value = ghb_string_value_new(str);
    }
    else if (type == GTK_TYPE_RADIO_BUTTON)
    {
        gboolean bval;
#if !GTK_CHECK_VERSION(3, 90, 0)
        bval = gtk_toggle_button_get_inconsistent(GTK_TOGGLE_BUTTON(widget));
        if (bval)
        {
            value = ghb_bool_value_new(FALSE);
        }
        else
#endif
        {
            bval = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
            value = ghb_bool_value_new(bval);
        }
    }
    else if (type == GTK_TYPE_CHECK_BUTTON)
    {
        gboolean bval;
        bval = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        value = ghb_bool_value_new(bval);
    }
    else if (type == GTK_TYPE_TOGGLE_TOOL_BUTTON)
    {
        gboolean bval;
        bval = gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget));
        value = ghb_bool_value_new(bval);
    }
    else if (type == GTK_TYPE_TOGGLE_BUTTON)
    {
        gboolean bval;
        bval = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        value = ghb_bool_value_new(bval);
    }
    else if (type == GTK_TYPE_CHECK_MENU_ITEM)
    {
        gboolean bval;
        bval = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
        value = ghb_bool_value_new(bval);
    }
    else if (type == GTK_TYPE_COMBO_BOX)
    {
        GtkTreeModel *store;
        GtkTreeIter iter;
        gchar *shortOpt;

        store = gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
        if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget), &iter))
        {
            gtk_tree_model_get(store, &iter, 2, &shortOpt, -1);
            value = ghb_string_value_new(shortOpt);
            g_free(shortOpt);
        }
        else if (gtk_combo_box_get_has_entry(GTK_COMBO_BOX(widget)))
        {
            const gchar *str;
            str = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(widget))));
            if (str == NULL) str = "";
            value = ghb_string_value_new(str);
        }
        else
        {
            value = ghb_string_value_new("");
        }
    }
    else if (type == GTK_TYPE_SPIN_BUTTON)
    {
        double val;
        val = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
        value = ghb_double_value_new(val);
    }
    else if (type == GTK_TYPE_SCALE)
    {
        gdouble dval;
        gint digits;

        digits = gtk_scale_get_digits(GTK_SCALE(widget));
        dval = gtk_range_get_value(GTK_RANGE(widget));
        if (digits)
        {
            value = ghb_double_value_new(dval);
        }
        else
        {
            value = ghb_int_value_new(dval);
        }
    }
    else if (type == GTK_TYPE_SCALE_BUTTON)
    {
        gdouble dval;

        dval = gtk_scale_button_get_value(GTK_SCALE_BUTTON(widget));
        value = ghb_double_value_new(dval);
    }
    else if (type == GTK_TYPE_TEXT_VIEW)
    {
        GtkTextBuffer *buffer;
        GtkTextIter start, end;
        gchar *str;

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        str = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        value = ghb_string_value_new(str);
        g_free(str);
    }
    else if (type == GTK_TYPE_LABEL)
    {
        const gchar *str;
        str = gtk_label_get_text (GTK_LABEL(widget));
        value = ghb_string_value_new(str);
    }
    else if (type == GTK_TYPE_FILE_CHOOSER_BUTTON)
    {
        gchar *str = NULL;
        str = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(widget));
        if (str == NULL)
        {
            str = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(widget));
        }
        value = ghb_string_value_new(str);
        if (str != NULL)
            g_free(str);
    }
    else
    {
        g_warning("Attempt to get unknown widget type, name %s", name);
        g_free(value);
        value = NULL;
    }
    return value;
}

gchar*
ghb_widget_string(GtkWidget *widget)
{
    GhbValue *value;
    gchar *sval;

    value = ghb_widget_value(widget);
    sval = ghb_value_get_string_xform(value);
    ghb_value_free(&value);
    return sval;
}

gdouble
ghb_widget_double(GtkWidget *widget)
{
    GhbValue *value;
    gdouble dval;

    value = ghb_widget_value(widget);
    dval = ghb_value_get_double(value);
    ghb_value_free(&value);
    return dval;
}

gint64
ghb_widget_int64(GtkWidget *widget)
{
    GhbValue *value;
    gint64 ival;

    value = ghb_widget_value(widget);
    ival = ghb_value_get_int(value);
    ghb_value_free(&value);
    return ival;
}

gint
ghb_widget_int(GtkWidget *widget)
{
    GhbValue *value;
    gint ival;

    value = ghb_widget_value(widget);
    ival = (gint)ghb_value_get_int(value);
    ghb_value_free(&value);
    return ival;
}

gint
ghb_widget_boolean(GtkWidget *widget)
{
    GhbValue *value;
    gboolean bval;

    value = ghb_widget_value(widget);
    bval = ghb_value_get_bool(value);
    ghb_value_free(&value);
    return bval;
}

void
ghb_widget_to_setting(GhbValue *settings, GtkWidget *widget)
{
    const gchar *key = NULL;
    GhbValue *value;

    if (widget == NULL) return;
    g_debug("ghb_widget_to_setting");
    // Find corresponding setting
    key = ghb_get_setting_key(widget);
    if (key == NULL) return;
    value = ghb_widget_value(widget);
    if (value != NULL)
    {
        ghb_dict_set(settings, key, value);
    }
    else
    {
        g_debug("No value found for %s\n", key);
    }
}

void
ghb_update_widget(GtkWidget *widget, const GhbValue *value)
{
    GType type;
    gchar *str, *tmp;
    gint ival;
    gdouble dval;

    const char *name = ghb_get_setting_key(widget);
    type = ghb_value_type(value);
    if (type == GHB_ARRAY || type == GHB_DICT)
        return;
    if (value == NULL) return;
    str = tmp = ghb_value_get_string_xform(value);
    ival = ghb_value_get_int(value);
    dval = ghb_value_get_double(value);
    type = G_OBJECT_TYPE(widget);

    if (str == NULL)
        str = "";

    if (type == GTK_TYPE_ENTRY)
    {
        gtk_entry_set_text((GtkEntry*)widget, str);
    }
    else if (type == GTK_TYPE_RADIO_BUTTON)
    {
        if (ival)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), !!ival);
    }
    else if (type == GTK_TYPE_CHECK_BUTTON)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), ival);
    }
    else if (type == GTK_TYPE_TOGGLE_TOOL_BUTTON)
    {
        gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(widget), ival);
    }
    else if (type == GTK_TYPE_TOGGLE_BUTTON)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), ival);
    }
    else if (type == GTK_TYPE_CHECK_MENU_ITEM)
    {
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widget), ival);
    }
    else if (type == GTK_TYPE_COMBO_BOX)
    {
        GtkTreeModel *store;
        GtkTreeIter iter;
        gchar *shortOpt;
        gdouble ivalue;
        gboolean foundit = FALSE;

        store = gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
        if (gtk_tree_model_get_iter_first (store, &iter))
        {
            do
            {
                gtk_tree_model_get(store, &iter, 2, &shortOpt, -1);
                if (strcasecmp(shortOpt, str) == 0)
                {
                    gtk_combo_box_set_active_iter (
                        GTK_COMBO_BOX(widget), &iter);
                    g_free(shortOpt);
                    foundit = TRUE;
                    break;
                }
                g_free(shortOpt);
            } while (gtk_tree_model_iter_next (store, &iter));
        }
        if (!foundit && gtk_tree_model_get_iter_first (store, &iter))
        {
            do
            {
                gtk_tree_model_get(store, &iter, 3, &ivalue, -1);
                if ((gint)ivalue == ival || ivalue == dval)
                {
                    gtk_combo_box_set_active_iter (
                        GTK_COMBO_BOX(widget), &iter);
                    foundit = TRUE;
                    break;
                }
            } while (gtk_tree_model_iter_next (store, &iter));
        }
        if (!foundit)
        {
            if (gtk_combo_box_get_has_entry(GTK_COMBO_BOX(widget)))
            {
                GtkEntry *entry = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(widget)));
                if (entry)
                {
                    gtk_entry_set_text (entry, str);
                }
                else
                {
                    gtk_combo_box_set_active (GTK_COMBO_BOX(widget), 0);
                }
            }
            else
            {
                gtk_combo_box_set_active (GTK_COMBO_BOX(widget), 0);
            }
        }
    }
    else if (type == GTK_TYPE_SPIN_BUTTON)
    {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), dval);
    }
    else if (type == GTK_TYPE_SCALE)
    {
        gtk_range_set_value(GTK_RANGE(widget), dval);
    }
    else if (type == GTK_TYPE_SCALE_BUTTON)
    {
        gtk_scale_button_set_value(GTK_SCALE_BUTTON(widget), dval);
    }
    else if (type == GTK_TYPE_TEXT_VIEW)
    {
        static int text_view_busy = 0;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(
                                                GTK_TEXT_VIEW(widget));
        if (!text_view_busy)
        {
            text_view_busy = 1;
            gtk_text_buffer_set_text (buffer, str, -1);
            text_view_busy = 0;
        }
    }
    else if (type == GTK_TYPE_LABEL)
    {
        gtk_label_set_markup (GTK_LABEL(widget), str);
    }
    else if (type == GTK_TYPE_FILE_CHOOSER_BUTTON)
    {
        GtkFileChooserAction act;
        act = gtk_file_chooser_get_action(GTK_FILE_CHOOSER(widget));

        if (str[0] == 0)
        {
            // Do nothing
            ;
        }
        else if (act == GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER ||
                 act == GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER)
        {
            gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(widget), str);
        }
        else if (act == GTK_FILE_CHOOSER_ACTION_SAVE)
        {
            gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(widget), str);
        }
        else
        {
            if (g_file_test(str, G_FILE_TEST_IS_DIR))
            {
                gtk_file_chooser_set_current_folder(
                    GTK_FILE_CHOOSER(widget), str);
            }
            else if (g_file_test(str, G_FILE_TEST_EXISTS))
            {
                gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(widget), str);
            }
            else
            {
                gchar *dirname;

                dirname = g_path_get_dirname(str);
                gtk_file_chooser_set_current_folder(
                    GTK_FILE_CHOOSER(widget), dirname);
                g_free(dirname);
            }
        }
    }
    else
    {
        g_warning("Attempt to set unknown widget type, name %s", name);
    }
    g_free(tmp);
}

int
ghb_ui_update_from_settings(signal_user_data_t *ud, const gchar *name, const GhbValue *settings)
{
    GObject *object;
    GhbValue * value;

    g_debug("ghb_ui_update_from_settings() %s", name);
    if (name == NULL)
        return 0;
    value = ghb_dict_get_value(settings, name);
    if (value == NULL)
        return 0;
    object = GHB_OBJECT(ud->builder, name);
    if (object == NULL)
    {
        g_debug("Failed to find widget for key: %s\n", name);
        return -1;
    }
    ghb_update_widget((GtkWidget*)object, value);
    // Its possible the value hasn't changed. Since settings are only
    // updated when the value changes, I'm initializing settings here as well.
    ghb_widget_to_setting(ud->settings, (GtkWidget*)object);
    return 0;
}

int
ghb_ui_update(signal_user_data_t *ud, const gchar *name, const GhbValue *value)
{
    GObject *object;

    g_debug("ghb_ui_update() %s", name);
    if (name == NULL || value == NULL)
        return 0;
    object = GHB_OBJECT(ud->builder, name);
    if (object == NULL)
    {
        g_debug("Failed to find widget for key: %s\n", name);
        return -1;
    }
    ghb_update_widget((GtkWidget*)object, value);
    // Its possible the value hasn't changed. Since settings are only
    // updated when the value changes, I'm initializing settings here as well.
    ghb_widget_to_setting(ud->settings, (GtkWidget*)object);
    return 0;
}

int
ghb_ui_settings_update(
    signal_user_data_t *ud,
    GhbValue *settings,
    const gchar *name,
    const GhbValue *value)
{
    GObject *object;

    g_debug("ghb_ui_update() %s", name);
    if (name == NULL || value == NULL)
        return 0;
    object = GHB_OBJECT(ud->builder, name);
    if (object == NULL)
    {
        g_debug("Failed to find widget for key: %s\n", name);
        return -1;
    }
    ghb_update_widget((GtkWidget*)object, value);
    // Its possible the value hasn't changed. Since settings are only
    // updated when the value changes, I'm initializing settings here as well.
    ghb_widget_to_setting(settings, (GtkWidget*)object);
    return 0;
}

