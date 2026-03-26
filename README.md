Introduction:

University dormitories play a vital role in student life by providing accommodation and essential daily services. Among these services, the university restaurant
is an integral part of the dormitory, ensuring meal availability for resident students.
The University Dormitory & Restaurant Management System (UDRMS) is
designed to model and manage dormitory operations, including student accommodation, room allocation, and restaurant services such as menu management
and meal access. The system aims to reflect real-world university infrastructure
in a clear and structured way.

This project focuses on applying Object-Oriented Programming (OOP) principles to design a modular, maintainable, and well-structured system. UML
diagrams will be used to document the system design and object relationships,
helping students better understand abstraction and system modeling.



Objectives:

• Model a real-world university dormitory system using Object-Oriented
Programming.
• Apply core OOP concepts such as encapsulation, composition, inheritance,
and basic polymorphism.
• Design a clear relationship between dormitories and restaurants, where
the restaurant is part of the dormitory.
• Organize a C++ project using multiple source and header files.
• Design and implement a graphical user interface for system interaction.
• Utilize appropriate data structures to ensure good performance, especially
for managing students, rooms, and dormitories.
• Test the system to ensure correct functionality, including room allocation,
capacity constraints, and restaurant access.
• Create UML diagrams to represent the system structure and interactions.



Features:

1. Student Management:
• Add and remove students from the system.
• Store student information (ID, full name, academic year).
• Assign students to dormitories and rooms.
• Display student details and accommodation status.

2. Dormitory Management:
• Create and manage multiple dormitories.
• Define dormitory capacity and available rooms.
• Assign and remove students from rooms.
• Display occupied and available rooms.

3. Room Management:
• Manage rooms with room numbers and capacities.
• Track room occupancy.
• Prevent over-occupancy of rooms.

4. Restaurant Management (Part of Dormitory):
• Each dormitory has exactly one restaurant.
• Manage daily menus (breakfast, lunch, dinner).
• Allow dormitory residents to view restaurant menus.
• (Optional) Track the number of meals served per day.

5. Graphical User Interface:
• Develop a graphical user interface for managing dormitory and restaurant operations.
• Use C++ with a graphical framework such as Qt for enhanced interactivity.
• Ensure intuitive navigation using windows, buttons, forms, and dialogs.
• Validate user input and handle incorrect entries.

6. UML Design:
• Create a UML Class Diagram representing the system structure.
• Clearly show relationships such as composition and association.
• (Optional) Provide a UML Sequence Diagram for key operations.

7. File Handling and Data Persistence:
• Save and load data related to students, dormitories, rooms, and
restaurants.
• Ensure data persistence across program executions.

8. Error Handling:
• Handle invalid inputs and unexpected situations gracefully.
• Display clear and meaningful error messages.



System Structure:

• Main Program File: Controls the program flow and user interaction.
• Class Files: Separate classes such as Student, Room, Restaurant, Dormitory, and University.
• Graphical Interface Files: Manage UI components, potentially using
C++ with Qt for enhanced interactivity.
• UML Documentation: UML class and sequence diagrams describing
the system design.
• Data Files: Files used to store persistent data (students, dormitories,
rooms, restaurants).
• Data Structures: Appropriate data structures (arrays, lists, or other
containers) should be used to store and manage students, rooms, and
dormitories efficiently, supporting scalability and ease of management.
