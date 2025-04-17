#include "hftsim.hpp"
#include <sol/sol.hpp>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

void bind_hft(sol::state_view lua) {
  using namespace hft;
  lua.new_enum<false>(
      "event", "change_in_interest_rate", hft::event::change_in_interest_rate,
      "inflation_rise", hft::event::inflation_rise, "inflation_fall",
      hft::event::inflation_fall, "deflation", hft::event::deflation,
      "recession", hft::event::recession, "prosperity", hft::event::prosperity,
      "change_in_global_economy", hft::event::change_in_global_economy,
      "reports_of_economic_indicators",
      hft::event::reports_of_economic_indicators, "employment_data_release",
      hft::event::employment_data_release, "gdp_announcement",
      hft::event::gdp_announcement, "trade_balance_report",
      hft::event::trade_balance_report, "central_bank_meeting",
      hft::event::central_bank_meeting, "government_policy_change",
      hft::event::government_policy_change, "new_tax_laws",
      hft::event::new_tax_laws, "fiscal_stimulus", hft::event::fiscal_stimulus,
      "sanctions_announced", hft::event::sanctions_announced,

      "earnings_report", hft::event::earnings_report, "dividend_announcement",
      hft::event::dividend_announcement, "stock_split", hft::event::stock_split,
      "acquisition_merger_news", hft::event::acquisition_merger_news,
      "insider_trading_news", hft::event::insider_trading_news,
      "large_fund_liquidation", hft::event::large_fund_liquidation,
      "short_squeeze", hft::event::short_squeeze, "ipo_announcement",
      hft::event::ipo_announcement,

      "war_breakout", hft::event::war_breakout, "peace_agreement",
      hft::event::peace_agreement, "diplomatic_tensions",
      hft::event::diplomatic_tensions, "election_result",
      hft::event::election_result, "political_instability",
      hft::event::political_instability, "cyberattack_on_infrastructure",
      hft::event::cyberattack_on_infrastructure,

      "natural_disaster", hft::event::natural_disaster, "pandemic_news",
      hft::event::pandemic_news, "energy_crisis", hft::event::energy_crisis,
      "oil_price_spike", hft::event::oil_price_spike, "commodity_price_crash",
      hft::event::commodity_price_crash,

      "opinion_of_the_public", hft::event::opinion_of_the_public,
      "viral_news_on_social_media", hft::event::viral_news_on_social_media,
      "hedge_fund_opinion", hft::event::hedge_fund_opinion,
      "market_sentiment_bullish", hft::event::market_sentiment_bullish,
      "market_sentiment_bearish", hft::event::market_sentiment_bearish,
      "sudden_volume_surge", hft::event::sudden_volume_surge,
      "unexpected_volatility", hft::event::unexpected_volatility,

      "crossing_moving_average", hft::event::crossing_moving_average,
      "price_breaks_support", hft::event::price_breaks_support,
      "price_breaks_resistance", hft::event::price_breaks_resistance,
      "flash_crash_detected", hft::event::flash_crash_detected,

      "nothing", hft::event::nothing);

  lua.new_enum<false>("order_type", "buy", hft::order::type::buy, "sell",
                      hft::order::type::sell);

  lua.new_enum<false>("order_status", "open", hft::order::status::open,
                      "partial", hft::order::status::partial, "closed",
                      hft::order::status::closed, "cancelled",
                      hft::order::status::cancelled);

  lua.new_enum<false>("order_preference", "limit",
                      hft::order::preference::limit, "market",
                      hft::order::preference::market);

  lua.new_usertype<stock>(
      "stock", sol::call_constructor,
      sol::factories([](s in_name, double in_price) -> std::shared_ptr<stock> {
        return std::make_shared<stock>(in_name, in_price);
      }),
      "get_price", &stock::get_price, "fluctuate", &stock::fluctuate,
      "edit_price", &stock::edit_price, "economic_indicators",
      &stock::economic_indicators);
}

extern "C" DLLEXPORT int luaopen_hftsim(lua_State *L) {
  sol::state_view s(L);
  bind_hft(s);
  return 1;
}