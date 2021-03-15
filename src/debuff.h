#include <string>

namespace debuff
{

    enum ID : int
    {
        FIRE_VULNERABILITY = 22959,
    };


    class Debuff
    {

    public:
        ID id;
        std::string name;
        double duration;
        int stacks = 1;
        int max_stacks = 1;
        bool hidden = false;

        int addStack()
        {
            if (stacks < max_stacks)
                stacks++;
            else
                return 0;

            return stacks;
        }

    };

    class FireVulnerability : public Debuff
    {

    public:
        FireVulnerability()
        {
            id = FIRE_VULNERABILITY;
            name = "Fire Vulnerability";
            duration = 30;
            max_stacks = 5;
        }

    };

}