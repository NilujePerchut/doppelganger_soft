#!/usr/bin/env python
#######################################################################
#   Title:          Doppelganger uploader gui
#   File:           Doppelganger_uploder_gui.py
#   Descr.:         Bootloader gui client for the Doppelganger
#   Stolen from:    Using primitives from Pinguino
#   Comment:
#######################################################################
# This file is part of Doppelganger:
# http://thewireddoesntexist.org/doppelganger
# Released under the GPLV3 license (see LICENCE file)
#######################################################################

import os
import sys
import time
from PyQt4 import QtGui, QtCore

from uploader8 import getDevice, main
from uploader8 import VENDOR_ID, PRODUCT_ID, ERR_DEVICE_NOT_FOUND

__version__ = "0.1.0"
PROGRAM_NAME = "Doppelganger firmware uploader {0}".format(__version__)

WINDOW_SIZE_W = 500
WINDOW_SIZE_H = 400

POLLING_PERIOD = 2


class DetectionWorker(QtCore.QObject):

    done = QtCore.pyqtSignal()

    @QtCore.pyqtSlot()
    def doDetection(self):
        while(getDevice(VENDOR_ID, PRODUCT_ID) == ERR_DEVICE_NOT_FOUND):
            time.sleep(POLLING_PERIOD)
        self.done.emit()


class FlashWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(FlashWidget, self).__init__()
        self.dfh = QtGui.QLabel(self)
        self.setAcceptDrops(True)
        self.dfh.setText("Drop hex file here")
        self.dfh.setFont(QtGui.QFont("Arial", 22))
        self.dfh.setAlignment(QtCore.Qt.AlignCenter)
        self.load_btn = QtGui.QPushButton("or click here to choose a file",
                                          self)
        self.load_btn.clicked.connect(self.load_btn_callback)
        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(self.dfh)
        vbox.addWidget(self.load_btn)
        self.setLayout(vbox)
        self.show()

    def load_btn_callback(self):
        fname = QtGui.QFileDialog.getOpenFileName(self, "Open firmware file",
                                                  filter="*.hex")
        self.flash_firmware(unicode(fname))

    def dropEvent(self, event):
        # Only one file is accepted
        filename = [unicode(u.toLocalFile())
                    for u in event.mimeData().urls()][0]
        self.flash_firmware(filename)

    def dragEnterEvent(self, event):
        # Must have an url
        if not event.mimeData().hasUrls():
            event.ignore()
            return
        files = [unicode(u.toLocalFile()) for u in event.mimeData().urls()]
        # Must be uniq
        if len(files) > 1:
            event.ignore()
            return
        # Must be an hex file
        if os.path.splitext(files[0])[1] != ".hex":
            event.ignore()
            return
        event.accept()

    def flash_firmware(self, filename):
        print("About to flash the {0} firmware".format(filename))
        if getDevice(VENDOR_ID, PRODUCT_ID) == ERR_DEVICE_NOT_FOUND:
            return
        main('18f45k50', filename)
        # Display a message explaining that all went right
        msg = QtGui.QMessageBox()
        msg.setIcon(QtGui.QMessageBox.Information)

        msg.setText("Firmware successfully written")
        msg.setWindowTitle("Doppelganger upload gui")
        msg.setStandardButtons(QtGui.QMessageBox.Ok)
        msg.exec_()


class DetectWidget(QtGui.QWidget):
    def __init__(self, parent=None, main_window=None):
        super(DetectWidget, self).__init__()
        self.main_window = main_window
        #  Title
        firmupdate = QtGui.QLabel(self)
        firmupdate.setText("Doppelganger detection")
        firmupdate.setFont(QtGui.QFont("Arial", 22))

        #  Picture
        tuto = QtGui.QLabel(self)
        pic = QtGui.QPixmap(os.path.join(os.getcwd(), "rcs",
                                         "bootloader_tuto.png"))
        pic_resized = pic.scaled(500, 500, QtCore.Qt.KeepAspectRatio)
        tuto.setPixmap(pic_resized)

        #  Infos
        infos = QtGui.QLabel()
        infos.setText("Plug the Doppelganger board to the computer USB port" +
                      " while pressing the Up arrow")
        infos.setWordWrap(True)

        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(firmupdate)
        vbox.addStretch(1)
        vbox.addWidget(tuto)
        vbox.addStretch(1)
        vbox.addWidget(infos)
        vbox.addStretch(1)
        self.setLayout(vbox)


class StackedWidget(QtGui.QStackedWidget):

    def __init__(self, parent=None):
        QtGui.QStackedWidget.__init__(self, parent)

    def setCurrentIndex(self, index):
        QtGui.QStackedWidget.setCurrentIndex(self, index)

    def setPageN(self, n):
        print("Setting page {0}".format(n))
        self.setCurrentIndex(n)


class MainWindow(QtGui.QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__()
        self.initUI()
        self.setupThread()

    def initUI(self):

        self.stack = StackedWidget()
        self.stack.addWidget(DetectWidget())
        self.stack.addWidget(FlashWidget())
        self.setCentralWidget(self.stack)

        # Status bar
        self.statusBar = QtGui.QStatusBar()
        self.setStatusBar(self.statusBar)
        self.statusBar.showMessage("Not connected")

        self.setWindowTitle(PROGRAM_NAME)
        self.setFixedSize(WINDOW_SIZE_W, WINDOW_SIZE_H)

        self.show()

    @QtCore.pyqtSlot()
    def detectDone(self):
        self.stack.setPageN(1)
        self.statusBar.showMessage("Connected to Doppelgander")

    def setupThread(self):
        self.thread = QtCore.QThread()
        self.detection_worker = DetectionWorker()

        self.detection_worker.moveToThread(self.thread)

        self.thread.started.connect(self.detection_worker.doDetection)
        self.detection_worker.done.connect(self.detectDone)

        # Start thread
        self.thread.start()


def app_main():
    app = QtGui.QApplication(sys.argv)
    ex = MainWindow()
    sys.exit(app.exec_())


if __name__ == '__main__':
    app_main()
