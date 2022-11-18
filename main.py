import sys
from PySide2.QtWidgets import (
    QApplication,
    QWidget,
    QLabel,
    QLineEdit,
    QPushButton
)
from PySide2 import QtGui, QtCore
import os


def button1_clicked(port):
    os.system(f"fuser -k {port}/tcp")


def window():
    app = QApplication(sys.argv)
    widget = QWidget()
    title = ""
    initial_value = "8000"
    label = QLabel()
    label.setText(title)
    label.setFixedWidth(100)
    lineEdit = QLineEdit(widget)
    lineEdit.setAlignment(QtCore.Qt.AlignCenter)
    lineEdit.setFixedWidth(40)
    lineEdit.setValidator(QtGui.QIntValidator())
    if initial_value != None:
        lineEdit.setText(str(initial_value))
    button = QPushButton(widget, text="kill port")
    button.clicked.connect(lambda: button1_clicked(lineEdit.text()))

    button.setGeometry(QtCore.QRect(50, 50, 70, 30))
    widget.setGeometry(50, 50, 150, 150)
    widget.setWindowTitle("PyQt5 Example")
    widget.show()

    sys.exit(app.exec_())


if __name__ == "__main__":
    window()
