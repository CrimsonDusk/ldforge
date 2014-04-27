/*
 *  LDForge: LDraw parts authoring CAD
 *  Copyright (C) 2013, 2014 Santeri Piippo
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QInputDialog>

#include "mainWindow.h"
#include "ldDocument.h"
#include "editHistory.h"
#include "configDialog.h"
#include "addObjectDialog.h"
#include "miscallenous.h"
#include "glRenderer.h"
#include "dialogs.h"
#include "primitives.h"
#include "radioGroup.h"
#include "colors.h"
#include "glCompiler.h"
#include "ui_newpart.h"

extern_cfg (Bool,		gl_wireframe);
extern_cfg (Bool,		gl_colorbfc);
extern_cfg (String,	ld_defaultname);
extern_cfg (String,	ld_defaultuser);
extern_cfg (Int,		ld_defaultlicense);
extern_cfg (Bool,		gl_drawangles);
extern_cfg (Bool,		gl_randomcolors)

// =============================================================================
//
DEFINE_ACTION (New, CTRL_SHIFT (N))
{
	QDialog* dlg = new QDialog (g_win);
	Ui::NewPartUI ui;
	ui.setupUi (dlg);

	String authortext = ld_defaultname;

	if (not ld_defaultuser.isEmpty())
		authortext.append (format (" [%1]", ld_defaultuser));

	ui.le_author->setText (authortext);

	switch (ld_defaultlicense)
	{
		case 0:
			ui.rb_license_ca->setChecked (true);
			break;

		case 1:
			ui.rb_license_nonca->setChecked (true);
			break;

		case 2:
			ui.rb_license_none->setChecked (true);
			break;

		default:
			QMessageBox::warning (null, "Warning",
				format ("Unknown ld_defaultlicense value %1!", ld_defaultlicense));
			break;
	}

	if (dlg->exec() == QDialog::Rejected)
		return;

	newFile();

	const LDBFC::Statement BFCType =
		ui.rb_bfc_ccw->isChecked() ? LDBFC::CertifyCCW :
		ui.rb_bfc_cw->isChecked()  ? LDBFC::CertifyCW : LDBFC::NoCertify;

	const String license =
		ui.rb_license_ca->isChecked()    ? g_CALicense :
		ui.rb_license_nonca->isChecked() ? g_nonCALicense : "";

	getCurrentDocument()->addObjects (
	{
		new LDComment (ui.le_title->text()),
		new LDComment ("Name: <untitled>.dat"),
		new LDComment (format ("Author: %1", ui.le_author->text())),
		new LDComment (format ("!LDRAW_ORG Unofficial_Part")),
		(license != "" ? new LDComment (license) : null),
		new LDEmpty,
		new LDBFC (BFCType),
		new LDEmpty,
	});

	doFullRefresh();
}

// =============================================================================
//
DEFINE_ACTION (NewFile, CTRL (N))
{
	newFile();
}

// =============================================================================
//
DEFINE_ACTION (Open, CTRL (O))
{
	String name = QFileDialog::getOpenFileName (g_win, "Open File", "", "LDraw files (*.dat *.ldr)");

	if (name.length() == 0)
		return;

	openMainFile (name);
}

// =============================================================================
//
DEFINE_ACTION (Save, CTRL (S))
{
	save (getCurrentDocument(), false);
}

// =============================================================================
//
DEFINE_ACTION (SaveAs, CTRL_SHIFT (S))
{
	save (getCurrentDocument(), true);
}

// =============================================================================
//
DEFINE_ACTION (SaveAll, CTRL (L))
{
	for (LDDocument* file : g_loadedFiles)
	{
		if (file->isImplicit())
			continue;

		save (file, false);
	}
}

// =============================================================================
//
DEFINE_ACTION (Close, CTRL (W))
{
	if (not getCurrentDocument()->isSafeToClose())
		return;

	delete getCurrentDocument();
}

// =============================================================================
//
DEFINE_ACTION (CloseAll, 0)
{
	if (not safeToCloseAll())
		return;

	closeAll();
}

// =============================================================================
//
DEFINE_ACTION (Settings, 0)
{
	(new ConfigDialog)->exec();
}

// =============================================================================
//
DEFINE_ACTION (SetLDrawPath, 0)
{
	(new LDrawPathDialog (true))->exec();
}

// =============================================================================
//
DEFINE_ACTION (Exit, CTRL (Q))
{
	exit (0);
}

// =============================================================================
//
DEFINE_ACTION (NewSubfile, 0)
{
	AddObjectDialog::staticDialog (LDObject::ESubfile, null);
}

// =============================================================================
//
DEFINE_ACTION (NewLine, 0)
{
	AddObjectDialog::staticDialog (LDObject::ELine, null);
}

// =============================================================================
//
DEFINE_ACTION (NewTriangle, 0)
{
	AddObjectDialog::staticDialog (LDObject::ETriangle, null);
}

// =============================================================================
//
DEFINE_ACTION (NewQuad, 0)
{
	AddObjectDialog::staticDialog (LDObject::EQuad, null);
}

// =============================================================================
//
DEFINE_ACTION (NewCLine, 0)
{
	AddObjectDialog::staticDialog (LDObject::ECondLine, null);
}

// =============================================================================
//
DEFINE_ACTION (NewComment, 0)
{
	AddObjectDialog::staticDialog (LDObject::EComment, null);
}

// =============================================================================
//
DEFINE_ACTION (NewBFC, 0)
{
	AddObjectDialog::staticDialog (LDObject::EBFC, null);
}

// =============================================================================
//
DEFINE_ACTION (NewVertex, 0)
{
	AddObjectDialog::staticDialog (LDObject::EVertex, null);
}

// =============================================================================
//
DEFINE_ACTION (Edit, 0)
{
	if (selection().size() != 1)
		return;

	LDObject* obj = selection() [0];
	AddObjectDialog::staticDialog (obj->type(), obj);
}

// =============================================================================
//
DEFINE_ACTION (Help, KEY (F1))
{
}

// =============================================================================
//
DEFINE_ACTION (About, 0)
{
	AboutDialog().exec();
}

// =============================================================================
//
DEFINE_ACTION (AboutQt, 0)
{
	QMessageBox::aboutQt (g_win);
}

// =============================================================================
//
DEFINE_ACTION (SelectAll, CTRL (A))
{
	for (LDObject* obj : getCurrentDocument()->objects())
		obj->select();

	updateSelection();
}

// =============================================================================
//
DEFINE_ACTION (SelectByColor, CTRL_SHIFT (A))
{
	int colnum = getSelectedColor();

	if (colnum == -1)
		return; // no consensus on color

	getCurrentDocument()->clearSelection();

	for (LDObject* obj : getCurrentDocument()->objects())
		if (obj->color() == colnum)
			obj->select();

	updateSelection();
}

// =============================================================================
//
DEFINE_ACTION (SelectByType, 0)
{
	if (selection().isEmpty())
		return;

	LDObject::Type type = getUniformSelectedType();

	if (type == LDObject::EUnidentified)
		return;

	// If we're selecting subfile references, the reference filename must also
	// be uniform.
	String refName;

	if (type == LDObject::ESubfile)
	{
		refName = static_cast<LDSubfile*> (selection()[0])->fileInfo()->name();

		for (LDObject* obj : selection())
			if (static_cast<LDSubfile*> (obj)->fileInfo()->name() != refName)
				return;
	}

	getCurrentDocument()->clearSelection();

	for (LDObject* obj : getCurrentDocument()->objects())
	{
		if (obj->type() != type)
			continue;

		if (type == LDObject::ESubfile && static_cast<LDSubfile*> (obj)->fileInfo()->name() != refName)
			continue;

		obj->select();
	}

	updateSelection();
}

// =============================================================================
//
DEFINE_ACTION (GridCoarse, 0)
{
	grid = Grid::Coarse;
	updateGridToolBar();
}

DEFINE_ACTION (GridMedium, 0)
{
	grid = Grid::Medium;
	updateGridToolBar();
}

DEFINE_ACTION (GridFine, 0)
{
	grid = Grid::Fine;
	updateGridToolBar();
}

// =============================================================================
//
DEFINE_ACTION (ResetView, CTRL (0))
{
	R()->resetAngles();
	R()->update();
}

// =============================================================================
//
DEFINE_ACTION (InsertFrom, 0)
{
	String fname = QFileDialog::getOpenFileName();
	int idx = getInsertionPoint();

	if (not fname.length())
		return;

	QFile f (fname);

	if (not f.open (QIODevice::ReadOnly))
	{
		critical (format ("Couldn't open %1 (%2)", fname, f.errorString()));
		return;
	}

	LDObjectList objs = loadFileContents (&f, null);

	getCurrentDocument()->clearSelection();

	for (LDObject* obj : objs)
	{
		getCurrentDocument()->insertObj (idx, obj);
		obj->select();
		R()->compileObject (obj);

		idx++;
	}

	refresh();
	scrollToSelection();
}

// =============================================================================
//
DEFINE_ACTION (ExportTo, 0)
{
	if (selection().isEmpty())
		return;

	String fname = QFileDialog::getSaveFileName();

	if (fname.length() == 0)
		return;

	QFile file (fname);

	if (not file.open (QIODevice::WriteOnly | QIODevice::Text))
	{
		critical (format ("Unable to open %1 for writing (%2)", fname, file.errorString()));
		return;
	}

	for (LDObject* obj : selection())
	{
		String contents = obj->asText();
		QByteArray data = contents.toUtf8();
		file.write (data, data.size());
		file.write ("\r\n", 2);
	}
}

// =============================================================================
//
DEFINE_ACTION (InsertRaw, 0)
{
	int idx = getInsertionPoint();

	QDialog* const dlg = new QDialog;
	QVBoxLayout* const layout = new QVBoxLayout;
	QTextEdit* const te_edit = new QTextEdit;
	QDialogButtonBox* const bbx_buttons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	layout->addWidget (te_edit);
	layout->addWidget (bbx_buttons);
	dlg->setLayout (layout);
	dlg->setWindowTitle (APPNAME " - Insert Raw");
	dlg->connect (bbx_buttons, SIGNAL (accepted()), dlg, SLOT (accept()));
	dlg->connect (bbx_buttons, SIGNAL (rejected()), dlg, SLOT (reject()));

	if (dlg->exec() == QDialog::Rejected)
		return;

	getCurrentDocument()->clearSelection();

	for (String line : String (te_edit->toPlainText()).split ("\n"))
	{
		LDObject* obj = parseLine (line);

		getCurrentDocument()->insertObj (idx, obj);
		obj->select();
		idx++;
	}

	refresh();
	scrollToSelection();
}

// =============================================================================
//
DEFINE_ACTION (Screenshot, 0)
{
	setlocale (LC_ALL, "C");

	int w, h;
	uchar* imgdata = R()->getScreencap (w, h);
	QImage img = imageFromScreencap (imgdata, w, h);

	String root = basename (getCurrentDocument()->name());

	if (root.right (4) == ".dat")
		root.chop (4);

	String defaultname = (root.length() > 0) ? format ("%1.png", root) : "";
	String fname = QFileDialog::getSaveFileName (g_win, "Save Screencap", defaultname,
				"PNG images (*.png);;JPG images (*.jpg);;BMP images (*.bmp);;All Files (*.*)");

	if (not fname.isEmpty() && not img.save (fname))
		critical (format ("Couldn't open %1 for writing to save screencap: %2", fname, strerror (errno)));

	delete[] imgdata;
}

// =============================================================================
//
extern_cfg (Bool, gl_axes);
DEFINE_ACTION (Axes, 0)
{
	gl_axes = not gl_axes;
	updateActions();
	R()->update();
}

// =============================================================================
//
DEFINE_ACTION (VisibilityToggle, 0)
{
	for (LDObject* obj : selection())
		obj->setHidden (not obj->isHidden());

	refresh();
}

// =============================================================================
//
DEFINE_ACTION (VisibilityHide, 0)
{
	for (LDObject* obj : selection())
		obj->setHidden (true);

	refresh();
}

// =============================================================================
//
DEFINE_ACTION (VisibilityReveal, 0)
{
	for (LDObject* obj : selection())
	obj->setHidden (false);
	refresh();
}

// =============================================================================
//
DEFINE_ACTION (Wireframe, 0)
{
	gl_wireframe = not gl_wireframe;
	R()->refresh();
}

// =============================================================================
//
DEFINE_ACTION (SetOverlay,  0)
{
	OverlayDialog dlg;

	if (not dlg.exec())
		return;

	R()->setupOverlay ((GL::EFixedCamera) dlg.camera(), dlg.fpath(), dlg.ofsx(),
		dlg.ofsy(), dlg.lwidth(), dlg.lheight());
}

// =============================================================================
//
DEFINE_ACTION (ClearOverlay, 0)
{
	R()->clearOverlay();
}

// =============================================================================
//
DEFINE_ACTION (ModeSelect, CTRL (1))
{
	R()->setEditMode (ESelectMode);
}

// =============================================================================
//
DEFINE_ACTION (ModeDraw, CTRL (2))
{
	R()->setEditMode (EDrawMode);
}

// =============================================================================
//
DEFINE_ACTION (ModeCircle, CTRL (3))
{
	R()->setEditMode (ECircleMode);
}

// =============================================================================
//
DEFINE_ACTION (DrawAngles, 0)
{
	gl_drawangles = not gl_drawangles;
	R()->refresh();
}

// =============================================================================
//
DEFINE_ACTION (SetDrawDepth, 0)
{
	if (R()->camera() == GL::EFreeCamera)
		return;

	bool ok;
	double depth = QInputDialog::getDouble (g_win, "Set Draw Depth",
											format ("Depth value for %1 Camera:", R()->getCameraName()),
											R()->getDepthValue(), -10000.0f, 10000.0f, 3, &ok);

	if (ok)
		R()->setDepthValue (depth);
}

#if 0
// This is a test to draw a dummy axle. Meant to be used as a primitive gallery,
// but I can't figure how to generate these pictures properly. Multi-threading
// these is an immense pain.
DEFINE_ACTION (testpic, "Test picture", "", "", (0))
{
	LDDocument* file = getFile ("axle.dat");
	setlocale (LC_ALL, "C");

	if (not file)
	{
		critical ("couldn't load axle.dat");
		return;
	}

	int w, h;

	GLRenderer* rend = new GLRenderer;
	rend->resize (64, 64);
	rend->setAttribute (Qt::WA_DontShowOnScreen);
	rend->show();
	rend->setFile (file);
	rend->setDrawOnly (true);
	rend->compileAllObjects();
	rend->initGLData();
	rend->drawGLScene();

	uchar* imgdata = rend->screencap (w, h);
	QImage img = imageFromScreencap (imgdata, w, h);

	if (img.isNull())
	{
		critical ("Failed to create the image!\n");
	}
	else
	{
		QLabel* label = new QLabel;
		QDialog* dlg = new QDialog;
		label->setPixmap (QPixmap::fromImage (img));
		QVBoxLayout* layout = new QVBoxLayout (dlg);
		layout->addWidget (label);
		dlg->exec();
	}

	delete[] imgdata;
	rend->deleteLater();
}
#endif

// =============================================================================
//
DEFINE_ACTION (ScanPrimitives, 0)
{
	PrimitiveScanner::start();
}

// =============================================================================
//
DEFINE_ACTION (BFCView, SHIFT (B))
{
	gl_colorbfc = not gl_colorbfc;
	updateActions();
	R()->refresh();
}

// =============================================================================
//
DEFINE_ACTION (JumpTo, CTRL (G))
{
	bool ok;
	int defval = 0;
	LDObject* obj;

	if (selection().size() == 1)
		defval = selection()[0]->lineNumber();

	int idx = QInputDialog::getInt (null, "Go to line", "Go to line:", defval,
		1, getCurrentDocument()->getObjectCount(), 1, &ok);

	if (not ok || (obj = getCurrentDocument()->getObject (idx - 1)) == null)
		return;

	getCurrentDocument()->clearSelection();
	obj->select();
	updateSelection();
}

// =============================================================================
//
DEFINE_ACTION (SubfileSelection, 0)
{
	if (selection().size() == 0)
		return;

	String			parentpath = getCurrentDocument()->fullPath();

	// BFC type of the new subfile - it shall inherit the BFC type of the parent document
	LDBFC::Statement		bfctype = LDBFC::NoCertify;

	// Dirname of the new subfile
	String			subdirname = dirname (parentpath);

	// Title of the new subfile
	String			subtitle;

	// Comment containing the title of the parent document
	LDComment*		titleobj = dynamic_cast<LDComment*> (getCurrentDocument()->getObject (0));

	// License text for the subfile
	String			license = getLicenseText (ld_defaultlicense);

	// LDraw code body of the new subfile (i.e. code of the selection)
	QStringList		code;

	// Full path of the subfile to be
	String			fullsubname;

	// Where to insert the subfile reference?
	int				refidx = selection()[0]->lineNumber();

	// Determine title of subfile
	if (titleobj != null)
		subtitle = "~" + titleobj->text();
	else
		subtitle = "~subfile";

	// Remove duplicate tildes
	while (subtitle[0] == '~' && subtitle[1] == '~')
		subtitle.remove (0, 1);

	// If this the parent document isn't already in s/, we need to stuff it into
	// a subdirectory named s/. Ensure it exists!
	String topdirname = basename (dirname (getCurrentDocument()->fullPath()));

	if (topdirname != "s")
	{
		String desiredPath = subdirname + "/s";
		String title = tr ("Create subfile directory?");
		String text = format (tr ("The directory <b>%1</b> is suggested for "
			"subfiles. This directory does not exist, create it?"), desiredPath);

		if (QDir (desiredPath).exists() || confirm (title, text))
		{
			subdirname = desiredPath;
			QDir().mkpath (subdirname);
		}
	}

	// Determine the body of the name of the subfile
	if (not parentpath.isEmpty())
	{
		if (parentpath.endsWith (".dat"))
			parentpath.chop (4);

		// Remove the s?? suffix if it's there, otherwise we'll get filenames
		// like s01s01.dat when subfiling subfiles.
		QRegExp subfilesuffix ("s[0-9][0-9]$");
		if (subfilesuffix.indexIn (parentpath) != -1)
			parentpath.chop (subfilesuffix.matchedLength());

		int subidx = 1;
		String digits;
		QFile f;
		String testfname;

		do
		{
			digits.setNum (subidx++);

			// pad it with a zero
			if (digits.length() == 1)
				digits.prepend ("0");

			fullsubname = subdirname + "/" + basename (parentpath) + "s" + digits + ".dat";
		} while (findDocument ("s\\" + basename (fullsubname)) != null || QFile (fullsubname).exists());
	}

	// Determine the BFC winding type used in the main document - it is to
	// be carried over to the subfile.
	for (LDObject* obj : getCurrentDocument()->objects())
	{
		LDBFC* bfc = dynamic_cast<LDBFC*> (obj);

		if (not bfc)
			continue;

		LDBFC::Statement a = bfc->statement();

		if (a == LDBFC::CertifyCCW || a == LDBFC::CertifyCW || a == LDBFC::NoCertify)
		{
			bfctype = a;
			break;
		}
	}

	// Get the body of the document in LDraw code
	for (LDObject* obj : selection())
		code << obj->asText();

	// Create the new subfile document
	LDDocument* doc = new LDDocument;
	doc->setImplicit (false);
	doc->setFullPath (fullsubname);
	doc->setName (LDDocument::shortenName (fullsubname));
	doc->addObjects (
	{
		new LDComment (subtitle),
		new LDComment ("Name: "),
		new LDComment (format ("Author: %1 [%2]", ld_defaultname, ld_defaultuser)),
		new LDComment (format ("!LDRAW_ORG Unofficial_Subpart")),
		(license != "" ? new LDComment (license) : null),
		new LDEmpty,
		new LDBFC (bfctype),
		new LDEmpty,
	});

	// Add the actual subfile code to the new document
	for (String line : code)
	{
		LDObject* obj = parseLine (line);
		doc->addObject (obj);
	}

	// Try save it
	if (save (doc, true))
	{
		// Remove the selection now
		for (LDObject* obj : selection())
			obj->destroy();

		// Compile all objects in the new subfile
		R()->compiler()->compileDocument (doc);
		g_loadedFiles << doc;

		// Add a reference to the new subfile to where the selection was
		LDSubfile* ref = new LDSubfile();
		ref->setColor (maincolor);
		ref->setFileInfo (doc);
		ref->setPosition (g_origin);
		ref->setTransform (g_identity);
		getCurrentDocument()->insertObj (refidx, ref);

		// Refresh stuff
		updateDocumentList();
		doFullRefresh();
	}
	else
	{
		// Failed to save.
		delete doc;
	}
}

DEFINE_ACTION (RandomColors, CTRL_SHIFT (R))
{
	gl_randomcolors = not gl_randomcolors;
	R()->refresh();
}