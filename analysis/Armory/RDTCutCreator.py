#!/usr/bin/python3
"""
RDTCutCreator.py : draw one graphic cut per RDT telescope (4) on dE vs E, save as TObjArray "cutList"
                   (same file format as Armory/RDTCutCreator.C : cut<i>, VarX rdt[2i], VarY rdt[2i+1])

usage (from analysis/working) :
   python3 ../Armory/RDTCutCreator.py ../root_data/gen_run008.root [more files / glob ...]
           [-o rdtCuts.root] [--emin 0] [--emax 6000] [--demin 0] [--demax 2000] [--logz] [--no-grid] [--trace]
           [--gate "rdt[0] > 100"] [--tree gen_tree]
   (--erange / --derange = --emax / --demax)
   from the ROOT prompt of MonitorsBZero :  RDTCutCreator()   (runs this script on the processed runs)

window : PyQt6 (matplotlib plot inside), plain matplotlib window if PyQt6 is not available

for each telescope (telescope 0, 1, 2, 3) :
   left click          : add a vertex
   double-click        : close the cut (ONLY closes it)
   drag a vertex       : move it (closed cut), to adjust the cut before accepting
   right-click vertex  : delete it (closed cut, >= 3 vertices kept). also Backspace / Delete with the mouse over the vertex
   Backspace / Delete  : remove the last vertex (while drawing)            [Undo button]
   C                   : clear, draw again                                 [Clear button]
   E                   : load the cut of this telescope from the output file (dashed gray) [Load old button]
   X                   : accept the closed cut, go to the next telescope   [Accept button]
   close the window    : abort, nothing saved, existing file untouched
after the 4th accept : the cuts are saved, the window closes

controls (PyQt6) : E min / E max / dE min / dE max (Enter to apply), logz, grid, save to (Browse...)
   keys typed in a box go to the box, the shortcuts (X, C, E, Backspace) work again after Enter
zoom / pan of the toolbar : the clicks go to the toolbar while it is active, press the button again to leave it
"""

import argparse
import glob
import os
import sys

import numpy as np

import matplotlib
try:
   from PyQt6 import QtCore, QtGui, QtWidgets
   matplotlib.use('QtAgg')
   HAS_QT = True
except ImportError:
   HAS_QT = False
#---- matplotlib default keys that clash with the cut keys (plain matplotlib window only)
matplotlib.rcParams['keymap.back']    = ['left', 'MouseButton.BACK']   # drop 'c', 'backspace'
matplotlib.rcParams['keymap.forward'] = ['right', 'MouseButton.FORWARD']
matplotlib.rcParams['keymap.quit']    = ['ctrl+w', 'cmd+w']             # drop 'q' (closing = abort)
matplotlib.rcParams['keymap.save']    = ['ctrl+s']                      # drop 's'
matplotlib.rcParams['keymap.yscale']  = []                              # drop 'l'
matplotlib.rcParams['keymap.xscale']  = []                              # drop 'k', 'L'
from matplotlib.figure import Figure
from matplotlib.colors import LogNorm

NTEL = 4
NBIN = 500
PICK_PIXEL = 10                                   # vertex pick radius [pixel]
LINE_COLOR = ['black', 'red', 'green', 'blue']    # ROOT colors 1, 2, 8, 4 of RDTCutCreator.C
ROOT_COLOR = [1, 2, 8, 4]
HELP = "click: vertex | double-click: close | drag vertex: move | right-click vertex: delete | Backspace: undo | C: clear | E: load old | X: accept"


#################################################################### ROOT I/O
def import_root():
   import ROOT
   ROOT.gROOT.SetBatch(True)
   ROOT.gErrorIgnoreLevel = ROOT.kWarning
   return ROOT


def load_data(ROOT, files, treeName, branch, gate):
   """ returns (N, 8) array of the RDT branch, NaN = no hit """
   vec = ROOT.std.vector('string')()
   for f in files: vec.push_back(f)
   df = ROOT.RDataFrame(treeName, vec)
   if gate: df = df.Filter(gate)
   cols = []
   for i in range(8):
      name = f"_rdt{i}"
      df = df.Define(name, f"(double) {branch}[{i}]")
      cols.append(name)
   arr = df.AsNumpy(cols)
   return np.column_stack([arr[c] for c in cols])


