#ifndef RESTAURANT_H
#define RESTAURANT_H
#include <vector>
#include <string>

class Restaurant {

    public:
                        // ENUMS 
        enum class morningDish {
            CoffeeMilk_and_Croissant, Yogurt_and_ChocolateBread, Tea_and_ChocolateBar
        };
        enum class dish {
            Couscous, Rechta, Spaghetti, Sardines, Chorba, Lentil_Soup, Chakhchoukha,
            Loubia, Rice, Tajjine, Tlitli, Mtewem, Jwaz, Fried_Chicken
        };
        enum class day {
            Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
        };
        
        struct dailyMenu {
            morningDish breakfast;
            dish lunch;
            dish dinner;
        };
    
    private:
                        // OPENING AND CLOSING TIMES
        static int BREAKFAST_OPEN;
        static int BREAKFAST_CLOSE;
        static int LUNCH_OPEN;
        static int LUNCH_CLOSE;
        static int DINNER_OPEN;
        static int DINNER_CLOSE;

                        // DAILY AND WEEKLY MENU
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
        dailyMenu getDailyMenu(day d) const;
        const dailyMenu* getWeeklyMenu() const; 

                        // SETTERS
        static void setBR_OPEN(int);
        static void setBR_CLOSE(int);
        static void setLU_OPEN(int);
        static void setLU_CLOSE(int);
        static void setDI_OPEN(int);
        static void setDI_CLOSE(int);
        
        void setDailyMenu(day d, morningDish b, dish l, dish din); 
        void setWeeklyMenu(dailyMenu sun, dailyMenu mon, dailyMenu tue, dailyMenu wed, dailyMenu thu, dailyMenu fri, dailyMenu sat);

                        // MEAL CHECKING
        bool didStudentEatBreakfast(int ID) const;
        bool didStudentEatLunch(int ID) const;
        bool didStudentEatDinner(int ID) const;

                        // LOGIC METHODS
        bool recordBreakfast(int ID); 
        bool recordLunch(int ID);
        bool recordDinner(int ID);
        void resetDailyMeals();
        bool isOpen(int hour) const;     

                        // QT UI HELPERS 
        static std::string getDishName(dish d);
        static std::string getMorningDishName(morningDish d);
        static std::string getDayName(day d);
};

#endif