# UDRMS — Class Architecture

```mermaid
classDiagram
    class University {
        -name: string
        -dormitories: vector~Dormitory~
        -studentPool: vector~Student~
    }

    class Dormitory {
        -number: int
        -blocks: vector~Block~
        -restaurant: Restaurant
    }

    class Block {
        -name: string
        -rooms: vector~vector~Room~~
    }

    class Room {
        -floor: int
        -number: int
        -residents: vector~Student~
        -capacity: static int
    }

    class Student {
        -id: int
        -firstName: string
        -familyName: string
        -email: string
        -phoneNumber: string
        -gender: string
        -academicYear: int
    }

    class Restaurant {
        -weeklyMenu: dailyMenu[7]
        -ateBreakfast: vector~int~
        -ateLunch: vector~int~
        -ateDinner: vector~int~
    }

    class dailyMenu {
        +breakfast: morningDish
        +lunch: dish
        +dinner: dish
    }

    class AppState {
        +university: University
        +role: Role
        +currentStudentId: int
        +activityLog: QVector~ActivityLog~
        +complaints: QVector~Complaint~
        +complaintIdCounter: int
    }

    class Complaint {
        +id: int
        +studentId: int
        +studentName: QString
        +dateTime: QDateTime
        +category: QString
        +description: QString
        +status: QString
        +adminReply: QString
    }

    class ActivityLog {
        +timestamp: QDateTime
        +action: QString
        +description: QString
    }

    AppState    *--  University  : singleton owns
    AppState    o--  Complaint   : manages
    AppState    o--  ActivityLog : tracks
    University  *--  Dormitory   : contains
    University  o--  Student     : pool
    Dormitory   *--  Block       : contains
    Dormitory   *--  Restaurant  : has
    Block       *--  Room        : grid of
    Room        o--  Student     : houses 0..capacity
    Restaurant  *--  dailyMenu   : weeklyMenu[7]
```

### Enums

| Enum | Values |
|------|--------|
| `AppState::Role` | `ADMIN`, `STUDENT` |
| `Restaurant::day` | `Sunday` – `Saturday` |
| `Restaurant::morningDish` | `CoffeeMilk_and_Croissant`, `Yogurt_and_ChocolateBread`, `Tea_and_ChocolateBar` |
| `Restaurant::dish` | `Couscous`, `Rechta`, `Spaghetti`, `Sardines`, `Chorba`, `Lentil_Soup`, `Chakhchoukha`, `Loubia`, `Rice`, `Tajjine`, `Tlitli`, `Mtewem`, `Jwaz`, `Fried_Chicken` |