def load_existing_cuts(ROOT, fileName):
   """ {i : (x array, y array)} from an existing cut file """
   cuts = {}
   if not fileName or not os.path.isfile(fileName): return cuts
   f = ROOT.TFile.Open(fileName)
   if not f or f.IsZombie(): return cuts
   cutList = f.Get("cutList")
   if cutList:
      for i in range(cutList.GetEntries()):
         c = cutList.At(i)
         n = c.GetN()
         x = np.array([c.GetX()[k] for k in range(n)])
         y = np.array([c.GetY()[k] for k in range(n)])
         cuts[i] = (x, y)
   f.Close()
   return cuts


def save_cuts(ROOT, fileName, polys, branch):
   keep = []
   cutList = ROOT.TObjArray()
   for i, verts in enumerate(polys):
      pts = list(verts) + [verts[0]]                 # closed polygon
      cut = ROOT.TCutG(f"cut{i}", len(pts))
      for k, (x, y) in enumerate(pts): cut.SetPoint(k, x, y)
      cut.SetVarX(f"{branch}[{2*i}]")
      cut.SetVarY(f"{branch}[{2*i+1}]")
      cut.SetTitle("")
      cut.SetLineColor(ROOT_COLOR[i])
      cutList.Add(cut)
      keep.append(cut)
   f = ROOT.TFile(fileName, "recreate")
   cutList.Write("cutList", ROOT.TObject.kSingleKey)
   f.Close()


