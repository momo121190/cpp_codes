import sys
import random
from PyQt5.QtCore import QThread, pyqtSignal, QTimer, QObject
from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QProgressBar,
    QLabel,
    QVBoxLayout,
    QWidget,
    QTabWidget,
    QStatusBar,
    QMessageBox,
)

StyleSheet = '''
QProgressBar#RedProgressBar {
    text-align: center;
}
QProgressBar#RedProgressBar::chunk {
    background-color: #F44336; /* Red */
}
QProgressBar#GreenProgressBar {
    min-height: 12px;
    max-height: 12px;
    border-radius: 6px;
}
QProgressBar#GreenProgressBar::chunk {
    border-radius: 6px;
    background-color: #009688; /* Green */
}
QProgressBar#BlueProgressBar {
    border: 2px solid #2196F3;
    border-radius: 5px;
    background-color: #E0E0E0;
}
QProgressBar#BlueProgressBar::chunk {
    background-color: #2196F3; /* Blue */
    width: 10px; 
    margin: 0.5px;
}
QProgressBar#YellowProgressBar {
    border: 2px solid #FFC107;
    border-radius: 5px;
    background-color: #FFF9C4;
}
QProgressBar#YellowProgressBar::chunk {
    background-color: #FFC107; /* Yellow */
    width: 10px;
    margin: 0.5px;
}
'''


class CarDataWorker(QObject):
    update_engineTemperature = pyqtSignal(int)
    update_FuelLevel = pyqtSignal(int)
    update_speed = pyqtSignal(int)

    def __init__(self):
        super().__init__()
        self.timer = QTimer()
        self.timer.timeout.connect(self.updateValues)

    def start(self):
        """ Start the timer after moving the worker to a separate thread. """
        self.timer.start(500)  # Update every 500ms

    def updateValues(self):
        """ Generate random values and emit them. """
        self.update_engineTemperature.emit(random.randint(0, 150))
        self.update_FuelLevel.emit(random.randint(0, 100))
        self.update_speed.emit(random.randint(1, 200))


class CarDataThread(QThread):
    update_engineTemperature = pyqtSignal(int)
    update_FuelLevel = pyqtSignal(int)
    update_speed = pyqtSignal(int)

    def __init__(self):
        super().__init__()
        self.worker = None  # Worker should be created inside run()

    def run(self):
        """ Create the worker in the new thread and start the timer. """
        self.worker = CarDataWorker()
        
        # Move worker to this thread
        self.worker.moveToThread(self)

        # Connect signals
        self.worker.update_engineTemperature.connect(self.update_engineTemperature.emit)
        self.worker.update_FuelLevel.connect(self.update_FuelLevel.emit)
        self.worker.update_speed.connect(self.update_speed.emit)

        # Start the worker's timer
        self.worker.start()

        # Start event loop to keep the thread alive
        self.exec_()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle("Real-Time Car Diagnostics Tool")
        self.setFixedSize(600, 400)

        # Initialize tab screen
        self.tabs = QTabWidget()
        self.tab1, self.tab2, self.tab3 = QWidget(), QWidget(), QWidget()
        self.tabs.addTab(self.tab1, "Engine")
        self.tabs.addTab(self.tab2, "Fuel")
        self.tabs.addTab(self.tab3, "Speed")

        self.layout = QVBoxLayout()
        self.layout.addWidget(self.tabs)

        # Create Engine Tab
        self.tab1.layout = QVBoxLayout()
        self.label_temp = QLabel("Engine Temperature: 0°C")
        self.tab1.layout.addWidget(self.label_temp)
        self.tab1.progress_bar = QProgressBar()
        self.tab1.progress_bar.setObjectName("GreenProgressBar")  # Set initial object name
        self.tab1.progress_bar.setValue(0)
        self.tab1.progress_bar.setMaximum(150)
        self.tab1.layout.addWidget(self.tab1.progress_bar)
        self.tab1.setLayout(self.tab1.layout)

        # Create Fuel Tab
        self.tab2.layout = QVBoxLayout()
        self.label_fuel = QLabel("Fuel level: 0%")
        self.tab2.layout.addWidget(self.label_fuel)
        self.tab2.progress_bar = QProgressBar()
        self.tab2.progress_bar.setValue(50)
        self.tab2.progress_bar.setMaximum(100)
        self.tab2.layout.addWidget(self.tab2.progress_bar)
        self.tab2.setLayout(self.tab2.layout)

        # Create Speed Tab
        self.tab3.layout = QVBoxLayout()
        self.label_speed = QLabel("Speed: 0 km/h")
        self.tab3.layout.addWidget(self.label_speed)
        self.tab3.progress_bar = QProgressBar()
        self.tab3.progress_bar.setValue(30)
        self.tab3.progress_bar.setMaximum(200)
        self.tab3.layout.addWidget(self.tab3.progress_bar)
        self.tab3.setLayout(self.tab3.layout)

        # Status Bar
        self.statusBar = QStatusBar()
        self.statusBar.showMessage("Running")
        self.layout.addWidget(self.statusBar)

        # Set Layout
        container = QWidget()
        container.setLayout(self.layout)
        self.setCentralWidget(container)

        # Start Background Thread
        self.carDataThread = CarDataThread()
        self.carDataThread.update_engineTemperature.connect(self.update_progressTemp)
        self.carDataThread.update_FuelLevel.connect(self.update_progressFuel)
        self.carDataThread.update_speed.connect(self.update_progressSpeed)
        self.carDataThread.start()


    def update_progressTemp(self, value):
        """ Update Engine Temperature ProgressBar and Status Bar Warning """
        if value > 120:
            self.tab1.progress_bar.setObjectName("RedProgressBar")
            self.statusBar.showMessage(f"⚠️ Warning: Engine Overheating! Temperature = {value}°C", 5000)

        else:
            self.tab1.progress_bar.setObjectName("GreenProgressBar")
            self.statusBar.clearMessage()

        # Apply stylesheet dynamically to reflect changes
        self.tab1.progress_bar.setStyleSheet(StyleSheet)
        self.tab1.progress_bar.setValue(value)
        self.label_temp.setText(f"Engine Temperature: {value}°C")

    def update_progressFuel(self, value):
        """ Update Fuel Level ProgressBar and show warning when low """
        if value < 10:
            self.tab2.progress_bar.setObjectName("YellowProgressBar")
            self.statusBar.showMessage(f"⚠️ Warning: Fuel level is critically low! ({value}%)", 5000)
        else:
            self.tab2.progress_bar.setObjectName("GreenProgressBar")
            self.statusBar.clearMessage()

        # Apply stylesheet dynamically to reflect changes
        self.tab2.progress_bar.setStyleSheet(StyleSheet)
        self.tab2.progress_bar.setValue(value)
        self.label_fuel.setText(f"Fuel Level: {value}%")



    def update_progressSpeed(self, value):
        """ Update Speed ProgressBar """
        self.tab3.progress_bar.setValue(value)
        self.label_speed.setText(f"Speed: {value} km/h")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
