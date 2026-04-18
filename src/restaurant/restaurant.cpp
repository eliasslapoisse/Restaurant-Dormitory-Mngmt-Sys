#include "restaurant.h"
#include <algorithm>

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

// SETTERS (TIMES)

void Restaurant::setBR_OPEN(int t)  { BREAKFAST_OPEN = t; }
void Restaurant::setBR_CLOSE(int t) { BREAKFAST_CLOSE = t; }
void Restaurant::setLU_OPEN(int t)  { LUNCH_OPEN = t; }
void Restaurant::setLU_CLOSE(int t) { LUNCH_CLOSE = t; }
void Restaurant::setDI_OPEN(int t)  { DINNER_OPEN = t; }
void Restaurant::setDI_CLOSE(int t) { DINNER_CLOSE = t; }

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

// LOGIC METHODS (Actions)

bool Restaurant::recordBreakfast(int ID) {
    if (didStudentEatBreakfast(ID)) return false; // Already ate!
    ateBreakfast.push_back(ID);
    return true;
}

bool Restaurant::recordLunch(int ID) {
    if (didStudentEatLunch(ID)) return false; 
    ateLunch.push_back(ID);
    return true;
}

bool Restaurant::recordDinner(int ID) {
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