#################################################################### cut drawing on a matplotlib canvas
class CutCanvas:
   """ histogram + cut drawing / editing. only the cut is redrawn on mouse move (blitting).
       on_status(text) : called when the state changes, on_done() : after the 4th accept """

   def __init__(self, ROOT, data, args, fig, canvas, on_status=None, on_done=None):
      self.ROOT = ROOT
      self.data = data
      self.branch = args.branch
      self.range = [args.emin, args.emax, args.demin, args.demax]
      self.logz = args.logz
      self.grid = getattr(args, 'grid', True)
      self.output = args.output
      self.existing = load_existing_cuts(ROOT, self.output)
      if self.existing: print(f" existing {self.output} : {len(self.existing)} cuts (dashed gray, E to edit)")
      self.on_status = on_status or (lambda text: None)
      self.on_done = on_done or (lambda: None)
      self.tel = 0
      self.polys = []          # accepted cuts, list of [(x, y), ...]
      self.verts = []          # current cut
      self.closed = False
      self.drag = None         # index of the dragged vertex
      self.mouse = None        # last mouse position in the plot [pixel], for Backspace over a vertex
      self.finished = False
      self.bg = None           # background for blitting
      self.toolbar = None

      self.fig = fig
      self.canvas = canvas
      self.ax = fig.add_subplot(111)
      fig.subplots_adjust(left=0.12, right=0.96, bottom=0.10, top=0.94)
      canvas.mpl_connect('button_press_event',   self.on_press)
      canvas.mpl_connect('button_release_event', self.on_release)
      canvas.mpl_connect('motion_notify_event',  self.on_motion)
      canvas.mpl_connect('draw_event',           self.on_draw)
      self.start_telescope()

   #---------------------------------------------------------------- state
   def state_text(self):
      if self.finished:    return "all cuts accepted"
      if not self.verts:   return "click to start the cut"
      if self.closed:      return f"closed ({len(self.verts)} vertices) : drag a vertex to move, right-click to delete, X to accept"
      return f"drawing ({len(self.verts)} vertices) : double-click to close"

   def status(self, msg=None):
      text = msg if msg else self.state_text()
      if msg: print(f"   {msg}")
      self.on_status(text)

   #---------------------------------------------------------------- drawing
   def start_telescope(self):
      self.verts = []
      self.closed = False
      self.drag = None
      self.draw_hist()
      b, i = self.branch, self.tel
      print(f"======== {i}-th cut ({b}[{2*i+1}] vs {b}[{2*i}]) : click vertices, double-click to close, drag to edit, X to accept")
      self.status()

   def draw_hist(self):
      """ full redraw : histogram + reference cut of the current telescope, the current cut is kept """
      i = self.tel
      ax = self.ax
      ax.clear()
      b = self.branch
      e0, e1, d0, d1 = self.range
      e  = self.data[:, 2*i]
      de = self.data[:, 2*i+1]
      ok = np.isfinite(e) & np.isfinite(de)
      e, de = e[ok], de[ok]
      h, xe, ye = np.histogram2d(e, de, bins=NBIN, range=[[e0, e1], [d0, d1]])
      nIn = int(h.sum())
      if nIn < 1000:
         inR = (e >= e0) & (e <= e1) & (de >= d0) & (de <= d1)
         ax.plot(e[inR], de[inR], '.', ms=2, color='k')
      else:
         hm = np.ma.masked_where(h.T == 0, h.T)
         norm = LogNorm(vmin=1, vmax=max(2, hm.max())) if self.logz else None
         ax.imshow(hm, origin='lower', extent=(e0, e1, d0, d1), aspect='auto', cmap='viridis', norm=norm, interpolation='nearest')
      ax.set_xlim(e0, e1)
      ax.set_ylim(d0, d1)
      ax.set_xlabel(f"E  {b}[{2*i}]")
      ax.set_ylabel(f"dE  {b}[{2*i+1}]")
      ax.set_title(f"RDT telescope {i} : {b}[{2*i+1}] vs {b}[{2*i}]", fontsize=11)
      if self.grid:
         ax.grid(True, color='gray', alpha=0.4, lw=0.5, ls='--')
         ax.set_axisbelow(False)
      ax.text(0.98, 0.98, f"entries in range {nIn}", transform=ax.transAxes, ha='right', va='top', fontsize=9)

      if i in self.existing:
         x, y = self.existing[i]
         ax.plot(x, y, '--', color='gray', lw=1, label=f'cut-{i} in {os.path.basename(self.output)} (E : edit)')
         ax.legend(loc='upper left', fontsize=8)

      color = LINE_COLOR[i]
      #---- animated : not part of the full draw, drawn by blitting
      self.line,   = ax.plot([], [], '-o', color=color, lw=2, ms=6, mfc='white', animated=True)
      self.rubber, = ax.plot([], [], ':',  color=color, lw=1, animated=True)
      self.set_cut_data()
      self.bg = None
      self.canvas.draw_idle()

   def on_draw(self, event):
      """ after a full draw : keep the background, draw the cut on top """
      self.bg = self.canvas.copy_from_bbox(self.ax.bbox)
      self.ax.draw_artist(self.line)
      self.ax.draw_artist(self.rubber)

   def set_cut_data(self):
      pts = list(self.verts)
      if self.closed and pts: pts = pts + [pts[0]]
      self.line.set_data([p[0] for p in pts], [p[1] for p in pts])
      if self.closed or not self.verts: self.rubber.set_data([], [])

   def blit_cut(self):
      """ fast redraw of the cut only """
      self.set_cut_data()
      if self.bg is None:
         self.canvas.draw_idle()
         return
      self.canvas.restore_region(self.bg)
      self.ax.draw_artist(self.line)
      self.ax.draw_artist(self.rubber)
      self.canvas.blit(self.ax.bbox)

   #---------------------------------------------------------------- actions (keys / buttons)
   def accept(self):
      if self.finished: return
      if not self.closed:
         self.status("no closed cut yet : double-click to close the cut first")
         return
      self.polys.append(list(self.verts))
      print(f"   cut-{self.tel} accepted ({len(self.verts)} vertices)")
      self.tel += 1
      if self.tel < NTEL:
         self.start_telescope()
      else:
         self.finished = True
         self.status()
         self.on_done()

   def undo(self):
      """ drawing : remove the last vertex. closed : delete the vertex under the mouse """
      if self.finished or not self.verts: return
      if self.closed:
         k = self.pick_at(*self.mouse) if self.mouse else None
         if k is None:
            self.status("closed cut : right-click a vertex (or Backspace with the mouse over it) to delete it")
         else:
            self.remove_vertex(k)
         return
      self.verts.pop()
      self.blit_cut()
      self.status()

   def remove_vertex(self, k):
      if len(self.verts) <= 3:
         self.status("a closed cut needs at least 3 vertices (C to clear and redraw)")
         return
      self.verts.pop(k)
      print(f"   vertex {k} deleted ({len(self.verts)} vertices)")
      self.blit_cut()
      self.status()

   def clear(self):
      if self.finished: return
      self.verts = []
      self.closed = False
      self.blit_cut()
      self.status()

   def load_old(self):
      if self.finished: return
      if self.tel not in self.existing:
         self.status(f"no cut-{self.tel} in {self.output}")
         return
      x, y = self.existing[self.tel]
      pts = list(zip(x, y))
      pts = [q for k, q in enumerate(pts) if k == 0 or q != pts[k-1]]   # drop repeated points (ROOT editor double-click)
      if len(pts) > 1 and pts[0] == pts[-1]: pts = pts[:-1]             # stored closed
      self.verts = [(float(a), float(b)) for a, b in pts]
      self.closed = len(self.verts) >= 3
      print(f"   loaded cut-{self.tel} of {self.output} ({len(self.verts)} vertices) : drag to edit, X to accept")
      self.blit_cut()
      self.status()

   def set_range(self, rng):
      """ returns False if not valid (min >= max) """
      if rng[0] >= rng[1] or rng[2] >= rng[3]:
         self.status("range needs min < max")
         return False
      if list(rng) != self.range:
         self.range = list(rng)
         print(f"   range : E {rng[0]:g} - {rng[1]:g}, dE {rng[2]:g} - {rng[3]:g}")
         self.draw_hist()
      return True

   def set_logz(self, on):
      if bool(on) == self.logz: return
      self.logz = bool(on)
      self.draw_hist()

   def set_grid(self, on):
      if bool(on) == self.grid: return
      self.grid = bool(on)
      self.draw_hist()

   def set_output(self, fileName):
      fileName = fileName.strip()
      if not fileName or fileName == self.output: return
      self.output = fileName
      self.existing = load_existing_cuts(self.ROOT, self.output)
      print(f"   save to {self.output}" + (f" (exists, {len(self.existing)} cuts, will be overwritten)" if os.path.isfile(self.output) else " (new file)"))
      self.draw_hist()
      self.status()

   #---------------------------------------------------------------- mouse
   def toolbar_active(self):
      tb = self.toolbar or getattr(self.canvas, 'toolbar', None)
      return tb is not None and getattr(tb, 'mode', '') != ''

   def pick_at(self, px, py):
      """ index of the vertex within PICK_PIXEL of (px, py) [pixel], or None """
      if not self.verts: return None
      pix = self.ax.transData.transform(np.array(self.verts))
      d = np.hypot(pix[:, 0] - px, pix[:, 1] - py)
      k = int(np.argmin(d))
      return k if d[k] < PICK_PIXEL else None

   def pick_vertex(self, event):
      return self.pick_at(event.x, event.y)

   def on_press(self, event):
      if self.finished or event.inaxes != self.ax or self.toolbar_active(): return
      if event.xdata is None: return

      if self.closed and event.button == 3:               # edit mode : right-click deletes a vertex
         k = self.pick_vertex(event)
         if k is not None: self.remove_vertex(k)
         return
      if event.button != 1: return

      if self.closed:                                     # edit mode : drag
         self.drag = self.pick_vertex(event)
         return

      if event.dblclick:                                  # close (the 1st click of the double-click already added a vertex)
         if len(self.verts) >= 3:
            self.closed = True
            print(f"   cut closed ({len(self.verts)} vertices) : drag to edit, X to accept")
            self.blit_cut()
            self.status()
         else:
            self.status(f"need >= 3 vertices to close (have {len(self.verts)})")
         return

      self.verts.append((event.xdata, event.ydata))
      self.blit_cut()
      self.status()

   def on_release(self, event):
      self.drag = None

   def on_motion(self, event):
      if self.finished or event.inaxes != self.ax or event.xdata is None:
         self.mouse = None
         return
      self.mouse = (event.x, event.y)
      if self.drag is not None:
         self.verts[self.drag] = (event.xdata, event.ydata)
         self.blit_cut()
      elif self.verts and not self.closed:
         x0, y0 = self.verts[-1]
         self.rubber.set_data([x0, event.xdata], [y0, event.ydata])
         if self.bg is not None:
            self.canvas.restore_region(self.bg)
            self.ax.draw_artist(self.line)
            self.ax.draw_artist(self.rubber)
            self.canvas.blit(self.ax.bbox)


