import sys
from PyQt5.QtWidgets import QWidget, QProgressBar, QApplication, QPushButton, QVBoxLayout
from PyQt5.QtCore import QThread, pyqtSignal
import time


class ThreadCounting(QThread):
    result_ready = pyqtSignal(int)

    def __init__(self):
        super().__init__()

    def run(self):
        for i in range(1, 101):
            time.sleep(0.05)  # Simulate a time-consuming task in the worker thread
            self.result_ready.emit(i)


class MainWindow(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("QThread Example")
        self.setGeometry(100, 100, 300, 200)

        # Layout
        layout = QVBoxLayout()

        # Progress bar
        self.pbar = QProgressBar(self)
        layout.addWidget(self.pbar)

        # Button
        self.button = QPushButton("Start", self)
        self.button.clicked.connect(self.on_button_click)
        layout.addWidget(self.button)

        self.setLayout(layout)

        # Thread
        self.thread = ThreadCounting()
        self.thread.result_ready.connect(self.update_progress)
        self.thread.finished.connect(self.on_thread_finished)

    def on_button_click(self):
        self.button.setEnabled(False)
        self.thread.start()

    def update_progress(self, value):
        self.pbar.setValue(value)

    def on_thread_finished(self):
        self.button.setEnabled(True)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
