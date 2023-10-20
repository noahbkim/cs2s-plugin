#pragma once

#include <vector>

#include <irecipientfilter.h>

namespace cs2s::sdk::server
{

struct DummyRecipientFilter : public IRecipientFilter
{
    std::vector<CPlayerSlot> recipients;
    bool is_reliable{true};
    bool is_init_message{false};

    [[nodiscard]] bool IsReliable() const override { return this->is_reliable; }
    [[nodiscard]] bool IsInitMessage() const override { return this->is_init_message; }

    [[nodiscard]] int GetRecipientCount() const override { return this->recipients.size(); }
    [[nodiscard]] CPlayerSlot GetRecipientIndex(int slot) const override { return this->recipients[slot]; }
};

}