#################################################################### PyQt6 window
if HAS_QT:
   from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg, NavigationToolbar2QT

   class CutWindow(QtWidgets.QMainWindow):

      def __init__(self, ROOT, data, args, files):
         super().__init__()
         self.aborted = True
         self.setWindowTitle("RDT Cut Creator  |  " + ", ".join(os.path.basename(f) for f in files))
         self.resize(900, 1000)

         self.fig = Figure(figsize=(8, 8))
         self.canvas = FigureCanvasQTAgg(self.fig)
         self.canvas.setFocusPolicy(QtCore.Qt.FocusPolicy.StrongFocus)
         self.toolbar = NavigationToolbar2QT(self.canvas, self)

         #---- status bar : progress (left), message
         self.progress = QtWidgets.QLabel()
         self.progress.setStyleSheet("font-weight: bold; padding: 0 8px;")
         self.statusBar().addWidget(self.progress)
         self.message = QtWidgets.QLabel()
         self.statusBar().addWidget(self.message, 1)

         self.cc = CutCanvas(ROOT, data, args, self.fig, self.canvas, on_status=self.show_status, on_done=self.done)
         self.cc.toolbar = self.toolbar

         #---- range, logz
         self.spins = []
         rangeRow = QtWidgets.QHBoxLayout()
         for k, name in enumerate(["E min", "E max", "dE min", "dE max"]):
            sp = QtWidgets.QDoubleSpinBox()
            sp.setRange(-1e7, 1e7)
            sp.setDecimals(0)
            sp.setSingleStep(100)
            sp.setKeyboardTracking(False)          # valueChanged on Enter / focus out / arrows only
            sp.setValue(self.cc.range[k])
            sp.setMinimumWidth(90)
            sp.valueChanged.connect(self.range_changed)
            sp.editingFinished.connect(self.canvas.setFocus)
            rangeRow.addWidget(QtWidgets.QLabel(name))
            rangeRow.addWidget(sp)
            rangeRow.addSpacing(8)
            self.spins.append(sp)
         self.logzBox = QtWidgets.QCheckBox("logz")
         self.logzBox.setChecked(self.cc.logz)
         self.logzBox.toggled.connect(self.logz_changed)
         rangeRow.addWidget(self.logzBox)
         self.gridBox = QtWidgets.QCheckBox("grid")
         self.gridBox.setChecked(self.cc.grid)
         self.gridBox.toggled.connect(self.grid_changed)
         rangeRow.addWidget(self.gridBox)
         rangeRow.addStretch(1)

         #---- output file
         outRow = QtWidgets.QHBoxLayout()
         outRow.addWidget(QtWidgets.QLabel("Save to"))
         self.outEdit = QtWidgets.QLineEdit(self.cc.output)
         self.outEdit.editingFinished.connect(self.output_changed)
         outRow.addWidget(self.outEdit, 1)
         browse = QtWidgets.QPushButton("Browse...")
         browse.clicked.connect(self.browse)
         outRow.addWidget(browse)

         #---- actions
         actRow = QtWidgets.QHBoxLayout()
         for text, tip, func in [("Undo  (Backspace)", "remove the last vertex while drawing\n(closed cut : right-click a vertex, or Backspace with the mouse over it, to delete it)", self.cc.undo),
                                 ("Clear  (C)",        "clear the cut, draw again",           self.cc.clear),
                                 ("Load old  (E)",     "load this telescope's cut from the output file to edit it", self.cc.load_old)]:
            b = QtWidgets.QPushButton(text)
            b.setToolTip(tip)
            b.setFocusPolicy(QtCore.Qt.FocusPolicy.NoFocus)   # keep the keys for the shortcuts
            b.clicked.connect(func)
            actRow.addWidget(b)
         actRow.addStretch(1)
         self.acceptBtn = QtWidgets.QPushButton("Accept  (X)")
         self.acceptBtn.setToolTip("accept the closed cut, go to the next telescope (saved after the 4th)")
         self.acceptBtn.setStyleSheet("font-weight: bold; padding: 4px 16px;")
         self.acceptBtn.setFocusPolicy(QtCore.Qt.FocusPolicy.NoFocus)
         self.acceptBtn.clicked.connect(self.cc.accept)
         actRow.addWidget(self.acceptBtn)

         helpLabel = QtWidgets.QLabel(HELP)
         helpLabel.setStyleSheet("color: gray;")

         lay = QtWidgets.QVBoxLayout()
         lay.addWidget(self.toolbar)
         lay.addWidget(self.canvas, 1)
         lay.addWidget(helpLabel)
         lay.addLayout(rangeRow)
         lay.addLayout(outRow)
         lay.addLayout(actRow)
         central = QtWidgets.QWidget()
         central.setLayout(lay)
         self.setCentralWidget(central)

         #---- shortcuts : not triggered while typing in a box (the box takes the key)
         for seq, func in [("X", self.cc.accept), ("Shift+X", self.cc.accept),
                           ("C", self.cc.clear), ("Shift+C", self.cc.clear),
                           ("E", self.cc.load_old), ("Shift+E", self.cc.load_old),
                           ("Backspace", self.cc.undo), ("Delete", self.cc.undo)]:
            sc = QtGui.QShortcut(QtGui.QKeySequence(seq), self)
            sc.activated.connect(func)

         self.show_status(self.cc.state_text())
         self.canvas.setFocus()

      def show_status(self, text):
         cc = getattr(self, 'cc', None)
         if cc is None: return          # called while CutCanvas is being built
         self.progress.setText(f"telescope {min(cc.tel, NTEL-1)}  |  accepted {len(cc.polys)} / {NTEL}")
         over = "  (will overwrite)" if os.path.isfile(cc.output) else "  (new file)"
         self.message.setText(f"{text}     |  save to {cc.output}{over}")

      def range_changed(self, *a):
         rng = [sp.value() for sp in self.spins]
         if not self.cc.set_range(rng):
            for sp, v in zip(self.spins, self.cc.range):          # revert
               sp.blockSignals(True); sp.setValue(v); sp.blockSignals(False)

      def logz_changed(self, on):
         self.cc.set_logz(on)
         self.canvas.setFocus()

      def grid_changed(self, on):
         self.cc.set_grid(on)
         self.canvas.setFocus()

      def output_changed(self):
         self.cc.set_output(self.outEdit.text())
         self.outEdit.setText(self.cc.output)
         self.canvas.setFocus()

      def browse(self):
         name, _ = QtWidgets.QFileDialog.getSaveFileName(self, "Save the RDT cuts to", self.cc.output, "ROOT file (*.root)")
         if name:
            if not name.endswith(".root"): name += ".root"
            self.outEdit.setText(name)
            self.output_changed()

      def done(self):
         self.aborted = False
         self.close()

      def closeEvent(self, event):
         cc = self.cc
         if self.aborted and (cc.polys or cc.verts):
            r = QtWidgets.QMessageBox.question(self, "RDT Cut Creator",
                    f"Close without saving?\n{len(cc.polys)} accepted cut(s) will be discarded, {cc.output} not changed.")
            if r != QtWidgets.QMessageBox.StandardButton.Yes:
               event.ignore()
               return
         event.accept()


