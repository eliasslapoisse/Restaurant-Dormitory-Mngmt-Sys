# 🏛️ UDRMS - University Dormitory & Restaurant Management System

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![Qt](https://img.shields.io/badge/Qt-6.0%2B-41CD52.svg?style=flat&logo=qt)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

**UDRMS** is a comprehensive, high-performance desktop application built in C++ and Qt. Designed originally for university environments (such as ENSIA), it streamlines the management of student housing, dining facilities, maintenance requests, and inventory tracking. 

The application features a custom-built dark/orange UI theme and provides distinct portals for both **Administrators** and **Students**.

---

## ✨ Key Features

### ⚙️ Administrator Portal
* **🎓 Student Management:** Enroll, update, locate, and expel students. View students in the "Unassigned Pool" vs. "Assigned" tables.
* **🏠 Dormitory Allocation:** Auto-assign students to available spaces, or manually place them using an interactive, visual room grid. Swap and move students seamlessly.
* **🍽️ Restaurant & Menu Planner:** Set weekly menus, track daily student meal access, and configure global restaurant operating hours.
* **🔧 Maintenance Center:** Track and resolve student-submitted room issues (Plumbing, Electrical, etc.) with priority levels.
* **📦 Inventory & Announcements:** Monitor stock levels with low-stock warnings, manage billing/fees, and broadcast announcements to students.
* **💾 Data Persistence:** Locally save and load the entire university state via a custom C++ Data Manager.

### 🎓 Student Portal
* **🏠 Room Details:** View current dormitory assignment, room location, and roommate details.
* **🍽️ Daily Menu:** Check the daily breakfast, lunch, and dinner offerings for their specific dormitory.
* **🔧 Issue Reporting:** Submit maintenance requests directly to the administration.

---

## 📸 Screenshots

*(Replace these placeholder links with actual screenshots of your app once uploaded to GitHub)*

| Login Screen | Admin Dashboard |
| :---: | :---: |
| ![Login](docs/login_screenshot.png) | ![Dashboard](docs/dashboard_screenshot.png) |

| Visual Room Grid | Student Portal |
| :---: | :---: |
| ![Dorms](docs/dorms_screenshot.png) | ![Student](docs/student_screenshot.png) |

---

## 🚀 Getting Started

### Prerequisites
* A standard C++17 compiler (GCC, Clang, or MSVC).
* **[Qt Creator](https://www.qt.io/download)** and the Qt Framework (Qt 5 or Qt 6).

### Installation & Build
1. **Clone the repository:**
   ```bash
   git clone [https://github.com/YOUR_USERNAME/UDRMS.git](https://github.com/YOUR_USERNAME/UDRMS.git)
   cd UDRMS

   Open the Project:
Launch Qt Creator and open the udrms.pro project file.

Build the Application:

Run qmake (Right-click the project -> Run qmake).

Click Build (or press Ctrl+B).

Run:
Click the green Play button (Ctrl+R) to launch the application.

🔐 Default Credentials
To test the application immediately after building, use the following login credentials:

Administrator Access:

Username: admin

Password: admin123

Student Access:

Student ID: (Any valid ID generated after enrolling a student)

Password: (The student's registered phone number)

🏗️ Project Architecture
UDRMS strictly separates backend data logic from the frontend Qt GUI:

src/backend/: Pure C++ classes (Student, Room, Dormitory, Restaurant, University) handling all logic, algorithms, and data encapsulation.

src/frontend/: Qt-based GUI components (MainWindow, DashboardWidget, DormitoryWidget, StudentPortalWindow).

src/datamanager.h/cpp: A Singleton class acting as the bridge between the UI and the pure C++ backend, handling state and save/load persistence.

🔮 Roadmap / Future Updates
[ ] Export reports and student lists to PDF.

[ ] Implement QtCharts for visual dashboard analytics.

[ ] Dark Mode / Light Mode theme toggle.

[ ] Database integration (SQLite/PostgreSQL) replacing local file persistence.

🤝 Contributing
Contributions, issues, and feature requests are welcome!
Feel free to check the issues page.

Fork the Project

Create your Feature Branch (git checkout -b feature/AmazingFeature)

Commit your Changes (git commit -m 'Add some AmazingFeature')

Push to the Branch (git push origin feature/AmazingFeature)

Open a Pull Request

📄 License
Distributed under the MIT License. See LICENSE for more information.

Built with ❤️ for Sidi Abdollah National School of Artificial Intelligence (ENSIA)