#include "restaurant.h"
#include <algorithm>

// STATIC VARIABLE INITIALIZATION
int Restaurant::BREAKFAST_OPEN  = 7;
int Restaurant::BREAKFAST_CLOSE = 9;
int Restaurant::LUNCH_OPEN      = 12;
int Restaurant::LUNCH_CLOSE     = 14;
int Restaurant::DINNER_OPEN     = 19;
int Restaurant::DINNER_CLOSE    = 21;

// CONSTRUCTOR & DESTRUCTOR
Restaurant::Restaurant() {
    for (int i = 0; i < 7; i++) {
        weeklyMenu[i] = { morningDish::CoffeeMilk_and_Croissant, dish::Spaghetti, dish::Spaghetti };
    }
}

Restaurant::~Restaurant() {}


// GETTERS
Restaurant::dailyMenu Restaurant::getDailyMenu(day d) const {
    return weeklyMenu[static_cast<int>(d)];
}

const Restaurant::dailyMenu* Restaurant::getWeeklyMenu() const {
    return weeklyMenu;
}


// SECURE SETTERS (TIMES) - Validates 24-hour format
void Restaurant::setBR_OPEN(int t)  { if (t >= 0 && t <= 23) BREAKFAST_OPEN = t; }
void Restaurant::setBR_CLOSE(int t) { if (t >= 0 && t <= 24) BREAKFAST_CLOSE = t; }
void Restaurant::setLU_OPEN(int t)  { if (t >= 0 && t <= 23) LUNCH_OPEN = t; }
void Restaurant::setLU_CLOSE(int t) { if (t >= 0 && t <= 24) LUNCH_CLOSE = t; }
void Restaurant::setDI_OPEN(int t)  { if (t >= 0 && t <= 23) DINNER_OPEN = t; }
void Restaurant::setDI_CLOSE(int t) { if (t >= 0 && t <= 24) DINNER_CLOSE = t; }


// SETTERS (MENUS)
void Restaurant::setDailyMenu(day d, morningDish b, dish l, dish din) {
    int index = static_cast<int>(d);
    weeklyMenu[index].breakfast = b;
    weeklyMenu[index].lunch = l;
    weeklyMenu[index].dinner = din;
}

void Restaurant::setWeeklyMenu(dailyMenu sun, dailyMenu mon, dailyMenu tue, 
                               dailyMenu wed, dailyMenu thu, dailyMenu fri, dailyMenu sat) {
    weeklyMenu[static_cast<int>(day::Sunday)]    = sun;
    weeklyMenu[static_cast<int>(day::Monday)]    = mon;
    weeklyMenu[static_cast<int>(day::Tuesday)]   = tue;
    weeklyMenu[static_cast<int>(day::Wednesday)] = wed;
    weeklyMenu[static_cast<int>(day::Thursday)]  = thu;
    weeklyMenu[static_cast<int>(day::Friday)]    = fri;
    weeklyMenu[static_cast<int>(day::Saturday)]  = sat;
}


// MEAL CHECKING
bool Restaurant::didStudentEatBreakfast(int ID) const {
    return std::find(ateBreakfast.begin(), ateBreakfast.end(), ID) != ateBreakfast.end();
}

bool Restaurant::didStudentEatLunch(int ID) const {
    return std::find(ateLunch.begin(), ateLunch.end(), ID) != ateLunch.end();
}

bool Restaurant::didStudentEatDinner(int ID) const {
    return std::find(ateDinner.begin(), ateDinner.end(), ID) != ateDinner.end();
}


// SECURE LOGIC METHODS (Actions)
bool Restaurant::recordBreakfast(int ID, int currentHour) {
    // 1. Is the restaurant actually serving breakfast right now?
    if (currentHour < BREAKFAST_OPEN || currentHour >= BREAKFAST_CLOSE) return false;
    // 2. Did the student already eat?
    if (didStudentEatBreakfast(ID)) return false; 
    
    ateBreakfast.push_back(ID);
    return true;
}

bool Restaurant::recordLunch(int ID, int currentHour) {
    if (currentHour < LUNCH_OPEN || currentHour >= LUNCH_CLOSE) return false;
    if (didStudentEatLunch(ID)) return false; 
    
    ateLunch.push_back(ID);
    return true;
}

bool Restaurant::recordDinner(int ID, int currentHour) {
    if (currentHour < DINNER_OPEN || currentHour >= DINNER_CLOSE) return false;
    if (didStudentEatDinner(ID)) return false; 
    
    ateDinner.push_back(ID);
    return true;
}

void Restaurant::resetDailyMeals() {
    ateBreakfast.clear();
    ateLunch.clear();
    ateDinner.clear();
}

bool Restaurant::isOpen(int hour) const {
    if (hour >= BREAKFAST_OPEN && hour < BREAKFAST_CLOSE) return true;
    if (hour >= LUNCH_OPEN && hour < LUNCH_CLOSE) return true;
    if (hour >= DINNER_OPEN && hour < DINNER_CLOSE) return true;
    return false;
}


// QT UI HELPERS (Enum to String Converters)
std::string Restaurant::getMorningDishName(morningDish d) {
    switch(d) {
        case morningDish::CoffeeMilk_and_Croissant:  return "Coffee Milk & Croissant";
        case morningDish::Yogurt_and_ChocolateBread: return "Yogurt & Chocolate Bread";
        case morningDish::Tea_and_ChocolateBar:      return "Tea & Chocolate Bar";
        default: return "Unknown";
    }
}

std::string Restaurant::getDishName(dish d) {
    switch(d) {
        case dish::Couscous:       return "Couscous";
        case dish::Rechta:         return "Rechta";
        case dish::Spaghetti:      return "Spaghetti";
        case dish::Sardines:       return "Sardines";
        case dish::Chorba:         return "Chorba";
        case dish::Lentil_Soup:    return "Lentil Soup";
        case dish::Chakhchoukha:   return "Chakhchoukha";
        case dish::Loubia:         return "Loubia";
        case dish::Rice:           return "Rice";
        case dish::Tajjine:        return "Tajjine";
        case dish::Tlitli:         return "Tlitli";
        case dish::Mtewem:         return "Mtewem";
        case dish::Jwaz:           return "Jwaz";
        case dish::Fried_Chicken:  return "Fried Chicken";
        default: return "Unknown";
    }
}

std::string Restaurant::getDayName(day d) {
    switch(d) {
        case day::Sunday:    return "Sunday";
        case day::Monday:    return "Monday";
        case day::Tuesday:   return "Tuesday";
        case day::Wednesday: return "Wednesday";
        case day::Thursday:  return "Thursday";
        case day::Friday:    return "Friday";
        case day::Saturday:  return "Saturday";
        default: return "Unknown";
    }
}