#################################################################### plain matplotlib window (no PyQt6)
def run_matplotlib(ROOT, data, args):
   import matplotlib.pyplot as plt
   fig = plt.figure(figsize=(8, 8.5))
   state = {'aborted': True}
   def on_status(text):
      cc = getattr(fig, '_cc', None)
      head = f"telescope {min(cc.tel, NTEL-1)} | accepted {len(cc.polys)} / {NTEL}" if cc else ""
      fig.suptitle(f"{head}\n{text}", fontsize=10)
   def on_done():
      state['aborted'] = False
      plt.close(fig)
   cc = CutCanvas(ROOT, data, args, fig, fig.canvas, on_status=on_status, on_done=on_done)
   fig._cc = cc
   fig.subplots_adjust(top=0.88)
   fig.text(0.01, 0.005, HELP, fontsize=7, color='dimgray')
   keys = {'x': cc.accept, 'X': cc.accept, 'c': cc.clear, 'C': cc.clear, 'e': cc.load_old, 'E': cc.load_old,
           'backspace': cc.undo, 'delete': cc.undo}
   def on_key(event):
      f = keys.get(event.key)
      if f:
         f()
         fig.canvas.draw_idle()     # suptitle is outside the blit area
   fig.canvas.mpl_connect('key_press_event', on_key)
   on_status(cc.state_text())
   plt.show()
   return cc, state['aborted']


