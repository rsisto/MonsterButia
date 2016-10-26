#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (C) 2007,2008,2009 Chris Ball, based on Collabora's
# "hellomesh" demo.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

"""Pippy Activity: A simple Python programming activity ."""
from __future__ import with_statement
import gtk
import logging
import pango
import vte
import re
import os
import gobject
import time
import string

from port.style import font_zoom
from signal import SIGTERM
from gettext import gettext as _

from sugar.activity import activity
#xop from activity import ViewSourceActivity, TARGET_TYPE_TEXT
from activity import TARGET_TYPE_TEXT
from sugar.activity.activity import ActivityToolbox, \
     EditToolbar, get_bundle_path, get_bundle_name
from sugar.graphics import style
from sugar.graphics.toolbutton import ToolButton

import groupthink.sugar_tools
import groupthink.gtk_tools

text_buffer = None
# magic prefix to use utf-8 source encoding
PYTHON_PREFIX = """#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
#BUTIALO_REQUIRE = "require 'butialo'; setfenv(1,_G)"


OLD_TOOLBAR = False
try:
    from sugar.graphics.toolbarbox import ToolbarBox, ToolbarButton
    from sugar.activity.widgets import StopButton
except ImportError:
    OLD_TOOLBAR = True

# get screen sizes
SIZE_X = gtk.gdk.screen_width()
SIZE_Y = gtk.gdk.screen_height()

groupthink_mimetype = 'pickle/groupthink-butialo'

#xop class ButialoActivity(ViewSourceActivity, groupthink.sugar_tools.GroupActivity):
class ButialoActivity(groupthink.sugar_tools.GroupActivity):
    """Pippy Activity as specified in activity.info"""
    def early_setup(self):
        global text_buffer
        import gtksourceview2
        text_buffer = gtksourceview2.Buffer()

    def initialize_snippets_cb(self, Button):
	self.model.clear()
        direntry = {"name": "Devices",
                    "path": "" }
        olditerdev = self.model.insert_before(None, None)
        self.model.set_value(olditerdev, 0, direntry)
        self.model.set_value(olditerdev, 1, direntry["name"])

        root = os.popen("./lua parse_bobot_tree.lua")
        for d in root.readlines():
            d=d.strip()

            words = string.split(d) 
    	    #for w in words:
            #	print '%%%%%%%%%'+w

            words = string.split(d, None, 1)
            word1=words[0]
            word2=words[1]

            if word1 != '>':
                    word1device = word1.capitalize()
                    if word2 == 'Y':
		    	direntry = {"name": word1device + '  :-)',
		                "path": "" } # era d
                    else:
		    	direntry = {"name": word1device,
		                "path": "" } # era d
		    olditer = self.model.insert_before(olditerdev, None)
		    self.model.set_value(olditer, 0, direntry)
		    self.model.set_value(olditer, 1, direntry["name"])
            else:
                _file=word2.strip()
                entry = {"name": _file,
                         "path": word1device + "." + _file+" "}
                _iter = self.model.insert_before(olditer, None)
                self.model.set_value(_iter, 0, entry)
                self.model.set_value(_iter, 1, entry["name"])
                
        direntry = {"name": "Lua Language",
                    "path": "" }

        olditer = self.model.insert_before(None, None)
        self.model.set_value(olditer, 0, direntry)
        self.model.set_value(olditer, 1, direntry["name"])

        entry = {"name": "if ... then ... else ... end",
                 "path": "\nif i == 1 then\n  \nelse\n  \nend\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        entry = {"name": "for ... do ... end",
                 "path": "\nfor i = 1,10 do\n  \nend\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        entry = {"name": "while ... do ... end",
                 "path": "\nwhile a == 1 do\n  \nend\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        entry = {"name": "repeat ... until ...",
                 "path": "\nrepeat\n  \nuntil a ~= 1\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        entry = {"name": "function ... ",
                 "path": "\nlocal f = function f(a, b)\n  \n  return a,b\nend\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        entry = {"name": "--comment ",
                 "path": "--comentario\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])


        direntry = {"name": "Events",
                    "path": "" }
        olditer = self.model.insert_before(None, None)
        self.model.set_value(olditer, 0, direntry)
        self.model.set_value(olditer, 1, direntry["name"])

        entry = {"name": "add( func, op, ref, action, [hyst], [name] )",
                 "path": "\nlocal name = events.add( func, '>', 0, action )\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        entry = {"name": "remove( name )",
                 "path": "\nevents.remove( name )\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        entry = {"name": "go()",
                 "path": "\nevents.go()\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        entry = {"name": "stop()",
                 "path": "\nevents.stop()\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])


        direntry = {"name": "Util",
                    "path": "" }
        olditer = self.model.insert_before(None, None)
        self.model.set_value(olditer, 0, direntry)
        self.model.set_value(olditer, 1, direntry["name"])

        entry = {"name": "get_time()",
                 "path": "\nlocal time = util.get_time()\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])

        entry = {"name": "wait()",
                 "path": "\nutil.wait( 1 )\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])

        entry = {"name": "new_array()",
                 "path": "\nlocal array = util.new_array()\n"}
        _iter = self.model.insert_before(olditer, None)
        self.model.set_value(_iter, 0, entry)
        self.model.set_value(_iter, 1, entry["name"])
        
#        for d in sorted(os.listdir(root)):
#            if not os.path.isdir(os.path.join(root, d)):
#                continue  # skip non-dirs
#            direntry = {"name": _(d.capitalize()),
#                        "path": os.path.join(root, d) + "/"}
#            olditer = self.model.insert_before(None, None)
#            self.model.set_value(olditer, 0, direntry)
#            self.model.set_value(olditer, 1, direntry["name"])
#
#            for _file in sorted(os.listdir(os.path.join(root, d))):
#                if _file.endswith('~'):
#                    continue  # skip emacs backups
#                entry = {"name": _(_file.capitalize()),
#                         "path": os.path.join(root, d, _file)}
#                _iter = self.model.insert_before(olditer, None)
#                self.model.set_value(_iter, 0, entry)
#                self.model.set_value(_iter, 1, entry["name"])

        #treeview.expand_all()


    def initialize_display(self):
        self._logger = logging.getLogger('butialo-activity')

        # Top toolbar with share and close buttons:

        if OLD_TOOLBAR:
            activity_toolbar = self.toolbox.get_activity_toolbar()
        else:
            activity_toolbar = self.activity_button.page

        # add 'make bundle' entry to 'keep' palette.
        palette = activity_toolbar.keep.get_palette()
        # XXX: should clear out old palette entries?
        from sugar.graphics.menuitem import MenuItem
        from sugar.graphics.icon import Icon
        menu_item = MenuItem(_('As Butialo Document'))
        menu_item.set_image(Icon(file=('%s/activity/activity-icon.svg' %
                                       get_bundle_path()),
                                 icon_size=gtk.ICON_SIZE_MENU))
        menu_item.connect('activate', self.keepbutton_cb)
        palette.menu.append(menu_item)
        menu_item.show()
        #xop guardar como bundle
        #menu_item = MenuItem(_('As Activity Bundle'))
        #menu_item.set_image(Icon(file=('%s/activity/activity-default.svg' %
        #                               get_bundle_path()),
        #                         icon_size=gtk.ICON_SIZE_MENU))
        #menu_item.connect('activate', self.makebutton_cb)
        #palette.menu.append(menu_item)
        #menu_item.show()

        self._edit_toolbar = activity.EditToolbar()

        if OLD_TOOLBAR:
            activity_toolbar = gtk.Toolbar()
            self.toolbox.add_toolbar(_('Actions'), activity_toolbar)
            self.toolbox.set_current_toolbar(1)
            self.toolbox.add_toolbar(_('Edit'), self._edit_toolbar)
        else:
            edit_toolbar_button = ToolbarButton()
            edit_toolbar_button.set_page(self._edit_toolbar)
            edit_toolbar_button.props.icon_name = 'toolbar-edit'
            edit_toolbar_button.props.label = _('Edit')
            self.get_toolbar_box().toolbar.insert(edit_toolbar_button, -1)

        self._edit_toolbar.show()

        self._edit_toolbar.undo.connect('clicked', self.__undobutton_cb)
        self._edit_toolbar.redo.connect('clicked', self.__redobutton_cb)
        self._edit_toolbar.copy.connect('clicked', self.__copybutton_cb)
        self._edit_toolbar.paste.connect('clicked', self.__pastebutton_cb)

        if OLD_TOOLBAR:
            actions_toolbar = activity_toolbar
        else:
            actions_toolbar = self.get_toolbar_box().toolbar

        # The "go" button
        goicon_bw = gtk.Image()
        goicon_bw.set_from_file("%s/icons/run_bw.svg" % os.getcwd())
        goicon_color = gtk.Image()
        goicon_color.set_from_file("%s/icons/run_color.svg" % os.getcwd())
        gobutton = ToolButton(label=_("_Run!"))
        gobutton.props.accelerator = _('<alt>r')
        gobutton.set_icon_widget(goicon_bw)
        gobutton.set_tooltip("Run")
        gobutton.connect('clicked', self.flash_cb, dict({'bw': goicon_bw,
            'color': goicon_color}))
        gobutton.connect('clicked', self.gobutton_cb)
        actions_toolbar.insert(gobutton, -1)

        # The "stop" button
        stopicon_bw = gtk.Image()
        stopicon_bw.set_from_file("%s/icons/stopit_bw.svg" % os.getcwd())
        stopicon_color = gtk.Image()
        stopicon_color.set_from_file("%s/icons/stopit_color.svg" % os.getcwd())
        stopbutton = ToolButton(label=_("_Stop"))
        stopbutton.props.accelerator = _('<alt>s')
        stopbutton.set_icon_widget(stopicon_bw)
        stopbutton.connect('clicked', self.flash_cb, dict({'bw': stopicon_bw,
            'color': stopicon_color}))
        stopbutton.connect('clicked', self.stopbutton_cb)
        stopbutton.set_tooltip("Stop Running")
        actions_toolbar.insert(stopbutton, -1)

        # The "refresh" button
        refreshicon_bw = gtk.Image()
        refreshicon_bw.set_from_file("%s/icons/butiaoff.svg" % os.getcwd())
        refreshicon_color = gtk.Image()
        refreshicon_color.set_from_file("%s/icons/butiaon.svg" % os.getcwd())
        refreshbutton = ToolButton(label=_("_Reload"))
        refreshbutton.props.accelerator = _('<alt>d')
        refreshbutton.set_icon_widget(refreshicon_bw)
        refreshbutton.connect('clicked', self.flash_cb, dict({'bw': refreshicon_bw,
            'color': refreshicon_color}))
        refreshbutton.connect('clicked', self.initialize_snippets_cb)
        refreshbutton.set_tooltip("Reload devices")
        actions_toolbar.insert(refreshbutton, -1)

        # The "clear" button
        #clearicon_bw = gtk.Image()
        #clearicon_bw.set_from_file("%s/icons/eraser_bw.svg" % os.getcwd())
        #clearicon_color = gtk.Image()
        #clearicon_color.set_from_file("%s/icons/eraser_color.svg" %
        #                              os.getcwd())
        #clearbutton = ToolButton(label=_("_Clear"))
        #clearbutton.props.accelerator = _('<alt>c')
        #clearbutton.set_icon_widget(clearicon_bw)
        #clearbutton.connect('clicked', self.clearbutton_cb)
        #clearbutton.connect('clicked', self.flash_cb, dict({'bw': clearicon_bw,
        #    'color': clearicon_color}))
        #clearbutton.set_tooltip("Clear")
        #actions_toolbar.insert(clearbutton, -1)

        # The "beautify" button
        beautifyicon_bw = gtk.Image()
        beautifyicon_bw.set_from_file("%s/icons/beautifyicon_bw.svg" % os.getcwd())
        beautifyicon_color = gtk.Image()
        beautifyicon_color.set_from_file("%s/icons/beautifyicon_color.svg" %
                                      os.getcwd())
        beautifybutton = ToolButton(label=_("_Auto-format"))
        beautifybutton.props.accelerator = _('<alt>f')
        beautifybutton.set_icon_widget(beautifyicon_bw)
        beautifybutton.connect('clicked', self.beautifybutton_cb)
        beautifybutton.connect('clicked', self.flash_cb, dict({'bw': beautifyicon_bw,
            'color': beautifyicon_color}))
        beautifybutton.set_tooltip("Auto-format")
        actions_toolbar.insert(beautifybutton, -1)

        activity_toolbar.show()

        if not OLD_TOOLBAR:
            separator = gtk.SeparatorToolItem()
            separator.props.draw = False
            separator.set_expand(True)
            self.get_toolbar_box().toolbar.insert(separator, -1)
            separator.show()

            stop = StopButton(self)
            self.get_toolbar_box().toolbar.insert(stop, -1)

        # Main layout.
        self.hpane = gtk.HPaned()
        self.vpane = gtk.VPaned()

        # The sidebar.
        self.sidebar = gtk.VBox()
        self.model = gtk.TreeStore(gobject.TYPE_PYOBJECT, gobject.TYPE_STRING)
        treeview = gtk.TreeView(self.model)
        cellrenderer = gtk.CellRendererText()
        treecolumn = gtk.TreeViewColumn(_("Available"), cellrenderer, text=1)
        treeview.get_selection().connect("changed", self.selection_cb)
        treeview.append_column(treecolumn)
        treeview.set_size_request(int(SIZE_X * 0.3), SIZE_Y)

        # Create scrollbars around the view.
        scrolled = gtk.ScrolledWindow()
        scrolled.add(treeview)
        self.sidebar.pack_start(scrolled)
        self.hpane.add1(self.sidebar)

        #root = os.path.join(get_bundle_path(), 'data')

	#initialize snippets 
	self.initialize_snippets_cb(None)

        # Source buffer
        import gtksourceview2
        global text_buffer
        lang_manager = gtksourceview2.language_manager_get_default()
        if hasattr(lang_manager, 'list_languages'):
            langs = lang_manager.list_languages()
        else:
            lang_ids = lang_manager.get_language_ids()
            langs = [lang_manager.get_language(lang_id)
                     for lang_id in lang_ids]
        for lang in langs:
            for m in lang.get_mime_types():
                if m == "text/x-lua":
                    text_buffer.set_language(lang)

        if hasattr(text_buffer, 'set_highlight'):
            text_buffer.set_highlight(True)
        else:
            text_buffer.set_highlight_syntax(True)

        # The GTK source view window
        self.text_view = gtksourceview2.View(text_buffer)
        self.text_view.set_size_request(0, int(SIZE_Y * 0.5))
        self.text_view.set_editable(True)
        self.text_view.set_cursor_visible(True)
        self.text_view.set_show_line_numbers(True)
        self.text_view.set_wrap_mode(gtk.WRAP_CHAR)
        self.text_view.set_insert_spaces_instead_of_tabs(True)
        self.text_view.set_tab_width(2)
        self.text_view.set_auto_indent(True)
        self.text_view.modify_font(pango.FontDescription("Monospace " +
            str(font_zoom(style.FONT_SIZE))))

        # We could change the color theme here, if we want to.
        #mgr = gtksourceview2.style_manager_get_default()
        #style_scheme = mgr.get_scheme('kate')
        #self.text_buffer.set_style_scheme(style_scheme)

        codesw = gtk.ScrolledWindow()
        codesw.set_policy(gtk.POLICY_AUTOMATIC,
                      gtk.POLICY_AUTOMATIC)
        codesw.add(self.text_view)
        self.vpane.add1(codesw)

        # An hbox to hold the vte window and its scrollbar.
        outbox = gtk.HBox()

        # The vte python window
        self._vte = vte.Terminal()
        self._vte.set_encoding('utf-8')
        self._vte.set_size(30, 5)
        font = 'Monospace ' + str(font_zoom(style.FONT_SIZE))
        self._vte.set_font(pango.FontDescription(font))
        self._vte.set_colors(gtk.gdk.color_parse('#000000'),
                             gtk.gdk.color_parse('#E7E7E7'),
                             [])
        self._vte.connect('child_exited', self.child_exited_cb)
        self._child_exited_handler = None
        self._vte.drag_dest_set(gtk.DEST_DEFAULT_ALL,
                                [("text/plain", 0, TARGET_TYPE_TEXT)],
                                gtk.gdk.ACTION_COPY)
        self._vte.connect('drag_data_received', self.vte_drop_cb)
        outbox.pack_start(self._vte)

        outsb = gtk.VScrollbar(self._vte.get_adjustment())
        outsb.show()
        outbox.pack_start(outsb, False, False, 0)
        self.vpane.add2(outbox)
        self.hpane.add2(self.vpane)
        return self.hpane

    def when_shared(self):
        self.hpane.remove(self.hpane.get_child1())
        global text_buffer
        self.cloud.sharefield = \
            groupthink.gtk_tools.TextBufferSharePoint(text_buffer)
        # HACK : There are issues with undo/redoing while in shared
        # mode. So disable the 'undo' and 'redo' buttons when the activity
        # is shared.
        self._edit_toolbar.undo.set_sensitive(False)
        self._edit_toolbar.redo.set_sensitive(False)

    def vte_drop_cb(self, widget, context, x, y, selection, targetType, time):
        if targetType == TARGET_TYPE_TEXT:
            self._vte.feed_child(selection.data)

    def selection_cb(self, column):
        self.save()
        model, _iter = column.get_selected()
        value = model.get_value(_iter, 0)
        #self._logger.debug("clicked! %s" % value['path'])
        global text_buffer
        text_buffer.insert_at_cursor(value['path']) 
#        _file = open(value['path'], 'r')
#        lines = _file.readlines()
#        text_buffer.set_text("".join(lines))
#        self.metadata['title'] = value['name']
        self.stopbutton_cb(None)
        self._reset_vte()
        self.text_view.grab_focus()

    def timer_cb(self, button, icons):
        button.set_icon_widget(icons['bw'])
        button.show_all()
        return False

    def flash_cb(self, button, icons):
        button.set_icon_widget(icons['color'])
        button.show_all()
        gobject.timeout_add(400, self.timer_cb, button, icons)

#    def clearbutton_cb(self, button):
#        self.save()
#        global text_buffer
#        text_buffer.set_text("")
#        self.metadata['title'] = _('%s Activity') % get_bundle_name()
#        self.stopbutton_cb(None)
#        self._reset_vte()
#        self.text_view.grab_focus()

    def beautifybutton_cb(self, button):
        self.save()
        butialo_app_name = '%s/tmp/butialo_app.lua' % self.get_activity_root()
        self._write_text_buffer(butialo_app_name)
        formatted = os.popen("./lua beaut.lua %s" % butialo_app_name)
	#formatted = "./lua codeformatter.lua --file %s" % butialo_app_name
        global text_buffer
        text_buffer.set_text(formatted.read())
        self.metadata['title'] = _('%s Activity') % get_bundle_name()
        self.stopbutton_cb(None)
        #self._reset_vte()
        self.text_view.grab_focus()

    def _write_text_buffer(self, filename):
        global text_buffer
        start, end = text_buffer.get_bounds()
        text = text_buffer.get_text(start, end)

        with open(filename, 'w') as f:
            # write utf-8 coding prefix if there's not already one
            #if re.match(r'coding[:=]\s*([-\w.]+)',
            #            '\n'.join(text.splitlines()[:2])) is None:
            #    f.write(PYTHON_PREFIX)
            #f.write(BUTIALO_REQUIRE)
            #f.write("\n")
            for line in text:
                f.write(line)

    def _reset_vte(self):
        self._vte.grab_focus()
        self._vte.feed("\x1B[H\x1B[J\x1B[0;39m")

    def __undobutton_cb(self, button):
        global text_buffer
        if text_buffer.can_undo():
            text_buffer.undo()

    def __redobutton_cb(self, button):
        global text_buffer
        if text_buffer.can_redo():
            text_buffer.redo()

    def __copybutton_cb(self, button):
        global text_buffer
        text_buffer.copy_clipboard(gtk.Clipboard())

    def __pastebutton_cb(self, button):
        global text_buffer
        text_buffer.paste_clipboard(gtk.Clipboard(), None, True)

    def gobutton_cb(self, button):
        from shutil import copy2
        self.stopbutton_cb(button)  # try stopping old code first.
        #self._reset_vte()

        # FIXME: We're losing an odd race here
        # gtk.main_iteration(block=False)

        # write activity.py here too, to support pippy-based activities.
        #copy2('%s/activity.py' % get_bundle_path(),
        #      '%s/tmp/activity.py' % self.get_activity_root())
        copy2('%s/butialo.lua' % get_bundle_path(),
              '%s/tmp/butialo.lua' % self.get_activity_root())

	#if selected code, run selection
        global text_buffer
	butialo_app_name = ""
	if text_buffer.get_has_selection():
		start,end=text_buffer.get_selection_bounds()
		text = text_buffer.get_text(start, end)
		butialo_app_name='%s/tmp/butialo_snippet.lua' % self.get_activity_root()
		f = open(butialo_app_name, 'w')		 
		f.write("print (%s)" % text)
		f.close()
		self._vte.feed("\x1B[H\x1B[J\x1B[0;39m")
		self._vte.feed("--Evaluating selected code:\r\n")
		self._vte.feed("--print (%s)\r\n" % text)
		self._vte.feed("-------------------------\r\n")
	else:
	        self._reset_vte()
		butialo_app_name = '%s/tmp/butialo_app.lua' % self.get_activity_root()
		self._write_text_buffer(butialo_app_name)

	self._pid = self._vte.fork_command(
	    argv=["/bin/sh", "-c",
	          "./lua butialo.lua %s; sleep 1" % butialo_app_name],
	    #envv=["PYTHONPATH=%s/library:%s" % (get_bundle_path(),
	    #                                    os.getenv("PYTHONPATH", ""))],
	    envv=[],
	    directory=get_bundle_path())

    def stopbutton_cb(self, button):
        try:
            os.kill(self._pid, SIGTERM)
        except:
            pass  # process must already be dead.

    def keepbutton_cb(self, __):
        self.copy()

    def child_exited_cb(self, *args):
        """Called whenever a child exits.  If there's a handler, run it."""
        h, self._child_exited_handler = self._child_exited_handler, None
        if h is not None:
            h()

    def dismiss_alert_cb(self, alert, response_id):
        self.remove_alert(alert)

    def save_to_journal(self, file_path, cloudstring):
        _file = open(file_path, 'w')
        if not self._shared_activity:
            self.metadata['mime_type'] = 'text/x-lua'
            global text_buffer
            start, end = text_buffer.get_bounds()
            text = text_buffer.get_text(start, end)
            _file.write(text)
        else:
            self.metadata['mime_type'] = groupthink_mimetype
            _file.write(cloudstring)

    def load_from_journal(self, file_path):
        if self.metadata['mime_type'] == 'text/x-lua':
            text = open(file_path).read()
            # discard the '#!/usr/bin/python' and 'coding: utf-8' lines,
            # if present
            #text = re.sub(r'^' + re.escape(BUTIALO_REQUIRE), '', text)
            global text_buffer
            text_buffer.set_text(text)
        elif self.metadata['mime_type'] == groupthink_mimetype:
            return open(file_path).read()


if __name__ == '__main__':
    from gettext import gettext as _
    import sys
    if False:  # change this to True to test within Pippy
        sys.argv = sys.argv + ['-d', '/tmp', 'Butialo',
                               '/home/olpc/pippy_app.py']
    #print _("Working..."),
    #sys.stdout.flush()
    #xop main()
    #print _("done!")
    sys.exit(0)
