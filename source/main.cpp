#define TESLA_INIT_IMPL
#include <tesla.hpp>

static Service g_dmntchtSrv;

class GuiSlots : public tsl::Gui
{
    int *_slot;

public:
    GuiSlots(int *slot) : _slot(slot) {}

    virtual tsl::elm::Element *createUI() override
    {
        auto *rootFrame = new tsl::elm::OverlayFrame("ACNH-Buddy", "v1.0.0 - Item Selector");
        auto list = new tsl::elm::List();

        // Player progression: 20, 30, 40
        for (int i = 1; i <= 40; ++i)
        {
            auto *slotListItem = new tsl::elm::ListItem("Slot " + std::to_string(i));
            slotListItem->setClickListener([this](u64 keys) {
                if (keys & KEY_A || keys & KEY_RIGHT)
                {
                    *this->_slot = 12;
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
public:
    GuiItems() {}

    virtual tsl::elm::Element *createUI() override
    {
        auto *rootFrame = new tsl::elm::OverlayFrame("ACNH-Buddy", "v1.0.0 - Item Selector");
        auto list = new tsl::elm::List();

        rootFrame->setContent(list);
        return rootFrame;
    }
};

typedef struct {
    char readable_name[0x40];
    uint32_t num_opcodes;
    uint32_t opcodes[0x100];
} DmntCheatDefinition;

class GuiTest : public tsl::Gui
{
    int *_slot;
    int *_item;
    bool *_sticky;

public:
    GuiTest(int *slot, int *item, bool *sticky) : _slot(slot), _item(item), _sticky(sticky) {}

    virtual tsl::elm::Element *createUI() override
    {
        auto frame = new tsl::elm::OverlayFrame("ACNH-Buddy", "v1.0.0");
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("Slot Editor", true));
        auto *slotListItem = new tsl::elm::ListItem("Inventory Slot", std::to_string(*_slot));
        slotListItem->setClickListener([this](u64 keys) {
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
        list->addItem(slotListItem);

        auto *itemListItem = new tsl::elm::ListItem("Item IDs", std::to_string(*_item));
        itemListItem->setClickListener([](u64 keys) {
            if (keys & KEY_A || keys & KEY_RIGHT)
            {
                tsl::changeTo<GuiItems>();
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

        auto *stickyToggleItem = new tsl::elm::ToggleListItem("Sticky", *this->_sticky);
        stickyToggleItem->setClickListener([](u64 keys) {
            if (keys & KEY_LEFT)
            {
                tsl::goBack();
                return true;
            }

            return false;
        });
        stickyToggleItem->setStateChangedListener([this](bool state) {
            *this->_sticky = state;
        });
        list->addItem(stickyToggleItem);

        auto *setListItem = new tsl::elm::ListItem("Set Slot");
        setListItem->setClickListener([](u64 keys) {
            if (keys & KEY_A || keys & KEY_RIGHT)
            {
                /*const u8 in = 1;
                DmntCheatDefinition cheat_def;
                cheat_def.readable_name = "Add X to Slot 10";
                cheat_def.num_opcodes = 4;
                cheat_def.opcodes = { 0x08100000, 0xAC472418, 0x00000001, 0x00000c77 };
                serviceDispatchInOut(&g_dmntchtSrv, 65204, in, 6131998,
                    .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcMapAlias | SfBufferAttr_FixedSize },
                    .buffers = { { cheat_def, sizeof(*cheat_def) } },
                );*/
                return true;
            }
            else if (keys & KEY_LEFT)
            {
                tsl::goBack();
                return true;
            }

            return false;
        });
        list->addItem(setListItem);

        frame->setContent(list);
        return frame;
    }

    virtual void update() override
    {
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override
    {
        return false;
    }
};

class OverlayTest : public tsl::Overlay
{
    int _slot = 10;
    int _item = 0x50;
    bool _sticky = false; // TODO: Save even when overlay is closed

public:
    virtual void initServices() override
    {
        smGetService(&g_dmntchtSrv, "dmnt:cht");
        serviceDispatch(&g_dmntchtSrv, 65003);
    }

    virtual void exitServices() override
    {
        serviceClose(&g_dmntchtSrv);
    }

    virtual void onShow() override {}
    virtual void onHide() override {}

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<GuiTest>(&_slot, &_item, &_sticky);
    }
};

int main(int argc, char **argv)
{
    return tsl::loop<OverlayTest>(argc, argv);
}