#################################################################### main
def main():
   p = argparse.ArgumentParser(description="graphic cuts for the 4 RDT telescopes (dE vs E)")
   p.add_argument("files", nargs='+', help="root files (glob allowed)")
   p.add_argument("-o", "--output", default="rdtCuts.root", help="output cut file [rdtCuts.root], editable in the window")
   p.add_argument("--emin",  type=float, default=0,    help="E axis min [0]")
   p.add_argument("--emax",  "--erange",  type=float, default=6000, help="E axis max [6000]")
   p.add_argument("--demin", type=float, default=0,    help="dE axis min [0]")
   p.add_argument("--demax", "--derange", type=float, default=2000, help="dE axis max [2000]")
   p.add_argument("--logz", action="store_true", help="log color scale")
   p.add_argument("--no-grid", dest="grid", action="store_false", help="grid lines off (default on)")
   p.add_argument("--trace", action="store_true", help="use trdt instead of rdt")
   p.add_argument("--gate", default="", help="RDataFrame filter, e.g. \"rdt[0] > 100\"")
   p.add_argument("--tree", default="gen_tree", help="tree name [gen_tree]")
   p.add_argument("--report", default="", help=argparse.SUPPRESS)   # file to receive the saved output path (MonitorsBZero RDTCutCreator)
   args = p.parse_args()
   args.branch = "trdt" if args.trace else "rdt"
   if args.emin >= args.emax or args.demin >= args.demax:
      print("range needs min < max"); sys.exit(1)

   files = []
   for f in args.files:
      g = sorted(glob.glob(f))
      files += g if g else [f]
   missing = [f for f in files if not os.path.isfile(f)]
   if missing:
      print(f"file(s) not found : {missing}"); sys.exit(1)

   print("================ Graphic Cut Creator for RDT (python) ==============")
   for f in files: print(f"   {f}")

   ROOT = import_root()
   data = load_data(ROOT, files, args.tree, args.branch, args.gate)
   print(f" {len(data)} entries{' after gate ' + args.gate if args.gate else ''}")

   if HAS_QT:
      app = QtWidgets.QApplication.instance() or QtWidgets.QApplication(sys.argv[:1])
      win = CutWindow(ROOT, data, args, files)
      win.show()
      win.raise_()
      win.activateWindow()
      app.exec()
      cc, aborted = win.cc, win.aborted
   else:
      print(" PyQt6 not found, using the plain matplotlib window (pip3 install PyQt6 for the full window)")
      cc, aborted = run_matplotlib(ROOT, data, args)

   if aborted:
      print(f"====> window closed, stopped by user. {len(cc.polys)} cut(s) discarded, {cc.output} not written or changed.")
      sys.exit(0)

   save_cuts(ROOT, cc.output, cc.polys, args.branch)
   print(f"====> saved {len(cc.polys)} cuts into {cc.output}")
   if args.report:
      with open(args.report, "w") as fr: fr.write(os.path.abspath(cc.output) + "\n")


if __name__ == "__main__":
   main()
