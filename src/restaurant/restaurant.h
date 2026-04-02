#ifndef RESTAURANT_H
#define RESTAURANT_H
#include <vector>

class Restaurant {

    public:
                        // ENUMS
        enum class morningDish
        {
            CoffeeMilk_and_Croissant, Yogurt_and_ChocolateBread, Tea_and_ChocolateBar
        };
        enum class dish
        {
            Couscous, Rechta, Spaghetti, Sardines, Chorba, Lentil_Soup, Chakhchoukha,
            Loubia, Rice, Tajjine, Tlitli, Mtewem, Jwaz, Fried_Chicken
        };
        enum class day
        {
            Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
        };
    
    private:
                        // OPENING AND CLOSING TIMES
        static const int BREAKFAST_OPEN  = 7;
        static const int BREAKFAST_CLOSE = 9;
        static const int LUNCH_OPEN      = 12;
        static const int LUNCH_CLOSE     = 14;
        static const int DINNER_OPEN     = 19;
        static const int DINNER_CLOSE    = 21;

                        // DAILY AND WEEKLY MENU
        struct dailyMenu {
            morningDish breakfast;
            dish lunch;
            dish dinner;
        };
        dailyMenu weeklyMenu[7];

                        // MEAL TRACKING
        std::vector<int> ateBreakfast;
        std::vector<int> ateLunch;
        std::vector<int> ateDinner;

    public:
                        // CONSTRUCTOR AND DESTRUCTOR
        Restaurant();
        ~Restaurant();

                        // GETTERS
        dailyMenu getDailyMenu(day) const;
        dailyMenu getWeeklyMenu() const;

                        // SETTERS
        static void setBR_OPEN(int);
        static void setBR_CLOSE(int);
        static void setLU_OPEN(int);
        static void setLU_CLOSE(int);
        static void setDI_OPEN(int);
        static void setDI_CLOSE(int);
        void setDailyMenu(morningDish, dish, dish);
        void setWeeklyMenu(dailyMenu, dailyMenu, dailyMenu, dailyMenu, dailyMenu, dailyMenu, dailyMenu);

                        // METHODS
        bool didStudentEatBreakfast(int ID) const;
        bool didStudentEatLunch(int ID) const;
        bool didStudentEatDinner(int ID) const;
};

#endif