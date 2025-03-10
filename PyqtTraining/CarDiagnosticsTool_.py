import sys
import time
from PyQt5.QtCore import QThread, pyqtSignal, QTimer
from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QTabWidget,
    QLabel,
    QProgressBar,
    QStatusBar,
    QPushButton,
    QVBoxLayout,
    QWidget,
    QMessageBox,
    QFileDialog,
)
from PyQt5.QtGui import QColor
from datetime import datetime


# Custom QThread for generating car data
class CarDataThread(QThread):
    # Custom signals to send data to the main UI
    engine_temp_signal = pyqtSignal(int)
    fuel_level_signal = pyqtSignal(int)
    speed_signal = pyqtSignal(int)

    def __init__(self):
        super().__init__()
        self._is_running = True

    def run(self):
        while self._is_running:
            # Simulate data generation
            engine_temp = self.generate_random_value(80, 130)  # Engine temperature (80°C to 130°C)
            fuel_level = self.generate_random_value(0, 100)  # Fuel level (0% to 100%)
            speed = self.generate_random_value(0, 200)  # Speed (0 km/h to 200 km/h)

            # Emit signals with the generated data
            self.engine_temp_signal.emit(engine_temp)
            self.fuel_level_signal.emit(fuel_level)
            self.speed_signal.emit(speed)

            # Wait for 500 milliseconds before generating the next data
            time.sleep(0.5)

    def stop(self):
        self._is_running = False

    @staticmethod
    def generate_random_value(min_value, max_value):
        """Generate a random value between min_value and max_value."""
        import random
        return random.randint(min_value, max_value)


# Main Window
class CarDiagnosticsTool(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle("Car Diagnostics Tool")
        self.setFixedSize(800, 600)

        # Create a QTabWidget
        self.tabs = QTabWidget(self)
        self.setCentralWidget(self.tabs)

        # Engine Tab
        self.engine_tab = QWidget()
        self.engine_label = QLabel("Engine Temperature: 0°C", self.engine_tab)
        self.engine_progress = QProgressBar(self.engine_tab)
        self.engine_progress.setRange(0, 150)  # Engine temperature range: 0°C to 150°C
        self.engine_progress.setValue(0)
        engine_layout = QVBoxLayout()
        engine_layout.addWidget(self.engine_label)
        engine_layout.addWidget(self.engine_progress)
        self.engine_tab.setLayout(engine_layout)

        # Fuel Tab
        self.fuel_tab = QWidget()
        self.fuel_label = QLabel("Fuel Level: 0%", self.fuel_tab)
        self.fuel_progress = QProgressBar(self.fuel_tab)
        self.fuel_progress.setRange(0, 100)  # Fuel level range: 0% to 100%
        self.fuel_progress.setValue(0)
        fuel_layout = QVBoxLayout()
        fuel_layout.addWidget(self.fuel_label)
        fuel_layout.addWidget(self.fuel_progress)
        self.fuel_tab.setLayout(fuel_layout)

        # Speed Tab
        self.speed_tab = QWidget()
        self.speed_label = QLabel("Speed: 0 km/h", self.speed_tab)
        self.speed_progress = QProgressBar(self.speed_tab)
        self.speed_progress.setRange(0, 200)  # Speed range: 0 km/h to 200 km/h
        self.speed_progress.setValue(0)
        speed_layout = QVBoxLayout()
        speed_layout.addWidget(self.speed_label)
        speed_layout.addWidget(self.speed_progress)
        self.speed_tab.setLayout(speed_layout)

        # Add tabs to the QTabWidget
        self.tabs.addTab(self.engine_tab, "Engine")
        self.tabs.addTab(self.fuel_tab, "Fuel")
        self.tabs.addTab(self.speed_tab, "Speed")

        # Status Bar
        self.status_bar = QStatusBar(self)
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("Stopped")

        # Buttons
        self.start_button = QPushButton("Start Diagnostics", self)
        self.start_button.clicked.connect(self.start_diagnostics)
        self.stop_button = QPushButton("Stop Diagnostics", self)
        self.stop_button.clicked.connect(self.stop_diagnostics)
        self.stop_button.setEnabled(False)
        self.save_button = QPushButton("Save Data", self)
        self.save_button.clicked.connect(self.save_data)
        self.save_button.setEnabled(False)

        # Layout for buttons
        button_layout = QVBoxLayout()
        button_layout.addWidget(self.start_button)
        button_layout.addWidget(self.stop_button)
        button_layout.addWidget(self.save_button)

        # Add buttons to a separate widget
        button_widget = QWidget()
        button_widget.setLayout(button_layout)
        self.tabs.addTab(button_widget, "Controls")

        # Thread for generating car data
        self.data_thread = CarDataThread()
        self.data_thread.engine_temp_signal.connect(self.update_engine)
        self.data_thread.fuel_level_signal.connect(self.update_fuel)
        self.data_thread.speed_signal.connect(self.update_speed)

    def start_diagnostics(self):
        """Start the diagnostics thread."""
        self.data_thread.start()
        self.start_button.setEnabled(False)
        self.stop_button.setEnabled(True)
        self.save_button.setEnabled(True)
        self.status_bar.showMessage("Running")

    def stop_diagnostics(self):
        """Stop the diagnostics thread."""
        self.data_thread.stop()
        self.data_thread.wait()
        self.start_button.setEnabled(True)
        self.stop_button.setEnabled(False)
        self.status_bar.showMessage("Stopped")

    def update_engine(self, value):
        """Update the engine temperature tab."""
        self.engine_label.setText(f"Engine Temperature: {value}°C")
        self.engine_progress.setValue(value)
        if value > 120:  # Engine temperature warning
            self.engine_progress.setStyleSheet("QProgressBar::chunk { background-color: red; }")
            self.status_bar.showMessage("Warning: Engine temperature too high!")
        else:
            self.engine_progress.setStyleSheet("")  # Reset to default style

    def update_fuel(self, value):
        """Update the fuel level tab."""
        self.fuel_label.setText(f"Fuel Level: {value}%")
        self.fuel_progress.setValue(value)
        if value < 10:  # Fuel level warning
            self.fuel_progress.setStyleSheet("QProgressBar::chunk { background-color: yellow; }")
            self.status_bar.showMessage("Warning: Fuel level low!")
        else:
            self.fuel_progress.setStyleSheet("")  # Reset to default style

    def update_speed(self, value):
        """Update the speed tab."""
        self.speed_label.setText(f"Speed: {value} km/h")
        self.speed_progress.setValue(value)
        if value > 180:  # Speed warning
            self.speed_progress.setStyleSheet("QProgressBar::chunk { background-color: red; }")
            self.status_bar.showMessage("Warning: Speed too high!")
        else:
            self.speed_progress.setStyleSheet("")  # Reset to default style

    def save_data(self):
        """Save the current diagnostics data to a file."""
        file_name, _ = QFileDialog.getSaveFileName(self, "Save Data", "", "Text Files (*.txt)")
        if file_name:
            with open(file_name, "w") as file:
                file.write(f"Timestamp: {datetime.now()}\n")
                file.write(f"Engine Temperature: {self.engine_progress.value()}°C\n")
                file.write(f"Fuel Level: {self.fuel_progress.value()}%\n")
                file.write(f"Speed: {self.speed_progress.value()} km/h\n")
            QMessageBox.information(self, "Data Saved", "Diagnostics data saved successfully.")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = CarDiagnosticsTool()
    window.show()
    sys.exit(app.exec_())
