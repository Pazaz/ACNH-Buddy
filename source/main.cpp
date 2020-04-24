#define TESLA_INIT_IMPL
#include <tesla.hpp> // libtesla

#include <dmnt/dmntcht.h> // libstratosphere

#include <sstream>
#include <iomanip>

static std::map<u32, std::string> slots;
static std::map<u32, std::string> items;

// https://thispointer.com/how-to-search-by-value-in-a-map-c/
template <typename K, typename V>
bool findByValue(std::vector<K> &vec, std::map<K, V> mapOfElemen, V value)
{
    bool bResult = false;
    auto it = mapOfElemen.begin();
    while (it != mapOfElemen.end())
    {
        if (it->second == value)
        {
            bResult = true;
            vec.push_back(it->first);
        }
        it++;
    }
    return bResult;
}

// https://stackoverflow.com/a/5100745
template <typename T>
std::string int_to_hex(T i)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill('0') << std::setw(sizeof(T) * 2)
           << std::hex << i;
    return stream.str();
}

class GuiSlots : public tsl::Gui
{
    u32 *_slot;

public:
    GuiSlots(u32 *slot) : _slot(slot) {}

    virtual tsl::elm::Element *createUI() override
    {
        auto *rootFrame = new tsl::elm::OverlayFrame("ACNH-Buddy", "v1.0.0 - Item Selector");
        auto list = new tsl::elm::List();

        for (auto slot : slots)
        {
            auto *slotListItem = new tsl::elm::ListItem(slot.second);
            slotListItem->setClickListener([this, slotListItem](u64 keys) {
                if (keys & KEY_A || keys & KEY_RIGHT)
                {
                    std::vector<u32> vec;
                    findByValue(vec, slots, slotListItem->getText());
                    *this->_slot = vec[0];
                    return true;
                }
                else if (keys & KEY_LEFT)
                {
                    tsl::goBack();
                    return true;
                }

                return false;
            });
            list->addItem(slotListItem);
        }

        rootFrame->setContent(list);
        return rootFrame;
    }
};

class GuiItems : public tsl::Gui
{
    u32 *_item;

public:
    GuiItems(u32 *item) : _item(item) {}

    virtual tsl::elm::Element *createUI() override
    {
        auto *rootFrame = new tsl::elm::OverlayFrame("ACNH-Buddy", "v1.0.0 - Item Selector");
        auto list = new tsl::elm::List();

        for (auto item : items)
        {
            auto *itemListItem = new tsl::elm::ListItem(item.second);
            itemListItem->setClickListener([this, itemListItem](u64 keys) {
                if (keys & KEY_A || keys & KEY_RIGHT)
                {
                    std::vector<u32> vec;
                    findByValue(vec, items, itemListItem->getText());
                    *this->_item = vec[0];
                    return true;
                }
                else if (keys & KEY_LEFT)
                {
                    tsl::goBack();
                    return true;
                }

                return false;
            });
            list->addItem(itemListItem);
        }

        rootFrame->setContent(list);
        return rootFrame;
    }
};

class GuiBuddy : public tsl::Gui
{
    u32 *_slot, *_item, *_count;
    bool *_sticky, *_debug;

    tsl::elm::OverlayFrame *frame;
    tsl::elm::List *list;

    tsl::elm::CategoryHeader *slotEditorHeader;
    tsl::elm::ListItem *slotEditorSelector;
    tsl::elm::ListItem *slotEditorItemSelector;
    tsl::elm::ListItem *slotEditorCountSlider;
    tsl::elm::ToggleListItem *slotEditorStickyToggle;
    tsl::elm::ListItem *slotEditorSetButton;

    tsl::elm::CategoryHeader *miscHeader;
    tsl::elm::ToggleListItem *miscDebugToggle;

public:
    GuiBuddy(u32 *slot, u32 *item, u32 *count, bool *sticky, bool *debug) : _slot(slot), _item(item), _count(count), _sticky(sticky), _debug(debug) {}

