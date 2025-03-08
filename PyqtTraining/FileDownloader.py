import sys
import time
from PyQt5.QtCore import QThread, pyqtSignal, QTimer, Qt
from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QProgressBar,
    QPushButton,
    QLabel,
    QVBoxLayout,
    QWidget,
    QMessageBox,
    QGraphicsScene,
    QGraphicsView,
    QGraphicsRectItem,
)
from PyQt5.QtGui import QBrush, QColor


class DownloadThread(QThread):
    update_progress = pyqtSignal(int)  # Signal to update progress
    download_finished = pyqtSignal()  # Signal when download is finished

    def __init__(self):
        super().__init__()
        self._is_paused = False
        self._is_canceled = False

    def run(self):
        for i in range(101):  # Simulate download progress from 0% to 100%
            if self._is_canceled:
                break
            while self._is_paused:  # Pause the thread
                time.sleep(0.1)
            self.update_progress.emit(i)  # Emit progress
            time.sleep(0.1)  # Simulate time delay for download
        if not self._is_canceled:
            self.download_finished.emit()  # Emit finished signal

    def pause(self):
        self._is_paused = True

    def resume(self):
        self._is_paused = False

    def cancel(self):
        self._is_canceled = True


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle("File Downloader")
        self.setFixedSize(600, 400)

        # Progress Bar
        self.progress_bar = QProgressBar(self)
        self.progress_bar.setValue(0)

        # Status Label
        self.status_label = QLabel("Status: Idle", self)

        # Buttons
        self.start_button = QPushButton("Start Download", self)
        self.pause_button = QPushButton("Pause/Resume", self)
        self.pause_button.setEnabled(False)
        self.cancel_button = QPushButton("Cancel", self)
        self.cancel_button.setEnabled(False)

        # Graphics View for Visualization
        self.scene = QGraphicsScene(self)
        self.view = QGraphicsView(self.scene, self)
        self.view.setFixedSize(200, 50)
        self.rect_item = QGraphicsRectItem(0, 0, 0, 50)
        self.rect_item.setBrush(QBrush(QColor(0, 128, 255)))
        self.scene.addItem(self.rect_item)

        # Layout
        layout = QVBoxLayout()
        layout.addWidget(self.progress_bar)
        layout.addWidget(self.status_label)
        layout.addWidget(self.start_button)
        layout.addWidget(self.pause_button)
        layout.addWidget(self.cancel_button)
        layout.addWidget(self.view)

        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

        # Thread
        self.download_thread = DownloadThread()
        self.download_thread.update_progress.connect(self.update_progress)
        self.download_thread.download_finished.connect(self.download_finished)

        # Connect Buttons
        self.start_button.clicked.connect(self.start_download)
        self.pause_button.clicked.connect(self.toggle_pause)
        self.cancel_button.clicked.connect(self.cancel_download)

    def start_download(self):
        self.start_button.setEnabled(False)
        self.pause_button.setEnabled(True)
        self.cancel_button.setEnabled(True)
        self.status_label.setText("Status: Downloading...")
        self.progress_bar.setValue(0)
        self.rect_item.setRect(0, 0, 0, 50)  # Reset graphical visualization
        self.download_thread.start()

    def toggle_pause(self):
        if self.download_thread._is_paused:
            self.download_thread.resume()
            self.status_label.setText("Status: Downloading...")
            self.pause_button.setText("Pause")
        else:
            self.download_thread.pause()
            self.status_label.setText("Status: Paused")
            self.pause_button.setText("Resume")

    def cancel_download(self):
        self.download_thread.cancel()
        self.start_button.setEnabled(True)
        self.pause_button.setEnabled(False)
        self.cancel_button.setEnabled(False)
        self.status_label.setText("Status: Canceled")
        QMessageBox.information(self, "Download Canceled", "The download was canceled.")

    def update_progress(self, value):
        self.progress_bar.setValue(value)
        self.rect_item.setRect(0, 0, value * 2, 50)  # Update graphical visualization

    def download_finished(self):
        self.start_button.setEnabled(True)
        self.pause_button.setEnabled(False)
        self.cancel_button.setEnabled(False)
        self.status_label.setText("Status: Completed")
        QMessageBox.information(self, "Download Complete", "The download completed in 10 seconds.")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())