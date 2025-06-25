#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "test1.h"

class WoodItem;

class WoodAction {
public:
    virtual void perform(WoodItem &item) = 0;
    virtual ~WoodAction() = default;
};

class WoodItem {
    std::string type;
    double thickness;
    double moisture_content;
    bool is_treated;
    std::vector<WoodAction*> processing_steps;

public:
    WoodItem(const std::string &t, double thick, double moisture, bool treated,
             const std::vector<WoodAction*> &steps)
        : type(t), thickness(thick), moisture_content(moisture),
          is_treated(treated), processing_steps(steps) {}

    std::string getType() const { return type; }
    double getThickness() const { return thickness; }
    double getMoisture() const { return moisture_content; }
    bool isTreated() const { return is_treated; }

    void setMoisture(double m) { moisture_content = m; }
    void setTreated(bool t) { is_treated = t; }

    void process() {
        for (auto *step : processing_steps) {
            step->perform(*this);
        }
    }

    ~WoodItem() {
        for (auto *step : processing_steps) {
            delete step;
        }
    }
};

class CutWood : public WoodAction {
    double cut_length;
public:
    CutWood(double len) : cut_length(len) {}
    void perform(WoodItem &) override {}
};

class DryWood : public WoodAction {
public:
    void perform(WoodItem &item) override { 
        item.setMoisture(item.getMoisture() * 0.8);
    }
};

class TreatWood : public WoodAction {
public:
    void perform(WoodItem &item) override { 
        item.setTreated(true); 
    }
};

class ConditionalTreatment : public WoodAction {
    WoodAction *inner_action;
    std::string condition_type;
    double condition_value;
public:
    ConditionalTreatment(WoodAction *action, const std::string &cond, double val)
        : inner_action(action), condition_type(cond), condition_value(val) {}

    void perform(WoodItem &item) override {
        if (condition_type == "MoistureAbove" && item.getMoisture() > condition_value) {
            inner_action->perform(item);
        }
    }

    ~ConditionalTreatment() { delete inner_action; }
};

class WoodInventory {
    std::vector<WoodItem*> items;
public:
    void addItem(WoodItem *item) { items.push_back(item); }
    const std::vector<WoodItem*> &getItems() const { return items; }

    void processAll() {
        for (auto *item : items) {
            item->process();
        }
    }

    ~WoodInventory() {
        for (auto *item : items) {
            delete item;
        }
    }
};

TEST(WoodTest, AddSingleItem) {
    WoodInventory inv;
    std::vector<WoodAction*> steps = { new CutWood(2.5) };
    inv.addItem(new WoodItem("Oak", 25.0, 12.5, false, steps));
    const auto &items = inv.getItems();
    ASSERT_EQ(items.size(), 1);
    ASSERT_EQ(items[0]->getType(), "Oak");
    return true;
}

TEST(WoodTest, MultipleItemsCount) {
    WoodInventory inv;
    inv.addItem(new WoodItem("Pine", 20.0, 10.0, false, {}));
    inv.addItem(new WoodItem("Maple", 30.0, 8.0, true, {}));
    ASSERT_EQ(inv.getItems().size(), 2);
    return true;
}

TEST(WoodTest, ProcessDrying) {
    WoodInventory inv;
    std::vector<WoodAction*> steps = { new DryWood() };
    WoodItem* teak = new WoodItem("Teak", 15.0, 15.0, false, steps);
    inv.addItem(teak);
    double initial_moisture = teak->getMoisture();
    inv.processAll();
    ASSERT_TRUE(teak->getMoisture() < initial_moisture);
    return true;
}

TEST(WoodTest, ConditionalTreatment) {
    WoodInventory inv;
    std::vector<WoodAction*> steps = { new ConditionalTreatment(new TreatWood(), "MoistureAbove", 10.0) };
    inv.addItem(new WoodItem("Walnut", 18.0, 12.0, false, steps));
    inv.processAll();
    ASSERT_TRUE(inv.getItems()[0]->isTreated());
    return true;
}

TEST(WoodTest, ThicknessUnchangedAfterProcessing) {
    WoodInventory inv;
    std::vector<WoodAction*> steps = { new DryWood(), new TreatWood() };
    double initial_thickness = 20.0;
    WoodItem* mahogany = new WoodItem("Mahogany", initial_thickness, 14.0, false, steps);
    inv.addItem(mahogany);
    inv.processAll();
    ASSERT_EQ(mahogany->getThickness(), initial_thickness);
    return true;
}

TEST(WoodTest, UntreatedWhenMoistureBelowThreshold) {
    WoodInventory inv;
    std::vector<WoodAction*> steps = { new ConditionalTreatment(new TreatWood(), "MoistureAbove", 15.0) };
    WoodItem* cedar = new WoodItem("Cedar", 22.0, 12.0, false, steps);
    inv.addItem(cedar);
    inv.processAll();
    ASSERT_TRUE(!cedar->isTreated());
    return true;
}

int main() {
    RUN_TEST(WoodTest, AddSingleItem);
    RUN_TEST(WoodTest, MultipleItemsCount);
    RUN_TEST(WoodTest, ProcessDrying);
    RUN_TEST(WoodTest, ConditionalTreatment);
    RUN_TEST(WoodTest, ThicknessUnchangedAfterProcessing);
    RUN_TEST(WoodTest, UntreatedWhenMoistureBelowThreshold);
    return 0;
}