    virtual tsl::elm::Element *createUI() override
    {
        frame = new tsl::elm::OverlayFrame("ACNH-Buddy", "v1.0.0");
        list = new tsl::elm::List();
        slotEditorHeader = new tsl::elm::CategoryHeader("Slot Editor", true);

        slotEditorSelector = new tsl::elm::ListItem("Inventory Slot", slots.find(*_slot)->second);
        slotEditorSelector->setClickListener([this](u64 keys) {
            if (keys & KEY_A || keys & KEY_RIGHT)
            {
                tsl::changeTo<GuiSlots>(this->_slot);
                return true;
            }
            else if (keys & KEY_LEFT)
            {
                tsl::goBack();
                return true;
            }

            return false;
        });

        slotEditorItemSelector = new tsl::elm::ListItem("Item ID", items.find(*_item)->second);
        slotEditorItemSelector->setClickListener([this](u64 keys) {
            if (keys & KEY_A || keys & KEY_RIGHT)
            {
                tsl::changeTo<GuiItems>(this->_item);
                return true;
            }
            else if (keys & KEY_LEFT)
            {
                tsl::goBack();
                return true;
            }

            return false;
        });

        slotEditorCountSlider = new tsl::elm::ListItem("Item Count", std::to_string(*_count));
        slotEditorCountSlider->setClickListener([this](u64 keys) {
            if (keys & KEY_LEFT)
            {
                if (*this->_count > 1)
                {
                    --*this->_count;
                }
                return true;
            }
            else if (keys & KEY_RIGHT)
            {
                if (*this->_count < 40)
                {
                    ++*this->_count;
                }
                return true;
            }

            return false;
        });

        slotEditorStickyToggle = new tsl::elm::ToggleListItem("Sticky", *this->_sticky);
        slotEditorStickyToggle->setStateChangedListener([this](bool state) {
            *this->_sticky = state;
        });

        slotEditorSetButton = new tsl::elm::ListItem("Set Slot");
        slotEditorSetButton->setClickListener([this](u64 keys) {
            if (keys & KEY_A || keys & KEY_RIGHT)
            {
                u32 id = 0;
                DmntCheatDefinition definition = {
                    {'A', 'C', 'N', 'H', '\0'},
                    4,
                    {0x08100000, *this->_slot, *this->_count, *this->_item}};
                dmntchtAddCheat(&definition, true, &id);
                if (!*this->_sticky)
                {
                    dmntchtToggleCheat(id);
                    dmntchtRemoveCheat(id);
                }
                return true;
            }
            else if (keys & KEY_LEFT)
            {
                tsl::goBack();
                return true;
            }

            return false;
        });

        miscHeader = new tsl::elm::CategoryHeader("Miscellaneous", true);

        miscDebugToggle = new tsl::elm::ToggleListItem("Debug", *this->_debug);
        miscDebugToggle->setStateChangedListener([this](bool state) {
            *this->_debug = state;
        });

        list->addItem(slotEditorHeader);
        list->addItem(slotEditorSelector);
        list->addItem(slotEditorItemSelector);
        list->addItem(slotEditorCountSlider);
        list->addItem(slotEditorStickyToggle);
        list->addItem(slotEditorSetButton);
        list->addItem(miscHeader);
        list->addItem(miscDebugToggle);
        frame->setContent(list);
        return frame;
    }

    virtual void update() override
    {
        slotEditorSelector->setValue(*this->_debug ? int_to_hex(*this->_slot) : slots[*this->_slot]);
        slotEditorItemSelector->setValue(*this->_debug ? int_to_hex(*this->_item) : items[*this->_item]);
        slotEditorCountSlider->setValue(std::to_string(*this->_count));
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override
    {
        return false;
    }
};

class OverlayBuddy : public tsl::Overlay
{
    // TODO: Save these even when overlay is closed. Would static even apply here?
    u32 _slot = slots.begin()->first,
        _item = items.begin()->first,
        _count = 1;
    bool _sticky = true, _debug = false;

public:
    virtual void initServices() override
    {
        dmntchtInitialize();
        dmntchtForceOpenCheatProcess();
    }

    virtual void exitServices() override
    {
        dmntchtExit();
    }

    virtual void onShow() override {}
    virtual void onHide() override {}

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<GuiBuddy>(&_slot, &_item, &_count, &_sticky, &_debug);
    }
};

int main(int argc, char **argv)
{
    items[0x00000050] = "Clackercart";
    items[0x00000053] = "Rocking horse";

    u32 slot_address_base = 0xAC4723D0;
    // u32 slot_address_extended = 0xAC472318; // TODO: This doesn't line up when comparing address offsets to last ACNH version
    // 1:  AC3B90C0; 2889584832
    // 10: AC3B9108; 2889584904
    // 20: AC3B9158; 2889584984
    // 21: AC3B9008; 2889584648
    // 30: AC3B9050; 2889584720
    // 39: AC3B9098; 2889584792
    // 40: AC3B90A0; 2889584800

    for (u32 i = 1, offset = 0; i <= 20; ++i, offset += 8) // TODO: Change comparison to <= 40 when extended range is figured out.
    {
        u32 address = slot_address_base + offset; // TODO: (i <= 20 ? slot_address_base : slot_address_extended) + offset;
        slots[address] = "Slot " + std::to_string(i);
    }

    return tsl::loop<OverlayBuddy>(argc, argv);
}
